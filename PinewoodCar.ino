#include <TinyWireM.h>
#include <Adafruit_TinyLSM303.h>
#include <Adafruit_NeoPixel.h>

#define MOVE_THRESHOLD 10000      // Adjust this threshold as needed to trigger led motion. 
#define PIXEL_OUTPUT 1            // Trinket pin that the NeoPixel stick is connected to.   
#define NUM_PIXELS 8            
byte brightness = 40;             // Set to a low value to conserve power on the small LiPo battery.
int32_t DefaultColor = 0xffbf00;  // Set the "glowing" color for when the car isn't moving.
                                  // Note when the car is at rest, only every-other pixel will be on. This
								  //   was done to conserve battery power.

int32_t  accelx = 0;              // I'm only interested in the x-direction accelerometer data.
                                  // Note that the X-axis arrow on the accelerometer points to the front of the car.
uint32_t color = 0xff0000;
boolean changeColor = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_TinyLSM303 lsm;

void setup(){
  pixels.begin();
  pixels.setBrightness(brightness);
  setColor(DefaultColor);
  lsm.begin();
}

void loop(){
  lsm.read();
  accelx = (int32_t)lsm.accelData.x;
  if(abs(accelx) > MOVE_THRESHOLD){
    if(changeColor){  // This section changes the color of the scrolling pixels after the car has gone from moving to rest.
      color >>= 8;    // Color progression is red -> green -> blue then start the color cycle over again.
      if(!color) color = 0xff0000;
      changeColor = false;
    }    
    setColor(pixels.Color(0, 0, 0));
    pixels.show();
    pixels.setBrightness(180);                     // Boost the pixel brightness during motion.
    for(int i=NUM_PIXELS-1; i>=0; i--){            // This loop controls the scrolling motion of the pixels.
      pixels.setPixelColor(i, color);
      if(i < NUM_PIXELS-1){
        pixels.setPixelColor(i+1,pixels.Color(0, 0, 0));
      }
      pixels.show();
	  // The use of delay() below is not optimal. Future code improvement opportunity.
      if(abs(accelx) > MOVE_THRESHOLD+4000){       // If moving super fast make pixels scroll super fast. Adjust +xxxx as needed.
        delay(10);
      }
      else if(abs(accelx) > MOVE_THRESHOLD+2000){  // If moving fairly fast make pixels scroll fairly fast. Adjust +xxxx as needed.
        delay(20);
      }
      else{  // Default pixel scrolling speed.
        delay(30);
      }
    }
    pixels.show();
    setColor(pixels.Color(0, 0, 0));
  }
  else{
    changeColor = true;
    pixels.setBrightness(brightness);              // Reset to power-saving brightness.
    setColor(DefaultColor);                        // Reset to default pixel color.
  }
}

void setColor(uint32_t color){
  // This function sets the color for every-other pixel.
  // This was done as another way to conserve battery power.
  
  for (int i = 0; i < NUM_PIXELS; i+=2) pixels.setPixelColor(i,color);
  pixels.show();
}



