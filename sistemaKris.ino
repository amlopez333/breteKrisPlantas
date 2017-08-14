

int fsrPin = A5;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
int fsrVoltage;
int fsrWeight;// the analog reading converted to voltage
unsigned long fsrResistance;  // The voltage converted to resistance, can be very big so make "long"
unsigned long fsrConductance; 
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 47.0; // Measured resistance of 3.3k resistor
long fsrForce;       // Finally, the resistance converted to force
const int threshold = 100;
int timer = 0;
const int button = 4;
int val = 0;
int currentColor = 0;
boolean isFlashing = false;
#include <Adafruit_NeoPixel.h>
 
#define WEIGHT_PIN 9
#define N_LEDS 60
#define SUNLIGHT_PIN 6
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, WEIGHT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, WEIGHT_PIN, NEO_GRB + NEO_KHZ800); 
void setup() {
  Serial.begin(9600);
  strip1.begin();
  strip1.show();
  pinMode(button, INPUT);
  chase(strip1.Color(255, 0, 0));
  chase(strip1.Color(0, 255, 0));
  chase(strip1.Color(0, 0, 255));
}
 
void loop() {
  timer++;
  if(timer > 30){
    timer = 0;
    turnOn();
  }
  val = HIGH; //digitalRead(button);
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
  if (fsrR <= 600) 
    force = (fsrG - 0.00075) / 0.00000032639;
  else
    force =  fsrG / 0.000000642857;
  Serial.println("Force: " + String(force) + " g");
  //Serial.println();
  if(force < threshold){
    isFlashing = true;
    flashRed(); 
  }
  else{
    isFlashing = false;
    setColor();
  }
  //Serial.println("--------------------");
  Serial.println(timer);
  turnOnOff();
  delay(1000);
}
static void turnOnOff(){
  if(timer == 15){
    turnOff();
  }
  else if(timer == 30){
    turnOn();
  }
}

static void turnOff(){
  for(uint16_t i=0; i<strip2.numPixels()+4; i++){
      strip2.setPixelColor(i, strip1.Color(0, 0, 0)); 
      strip2.show();
      delay(25);
  }
}
static void turnOn(){
  for(uint16_t i=0; i<strip2.numPixels()+4; i++){
      strip2.setPixelColor(i, strip2.Color(255, 0, 255)); 
      strip2.show();
      delay(25);
  }
  
}
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
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, c); // Draw new pixel
      strip1.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip1.show();
      delay(10);
  }
}

void flashRed(){
    for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 0, 0));
      strip1.show();
    }
    for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(0, 0, 0)); 
      strip1.show();
    }
}

void setBlue(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(0, 0, 255));
      strip1.show();
    }
}
void setYellow(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 255, 0));
      strip1.show();
    }
}
void setRed(){
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 0, 0));
      strip1.show();
    }
}
/*void setup(void) {
  Serial.begin(9600);   // We'll send debugging information via the Serial monitor
}
 
void loop(void) {
  fsrReading = analogRead(fsrPin);  
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
 
  // analog voltage reading ranges from about 0 to 1023 which maps to 0V to 5V (= 5000mV)
  fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
  Serial.print("Voltage reading in mV = ");
  Serial.println(fsrVoltage);  
 
  if (fsrVoltage == 0) {
    Serial.println("No pressure");  
  } else {
    // The voltage = Vcc * R / (R + FSR) where R = 10K and Vcc = 5V
    // so FSR = ((Vcc - V) * R) / V        yay math!
    fsrResistance = 5000 - fsrVoltage;     // fsrVoltage is in millivolts so 5V = 5000mV
    fsrResistance *= 47;                // 10K resistor
    fsrResistance /= fsrVoltage;
    Serial.print("FSR resistance in ohms = ");
    Serial.println(fsrResistance);
 
    fsrConductance = 1000000;           // we measure in micromhos so 
    fsrConductance /= fsrResistance;
    Serial.print("Conductance in microMhos: ");
    Serial.println(fsrConductance);
 
    // Use the two FSR guide graphs to approximate the force
    if (fsrConductance <= 1000) {
      fsrForce = fsrConductance / 80;
      Serial.print("Force in Newtons: ");
      Serial.println(fsrForce);      
       
    } else {
      fsrForce = fsrConductance - 1000;
      fsrForce /= 30;
      Serial.print("Force in Newtons: ");
      Serial.println(fsrForce);            
    }
    fsrWeight = fsrForce / 9.8;
    Serial.print("Wieght in grams: ");
    Serial.println(fsrWeight);  
  }
  Serial.println("--------------------");
  delay(1000);
}*/
