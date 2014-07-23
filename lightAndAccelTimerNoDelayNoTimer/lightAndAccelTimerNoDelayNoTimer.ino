//#include <Adafruit_LSM303.h>

//#include <Wire.h>

#include <Adafruit_TinyLSM303.h>
#include <TinyWireM.h>
#include <USI_TWI_Master.h>
#include <Adafruit_NeoPixel.h>


Adafruit_TinyLSM303 lsm;
const int shakeDif=3500;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(13, 1, NEO_GRB + NEO_KHZ800);


void setup() 
{
  lsm.begin();
  strip.begin();
  strip.show();
}

void loop() 
{
  countShakes();
  delay(5);
}

void countShakes()
{
  boolean newShakeStarted=false;
  int c=0;
  uint32_t lastColor;
  if (shake()) //only start timer after first shake
  {
    long lastShakeTime=millis();
    while (!newShakeStarted) //count # of shakes before # millisec passes
    {
      if (shake())
      { 
        if (millis()-lastShakeTime<300) //if shakes have no significant gap inbetween
        {
           c++;
           lastShakeTime=millis();
           if (c==5) //#/2 +0.25 is the min num of jolts, more wont register
           {
             lastColor=changeColor();
           }
           if (c>=11)
           {
             sparkle(lastColor);
             newShakeStarted=true;
            // Serial.println("Reset");
           }
        }
        else
        {
          newShakeStarted=true;
        }
      }
    }
  }
}

void sparkle(uint32_t color)
{
  boolean evenLit=false;
  long lastShake=millis();
  while (millis()-lastShake<2500)
  {
    if (shake())
    {
      lastShake=millis();
    }
      for (int i=0; i<strip.numPixels(); i+=2)
      {
        if (!evenLit)
        {
          strip.setPixelColor(i,color);
          if (i-1>0)
          {
            strip.setPixelColor(i-1,strip.Color(0,0,0));
          }
        }
        else
        {
          strip.setPixelColor(i,strip.Color(0,0,0));
          if (i-1>0)
          {
             strip.setPixelColor(i-1,color);
          }
        }  
      }
    evenLit=!evenLit;
    strip.show();
    delay(1);
  }
    for (int i=0; i<strip.numPixels(); i++)
    {
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
    strip.show();
}

uint32_t changeColor()
{
  int randomHueR=(int)(random(0,256));//(int)(random(0,256)/8);
  int randomHueG=(int)(random(0,256));
  int randomHueB=(int)(random(0,256));
   for (int i=0; i<strip.numPixels(); i++)
   {
      strip.setPixelColor(i,strip.Color(randomHueR, randomHueG, randomHueB));
   }
   strip.show();
   //Serial.println("Change");
   return strip.Color(randomHueR,randomHueG, randomHueB);
}
    
boolean shake()
 {
      int x1, y1, z1, xDif, yDif, zDif, shakeCount;
      long firstRead=millis();
      lsm.read();
      x1=(int)lsm.accelData.x;
      y1=(int)lsm.accelData.y;
      z1=(int)lsm.accelData.z;
      while (millis()-firstRead<=160)
      {
         lsm.read();
         xDif=abs((int)(lsm.accelData.x-x1));
         yDif=abs((int)(lsm.accelData.y-y1));
         zDif=abs((int)(lsm.accelData.z-z1));
         shakeCount=0;
         if (xDif>shakeDif) shakeCount++;
         if (yDif>shakeDif) shakeCount++;
         if (zDif>shakeDif) shakeCount++;
         if (shakeCount>=2) return true;
      }
      //Serial.print("X: "); Serial.print(xDif); Serial.print(", Y: "); Serial.print(yDif); Serial.print(", Z: "); Serial.println(zDif);     
      //Serial.println(shakeCount);
      return false; //no shake within 40 ms
    }
 
