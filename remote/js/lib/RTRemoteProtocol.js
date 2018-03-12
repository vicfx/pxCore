/**
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */

/**
 * The rt remote protocol
 *
 * @author      TCSCODER
 * @version     1.0
 */


const RTMessageHelper = require('./RTMessageHelper');
const RTConst = require('./RTConst');
const RTStatusCode = require('./RTStatusCode');
const RTRemoteSerializer = require('./RTRemoteSerializer');
const RTEnvironment = require('./RTEnvironment');
const RTRemoteMessageType = require('./RTRemoteMessageType');
const helper = require('./common/helper');
const logger = require('./common/logger');

/**
 * the rt remote protocol class
 */
class RTRemoteProtocol {
  /**
   * create new RTRemoteProtocol
   * @param {RTRemoteTCPTransport} transport the remote transport
   * @param {boolean} transportOpened is the transport opened or not
   */
  constructor(transport, transportOpened) {
    /**
     * the remote transport
     * @type {RTRemoteTCPTransport}
     */
    this.transport = transport;

    /**
     * is the transport opened or not
     * @type {boolean}
     */
    this.transportOpened = transportOpened;

    /**
     * the buffer queue, used to cache and process packet
     * @type {Buffer}
     */
    this.bufferQueue = Buffer.alloc(0);

    /**
     * this mean protocol is runing or not
     * @type {boolean}
     */
    this.mRunning = false;

    /**
     * the call context map (promise map)
     * @type {object}
     */
    this.futures = {};
  }

  /**
   * init protocol, it will open tranport first iftransport not opened
   * @return {Promise<RTRemoteProtocol>} promise with RTRemoteProtocol instance
   */
  init() {
    return new Promise((resolve, reject) => {
      if (!this.transportOpened) {
        this.transport.open().then(() => { // open first
          this.mRunning = true;
          this.start();
          resolve(this);
        }).catch(err => reject(err));
      } else {
        this.mRunning = true;
        this.start();
        resolve(this);
      }
    });
  }

  /**
   * start to read message from transport
   */
  start() {
    const that = this;
    this.transport.socket.on('data', (data) => {
      that.bufferQueue = Buffer.concat([that.bufferQueue, Buffer.from(data)]);
      if (that.bufferQueue.length > RTConst.PROTOCOL_HEADER_LEN) { // parse head length
        const packetLen = that.bufferQueue.readUInt32BE(0);
        const totalLen = packetLen + RTConst.PROTOCOL_HEADER_LEN;
        if (that.bufferQueue.length >= totalLen) {
          // this mean is a full packet, this packet can prase as message
          const messageBuffer = Buffer.alloc(packetLen);
          that.bufferQueue.copy(messageBuffer, 0, RTConst.PROTOCOL_HEADER_LEN, totalLen);
          that.inComeMessage(RTRemoteSerializer.fromBuffer(messageBuffer));
          that.bufferQueue = that.bufferQueue.slice(totalLen); // remove parsed message
        }
      }
    });
  }

  /**
   * in come a message from other side
   * @param {object} message the remote message object
   */
  inComeMessage(message) {
    const key = message[RTConst.CORRELATION_KEY];
    const callContext = this.futures[key];
    if (callContext) { // call context
      if (callContext.expired) { // call timeout, already return error, so ignore this
      } else {
        clearTimeout(callContext.timeoutHandler); // clear timeout handler
      }

      if (message[RTConst.STATUS_CODE] !== RTStatusCode.OK) { // status error, reject directly
        callContext.reject(helper.getStatusStringByCode(message[RTConst.STATUS_CODE]));
      } else {
        callContext.resolve(message[RTConst.VALUE] || message[RTConst.FUNCTION_RETURN_VALUE]); // resolve
      }
    } else if (message[RTConst.MESSAGE_TYPE] === RTRemoteMessageType.KEEP_ALIVE_REQUEST) {
      // other side send keep a live request, so must return keep alive response with status ok
      this.transport.send(RTRemoteSerializer
        .toBuffer(RTMessageHelper.newKeepAliveResponse(message[RTConst.CORRELATION_KEY], RTStatusCode.OK)));
    } else if (message[RTConst.MESSAGE_TYPE] === RTRemoteMessageType.KEEP_ALIVE_RESPONSE) {
      // this send keep live request to other side, and got reponse from other side
      // so this reponse can ignored
    } else if (RTEnvironment.isServerMode()) {
      this.processMessageInServerMode(message);
    } else {
      this.processMessageInClientMode(message);
    }
  }

