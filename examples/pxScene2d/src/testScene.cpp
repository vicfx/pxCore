// pxCore CopyRight 2007-2015 John Robinson
// testScene.cpp

#include "testScene.h"

#include "rtPathUtils.h"
#include "pxScene2d.h"
#include "pxText.h"
#include "pxImage.h"
#include "pxKeycodes.h"

#include "pxInterpolators.h"


#define USE_BG
#define USE_BG_RADIAL
#define USE_TEXT
#define USE_PICTURE

#if 0

// Using C++ API... 


void testScene()
{
  rtString d;
  rtGetCurrentDirectory(d);
  d.append("/../images/banana.png");

  scene->init();

  pxObjectRef root = scene->getRoot();
  
  int n = 3;
  int nx = 100;
  int ny = 200;
  for (int i = 0; i < n; i++)
  {
    pxObjectRef p;

    if (i == 0)
    {
      rtRefT<rectangle9> r = new rectangle9();

      float c1[4] = {1,0,0,1};      
      float c2[4] = {1,1,1,0.5};

      r->setFillColor(c1);
      r->setLineColor(c2);
      r->setLineWidth(10);

      r->setW(300);
      r->setH(30);

      p = r;
      p->animateTo("h", 600, 0.2, pxInterpLinear, seesaw);
    }
    else if (i == 1)
    {
      rtRefT<pxText> t = new pxText();
      t->setText("Hello");
      p = t;
    }
    else
    {
      pxImage* i = new pxImage();
      p = i;
      i->setURL(d.cString());
      p->setCX(i->w()/2);
      p->setCY(i->h()/2);
    }

    nx += 10;
    if (nx > 1000)
    {
      nx = 0;
      ny += 10;
    }
    p->setX(nx);
    p->setY(ny);



    p->setRX(0);
    p->setRY(0);
    p->setRZ(1);

    p->setParent(root);
    
    p->animateTo("r", 360, 2.0, pxInterpLinear, loop);
    p->animateTo("x", 800, 2.0, pxInterpLinear, seesaw);
    p->animateTo("a", 0.5, 2.0, pxInterpLinear, seesaw);
  }
}
#elseif 0

// Using Dynamic rtObject API
void testScene() {
      printf("create new banana!\n");
  
  rtString d;
  rtGetCurrentDirectory(d);
  rtString d2 = d;
  d.append("/../images/banana.png");
  d2.append("/../images/curve_rectangle.png");

  scene->init();

  rtObjectRef root = scene.get<rtObjectRef>("root");  
  
  int n = 10;
  int nx = 100;
  int ny = 100;
  for (int i = 0; i < n; i++) {
    
    rtObjectRef p;

    if (i < 1) {
      scene.sendReturns<rtObjectRef>("createRectangle", p);
      p.set("w", 300);
      p.set("h", 30);
      p.set("fillColor",0x00ff00ff);
      p.set("lineColor",0xffffff80);
      p.set("lineWidth", 10);  
      p.send("animateTo", "h", 600, 0.5, 0, 0);
  }
    else if (i < 2){
      scene.sendReturns<rtObjectRef>("createImage9", p);
      p.set("url", d2);
      p.set("cx", p.get<float>("w")/2);
      p.set("cy", p.get<float>("h")/2);
      ny = 100;
      p.send("animateTo", "h", 600, 0.5, 0, 0);
      p.send("animateTo", "w", 600, 0.5, 0, 0);
    }
#if 1
    else if (i < n-3){
      scene.sendReturns<rtObjectRef>("createImage", p);
      p.set("url", d);
      p.set("cx", p.get<float>("w")/2);
      p.set("cy", p.get<float>("h")/2);
      ny = 100;
    }
#endif
    else {
      scene.sendReturns<rtObjectRef>("createText", p);
      p.send("animateTo", "sx", 2.0, 1.0, 0, 0);
      p.send("animateTo", "sy", 2.0, 1.0, 0, 0);
      nx = 200;
      if (i == n-3) {
        // utf8 test
        p.set("text", "Iñtërnâtiônàližætiøn");
        p.set("textColor", 0xffff00ff);
        ny = 200;
      }
      else if (i == n-2) {
        p.set("text", "pxCore!");
        p.set("textColor", 0xff0000ff);
        ny = 300;
      }
      else if (i == n-1) {
        //utf8 test...
        p.set("text", "Ādam");
        p.set("textColor", 0x00ffffff);
        ny = 400;
      }
      p.set("cx", p.get<float>("w")/2);
    }

    nx += 10;
    if (nx > 1000) {
      nx = 0;
      ny += 10;
    }

    p.set("parent", root);
    p.set("x", nx);
    p.set("y", ny);

    p.set("rx", 0);
    p.set("ry", 0);
    p.set("rz", 1);

    p.send("animateTo", "r", 360, 1.0+(i*0.3), 0, 1);
    if (i < n-1) {
      p.send("animateTo", "x", 600, 1.0+(i*0.3), 0, 0);
    }

    // Demonstrate how to invoke same function with a rtFunctionRef
    rtFunctionRef f = p.get<rtFunctionRef>("animateTo");
    f.send("a", 0.1, 2.0, 0, 0);

  }
}


