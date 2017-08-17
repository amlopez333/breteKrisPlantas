
#include <Adafruit_NeoPixel.h> //library for neopixels
#define WEIGHT_PIN 9 //strip pin for wieght neopixels
#define N_LEDS_WEIGHT 20 //number of leds on strip. THIS MUST MATCH THE EXACT NUMBER ON THE STRIP OTHERWISE YOU GET UNEXPECTED BEHAVIOR
#define SUNLIGHT_PIN 12 //strip pin for sunlight neopixels
#define N_LEDS_SUNLIGHT 40 //number of leds on strip. THIS MUST MATCH THE EXACT NUMBER ON THE STRIP OTHERWISE YOU GET UNEXPECTED BEHAVIOR
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS_WEIGHT, WEIGHT_PIN, NEO_GRB + NEO_KHZ800); //weight strip
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS_SUNLIGHT, SUNLIGHT_PIN, NEO_GRB + NEO_KHZ800);  //sunlight strip
int fsrPin = A5; //FSR PIN    
const float VCC = 4.98; //VOLTAGE measured 
const float R_DIV = 1000.0; //Resistance measured
long fsrForce; //variable that stores the force       
const int threshold = 100; //threshold for force. When fsrForce is less than threshold, lights blink red
int timer = 0; //timer
int timeOff = 15; //time that indicates when to turn off strip2
int timeOn = 30; //time that indicates when to turn on strip1
const int button = 4; //button PIN
int val = 0; //button value
int currentColor = 0; //current color 0: yellow, 1: red, 2: blue
boolean isFlashing = false; //state control 


/*Setup
 * initiates strips and serial. 
 */
void setup() {
  Serial.begin(9600);
  strip1.begin();
  strip1.show();
  strip2.begin();
  strip2.show();
  pinMode(button, INPUT);
  chase(strip1.Color(255, 0, 0));
  chase(strip1.Color(0, 255, 0));
  chase(strip1.Color(0, 0, 255));
  chase2(strip2.Color(255, 0, 0));
  chase2(strip2.Color(0, 255, 0));
  chase2(strip2.Color(0, 0, 255));
  turnOn();
  restorePreviousColor();
}

// main loop
void loop() {
  timer++;
  //resets timer
  if(timer > timeOn){
    timer = 0;
    turnOn();
  }
  val = digitalRead(button);
  Serial.println(val);
  setColor();
  int fsrADC = analogRead(fsrPin);  
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
    // Use ADC reading to calculate voltage:
  float fsrV = fsrADC * VCC / 1023.0;
  // Use voltage and static resistor value to 
  // calculate FSR resistance:
  float fsrR = R_DIV * (VCC / fsrV - 1.0);
  Serial.println("Resistance: " + String(fsrR) + " ohms");
  // Guesstimate force based on slopes in figure 3 of
  // FSR datasheet:
  float force;
  float fsrG = 1.0 / fsrR; // Calculate conductance
  // Break parabolic curve down into two linear slopes:
  // determines force. If you want a different threshold, read serial value and make threshold that value
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
  Serial.println("Force: " + String(force) + " g/s^2");
  //Serial.println();
  if(force < threshold){
    isFlashing = true;
    flashRed(); 
  }
  else{
    isFlashing = false;
    restorePreviousColor();
  }
  //
  Serial.print("time: ");
  Serial.println(timer);
  Serial.println("--------------------");
  turnOnOff();
  delay(1000);
}

/*
 * Handles turning on and off strip2
 */
static void turnOnOff(){
  if(timer == timeOff){
    turnOff();
  }
  else if(timer == timeOn){
    turnOn();
  }
}

/*
 *Turns off strip2 
 */
static void turnOff(){
  for(uint16_t i=0; i<strip2.numPixels()+4; i++){
      strip2.setPixelColor(i, strip2.Color(0, 0, 0)); 
      strip2.show();
      delay(25);
  }
}

/*
 *Turns on strip2 
 */
static void turnOn(){
  for(uint16_t i=0; i<strip2.numPixels()+4; i++){
      strip2.setPixelColor(i, strip2.Color(143, 1, 155)); 
      strip2.show();
      delay(25);
  }
  
}

/*
 * Sets color after button push
 */
static void setColor(){
  if(isFlashing){
    return;
  }
  if(val == HIGH && currentColor == 0){
    setYellow(); 
    currentColor++;
  }
  else if(val == HIGH && currentColor == 1){
    setRed();
    currentColor++;
  }
  else if(val == HIGH && currentColor == 2){
    //timer = 0;
    setBlue();
    currentColor = 0;
  }
}

/*
 * Indicator when arduino is plugged in and program begins
 */
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, c); // Draw new pixel
      strip1.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip1.show();
      delay(10);
  }
}

/*
 * Indicator when arduino is plugged in and program begins
 */
static void chase2(uint32_t c) {
  for(uint16_t i=0; i<strip2.numPixels()+4; i++) {
      strip2.setPixelColor(i, c); // Draw new pixel
      strip2.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip2.show();
      delay(10);
  }
}

/*
 * Flashes lights using color red
 */
static void flashRed(){
    for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 0, 0));
      strip1.show();
    }
    for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(0, 0, 0)); 
      strip1.show();
    }
}

/*
 * Restore previous color after flashing ends and force is equal to or greater than threshold
 */
static void restorePreviousColor(){
   if(currentColor == 0){
    setYellow(); 
  }
  else if(currentColor == 1){
    setRed();
  }
  else if(currentColor == 2){
    setBlue();
  }
}

/*
 * Sets strip1 to color blue
 */
static void setBlue(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(0, 0, 255));
      strip1.show();
    }
}

/*
 * Sets strip1 to color yellow
 */
static void setYellow(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 255, 0));
      strip1.show();
    }
}

/*
 * Sets strip1 to color red
 */
static void setRed(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 0, 0));
      strip1.show();
    }
}