  /**
   * process message in client mode
   * @param {object} message the message
   */
  processMessageInClientMode(message) {
    if (message[RTConst.MESSAGE_TYPE] === RTRemoteMessageType.METHOD_CALL_REQUEST) {
      const functionCb = RTEnvironment.getRtFunctionMap()[message[RTConst.FUNCTION_KEY]];
      if (functionCb) {
        functionCb(message[RTConst.FUNCTION_ARGS]);
      }
      this.sendCallResponse(message[RTConst.CORRELATION_KEY]);
    } else {
      logger.error(`unexpected message ${message}`);
    }
  }

  /**
   * process message in server mode
   * @param {object} message the message
   */
  processMessageInServerMode(message) {
    // TODO need implement in next challenge
    logger.debug(message, this);
  }

  /**
   * send call response to other side
   * @param {string} correlationKey the call request correlation key
   */
  sendCallResponse(correlationKey) {
    this.transport.send(RTRemoteSerializer
      .toBuffer(RTMessageHelper.newCallResponse(correlationKey, null, RTStatusCode.OK)));
  }

  /**
   * send set property by name
   * @param {string} objectId the object id
   * @param {string} propName the property name
   * @param {object} value the rtValue
   * @return {promise<object>} promise with result
   */
  sendSetByName(objectId, propName, value) {
    const messageObj = RTMessageHelper.newSetRequest(objectId, propName, value);
    return this.sendRequestMessage(messageObj);
  }

  /**
   * send set property by id
   * @param {string} objectId the object id
   * @param {number} index the property index
   * @param {object} value the rtValue
   * @return {Promise<{}>} promise with result
   */
  sendSetById(objectId, index, value) {
    // TODO need implement
    logger.debug(objectId, index, value);
    logger.error('sendSetById didn\'t implement', this);
    return Promise.resolve({});
  }

  /**
   * send call request by method name
   * @param {string} objectId the object id
   * @param {string} methodName the method name
   * @param {array} args the call function args
   * @return {Promise<object>} promise with returned rtValue
   */
  sendCallByName(objectId, methodName, ...args) {
    const messageObj = RTMessageHelper.newCallMethodRequest(objectId, methodName, ...args);
    return this.sendRequestMessage(messageObj);
  }

  /**
   * send request message to other side
   * @param {object} messageObj the message object
   * @return {Promise<object>} the promise with returned rtValue
   */
  sendRequestMessage(messageObj) {
    const callContext = {};
    this.futures[messageObj[RTConst.CORRELATION_KEY]] = callContext;
    this.transport.send(RTRemoteSerializer.toBuffer(messageObj));
    return new Promise((resolve, reject) => {
      callContext.resolve = resolve;
      callContext.reject = reject;
      callContext.expired = false;
      callContext.timeoutHandler = setTimeout(() => {
        callContext.expired = true;
        reject(new Error(helper.getStatusStringByCode(RTStatusCode.TIMEOUT)));
      }, RTConst.REQUEST_TIMEOUT);
    });
  }

  /**
   * send get property by name
   * @param {string} objectId the object id
   * @param {string} propName the property name
   * @return {Promise<object>} promise with result
   */
  sendGetByName(objectId, propName) {
    const messageObj = RTMessageHelper.newGetRequest(objectId, propName);
    return this.sendRequestMessage(messageObj);
  }

  /**
   * send get property by name
   * @param {string} objectId the object id
   * @param {string} index the property name
   * @return {Promise<object>} promise with result
   */
  sendGetById(objectId, index) {
    // TODO need implement
    logger.debug(objectId, index);
    logger.error('sendSetById didn\'t implement', this);
    return Promise.resolve({});
  }
}

/**
 * create new RTRemoteProtocol
 * @param {RTRemoteTCPTransport} transport the connection tranport
 * @param {boolean} transportOpened is the transport opened or not
 * @return {Promise<RTRemoteProtocol>} the promise with protocol
 */
function create(transport, transportOpened) {
  const protocol = new RTRemoteProtocol(transport, transportOpened);
  return protocol.init();
}

module.exports = {
  create,
};