#endif

#if 0
void fancy(void* ctx);
void x1(void* ctx);
void x2(void* ctx);
void y1(void* ctx);
void y2(void* ctx);
void rotate1(void* ctx);
void scale1(void* ctx);
void scale2(void* ctx);
void scale3(void* ctx);

void fancy(void* ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  x1((void*)p);
  y1((void*)p);
  rotate1((void*)p);
  scale1((void*)p);
}
void x1(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("x", 50, 1.0, pxInterpLinear, stop, x2, (void*)p);
}

void x2(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("x", 450, 2.0, easeOutElastic, stop, fancy, (void*)p);
}

void y1(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->setY(100);
  p->animateTo("y", 350, 1.0, easeOutBounce, stop, y2, (void*)p);
}

void y2(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("y", 150, 1.0, easeOutElastic, stop);
}

void rotate1(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->setR(0);
  p->animateTo("r", -360, 2.5, easeOutElastic, stop);
}

void scale1(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("sx", 0.2, 1, pxInterpLinear, stop, scale2, (void*)p);
  p->animateTo("sy", 0.2, 1, pxInterpLinear, stop);
}

void scale2(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("sx", 2.0, 0.5, exp2, stop, scale3, (void*)p);
  p->animateTo("sy", 2.0, 0.5, exp2, stop);
}

void scale3(void*ctx) {
  rtRefT<pxObject> p = (pxObject*)ctx;
  p->animateTo("sx", 1.0, 1.0, easeOutElastic, stop);
  p->animateTo("sy", 1.0, 1.0, easeOutElastic, stop);
}
#endif


#if 0
void ballScene() 
{
  
  rtString d;
  rtGetCurrentDirectory(d);
  rtString d2 = d;
  rtString d3 = d;
  d.append("/../images/skulls.png");
  d2.append("/../images/radial_gradient.png");
  d3.append("/../images/ball.png");

  scene->init();

  rtObjectRef root = scene.get<rtObjectRef>("root");  

  rtObjectRef bg;
  scene.sendReturns<rtObjectRef>("createImage", bg);
  bg.set("url", d);
  bg.set("xStretch", 2);
  bg.set("yStretch", 2);
  bg.set("parent", root);
  bg.set("w", scene->w());
  bg.set("h", scene->h());

  scene.sendReturns<rtObjectRef>("createImage", bg);
  bg.set("url", d2);
  bg.set("xStretch", 1);
  bg.set("yStretch", 1);
  bg.set("parent", root);
  bg.set("w", scene->w());
  bg.set("h", scene->h());

  rtRefT<pxImage> p = new pxImage();
  p->setURL(d3);
  p->setParent(root);
  p->setX(450);
  p->setY(350);
//  fancy((pxObject*)p);
}

#endif


#if 1
#ifdef USE_BG
rtObjectRef bg1;
#endif

#ifdef USE_BG_RADIAL
rtObjectRef bg2;
#endif

#ifdef USE_PICTURE
rtObjectRef picture;
rtString bananaURL;
rtString ballURL;
#endif

rtError onSizeCB(int numArgs, const rtValue* args, rtValue* /*result*/, void* /*context*/)
{
  if (numArgs == 2)
  {
    rtObjectRef e = args[0].toObject();
    int w = e.get<uint32_t>("w");
    int h = e.get<uint32_t>("h");

#ifdef USE_BG
    bg1.set("w", w);
    bg1.set("h", h);
#endif

#ifdef USE_BG_RADIAL
    bg2.set("w", w);
    bg2.set("h", h);
#endif
  }
  return RT_OK;
}

