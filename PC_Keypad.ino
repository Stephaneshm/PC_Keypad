#include <Wire.h>

#include "HID-Project.h"

#define Version "0.1a"

  
#define PCF8574_ADDR (0x38)
#define INTERRUPT_PIN (2)
#define DEBOUNCE_DELAY (200) // milliseconds
  
bool keyPressed = false;
int32_t lastPress = 0;


int ROW = 0; 
int COL = 0; 

char hexaKeys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};


volatile bool status = true;

//-------------------------------------------------------------------------
// http://www.phanderson.com/arduino/arduino_display.html
//
// This has several useful print routines for the Arduino, worth a look
//-------------------------------------------------------------------------
void printBinary(int v, int num_places) {
  int mask = 0, n;
  for (n = 1; n <= num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask;  // truncate v to specified number of places
  while (num_places) {
    if (v & (0x0001 << (num_places - 1))) {
      Serial.print("1");
    }
    else {
      Serial.print("0");
    }
    --num_places;
    if (((num_places % 4) == 0) && (num_places != 0)) {
      Serial.print("_");
    }
  }
  Serial.println("");
}

  

void keypress() {
  if (millis() - lastPress > DEBOUNCE_DELAY) {
    if (keyPressed==false) {
      keyPressed = true;
//      Serial.println("Key Pressed !");
    }
    lastPress = millis();
  }
}
  
  
void setup() {
  Serial.begin(115200);
  Serial.println("-------------- START --------------------");
  Serial.print("Version "); Serial.print(Version); Serial.println(" - Date :" __DATE__ " by SHM  ");
  Serial.println(__FILE__ ); //" " __DATE__ " " __TIME__);
  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0b00100110); // put HIGH Col
  Wire.endTransmission();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), keypress, CHANGE );
  Consumer.begin();
}

void Read(){
        Wire.requestFrom(PCF8574_ADDR, 1);
        delay(100);
        if (Wire.available()) {  printBinary(Wire.read(), 8);    }
}
  
void loop() {
 
  if (keyPressed) {
        byte abc = 0;
        Wire.requestFrom(PCF8574_ADDR, 1);
        if (Wire.available()) {
            abc = Wire.read();
           // Serial.print (" -> read COL  - ");
           // printBinary(abc, 8);
            if ((abc & 0b00100000) == 0 ) { COL=3;}
            else if ((abc & 0b00000100) == 0 ) { COL=2;}
            else if ((abc & 0b00000010) == 0 ) { COL=1;}
        }
        delay(10);
        Wire.beginTransmission(PCF8574_ADDR);
        Wire.write(0b11011001); //put high row
        Wire.endTransmission();
        delay(50);
        Wire.requestFrom(PCF8574_ADDR, 1);
        if (Wire.available()) {
            abc = Wire.read();
           // Serial.print (" -> read ROW  - ");
           // printBinary(abc, 8);
            if ((abc & 0b00001000) == 0 ) { ROW=1;}
            else if ((abc & 0b01000000) ==0 ) {ROW=2;}
            else if ((abc & 0b00000001) ==0 ) {ROW=3;}
            else if ((abc & 0b00010000) ==0 ) {ROW=4;}
            
        }    
        delay(200);
        Wire.beginTransmission(PCF8574_ADDR);
        Wire.write(0b00100110);
        Wire.endTransmission();
        Serial.println(hexaKeys[ROW-1][COL-1]);
        if (hexaKeys[ROW-1][COL-1]=="#") { Consumer.write(CONSUMER_CALCULATOR);}
        delay(200);
        keyPressed = false;
      }

   
}