rtError onKeyDownCB(int numArgs, const rtValue* args, rtValue* /*result*/, void* /*context*/)
{

  if (numArgs >0)
  {

    rtObjectRef e = args[0].toObject();
    uint32_t keyCode = e.get<uint32_t>("keyCode");
    printf("received keyCode %d\n", keyCode);

#ifdef USE_PICTURE
    switch(keyCode) 
    {
    case PX_KEY_ONE: // '1'
      printf("banana\n");
      picture.set("url", bananaURL);

      printf("\n ####################### bananaURL");

      break;

    case PX_KEY_TWO: // '2'
      printf("ball\n");
      picture.set("url", ballURL);

      printf("\n ####################### ballURL");

      break;
    default:
      rtLogWarn("unhandled key");
      break;
    }
#endif

  }
  return RT_OK;
}

void testScene()
{
  rtLogSetLevel(RT_LOG_DEBUG);

  pxScene2dRef scene = new pxScene2d;

  rtString d;
  rtString bgURL;
  rtGetCurrentDirectory(d);

#ifdef USE_PICTURE

  bananaURL = d;
  ballURL   = d;
  bananaURL.append("/../images/banana.png");
  ballURL.append("/../images/ball.png");
#endif

  scene->init();

  rtObjectRef root = scene.get<rtObjectRef>("root");  

  root.send("on",  "onKeyDown", new rtFunctionCallback(onKeyDownCB));
  scene.send("on", "onResize",  new rtFunctionCallback(onSizeCB));

#ifdef USE_BG
  scene.sendReturns<rtObjectRef>("createImage", bg1);
  bgURL = d;
  bgURL.append("/../images/skulls.png");
  bg1.set("url", bgURL);
  bg1.set("xStretch", 2);
  bg1.set("yStretch", 2);
  bg1.set("parent", root);
  bg1.set("w", scene->w());
  bg1.set("h", scene->h());
#endif


#if 0
  printf("Try enumerating properties on image.\n");
  rtObjectRef keys = bg1.get<rtObjectRef>("allKeys");
  uint32_t length = keys.get<uint32_t>("length");
  for (uint32_t i = 0; i < length; i++)
  {
    printf("i: %d key: %s\n", i, keys.get<rtString>(i).cString());
  }
#endif

#ifdef USE_BG_RADIAL
  scene.sendReturns<rtObjectRef>("createImage", bg2);
  bgURL = d;
  bgURL.append("/../images/radial_gradient.png");
  bg2.set("url", bgURL);
  bg2.set("xStretch", 1);
  bg2.set("yStretch", 1);
  bg2.set("parent", root);
  bg2.set("w", scene->w());
  bg2.set("h", scene->h());
#endif

#ifdef USE_TEXT
  rtObjectRef t;
  scene.sendReturns<rtObjectRef>("createText", t);
  t.set("text", "Choose Picture:\n"
        "1. Banana!\n"
        "2. Ball!\n");
  t.set("x", 100);
  t.set("y", 100);
  t.set("parent", root);

#endif

#ifdef USE_PICTURE
  scene.sendReturns<rtObjectRef>("createImage", picture);
  picture.set("x", 400);
  picture.set("y", 400);
  // TODO animateTo now takes a property bag of properties and targets too lazy to fix this call right now
  //  picture.send("animateTo", "r", 360, 0.5, 0, 1);
  picture.set("parent", root);

#ifndef USE_TEXT
  picture.set("url", bananaURL);
#endif

  picture.set("url", ballURL);
#endif


#if 0

  printf("Enumerate children of root object\n");
  rtObjectRef c = root.get<rtObjectRef>("children");
  uint32_t l = c.get<uint32_t>("length");

#if 1
  for (uint32_t i = 0; i < l; i++)
  {
    rtObjectRef o = c.get<rtObjectRef>(i);
    rtString s;
    o.sendReturns<rtString>("description", s);
    printf("class description: %s\n", s.cString());
  }
#endif

#endif //00

//  return scene.getPtr();
}

#endif

