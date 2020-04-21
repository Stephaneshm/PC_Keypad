// ---------------------------------------------------
//  ' ' est le symbole qui désigne un espace, il a la valeur 44
//  Alt Gr azerty                   €                                                                    ~  #  {  [  |  `  \  ^  @   ' '  ]  }  ¤      
//   Shift azerty       Q  B  C  D  E  F  G  H  I  J  K  L  ?  N  O  P  A  R  S  T  U  V  Z  X  Y  W  1  2  3  4  5  6  7  8  9  0   ' '  °  +  ¨  £  µ  No fr  M  %  NONE  .  /  §    >
//         azerty       q  b  c  d  e  f  g  h  i  j  k  l  ,  n  o  p  a  r  s  t  u  v  z  x  y  w  &  é  "  '  (  -  è  _  ç  à   ' '  )  =  ^  $  *  No fr  m  ù   ²    ;  :  !    <
//         qwerty       a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  1  2  3  4  5  6  7  8  9  0   ' '  -  =  [  ]  \  No US  ;  '   `    ,  .  /   No US      
//       scancode       4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39, 44, 45,46,47,48,49,  50,  51,52, 53,  54,55,56,  100};
// other see : https://www.arduino.cc/en/Reference/KeyboardModifiers




#include <Wire.h>
#include <Keyboard.h>

#define Version "0.1a"

// Comment out next line to prevent slow response when
// not actively debugging via serial port monitor
#define DEBUG

#define PCF8574_ADDR (0x38)
#define INTERRUPT_PIN (9)
#define DEBOUNCE_DELAY (200) 

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
    if (keyPressed == false) {
      keyPressed = true;
      //      Serial.println("Key Pressed !");
    }
    lastPress = millis();
  }
}


void setup() {

//#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("-------------- START --------------------");
  Serial.print("Version "); Serial.print(Version); Serial.println(" - Date :" __DATE__ " by SHM  ");
  Serial.println(__FILE__ ); //" " __DATE__ " " __TIME__);
//#endif

  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDR);
  Wire.write(0b00100110); // put HIGH Col
  Wire.endTransmission();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), keypress, CHANGE );
  Keyboard.begin();
}

void Read() {
  Wire.requestFrom(PCF8574_ADDR, 1);
  delay(100);
  if (Wire.available()) {
    printBinary(Wire.read(), 8);
  }
}

void loop() {

  if (keyPressed) {
    byte abc = 0;
    Wire.requestFrom(PCF8574_ADDR, 1);
    if (Wire.available()) {
      abc = Wire.read();
      // Serial.print (" -> read COL  - ");
      // printBinary(abc, 8);
      if ((abc & 0b00100000) == 0 ) {
        COL = 3;
      }
      else if ((abc & 0b00000100) == 0 ) {
        COL = 2;
      }
      else if ((abc & 0b00000010) == 0 ) {
        COL = 1;
      }
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
      if ((abc & 0b00001000) == 0 ) {
        ROW = 1;
      }
      else if ((abc & 0b01000000) == 0 ) {
        ROW = 2;
      }
      else if ((abc & 0b00000001) == 0 ) {
        ROW = 3;
      }
      else if ((abc & 0b00010000) == 0 ) {
        ROW = 4;
      }

    }
    delay(200);
    Wire.beginTransmission(PCF8574_ADDR);
    Wire.write(0b00100110);
    Wire.endTransmission();
#ifdef DEBUG
    Serial.println(hexaKeys[ROW - 1][COL - 1]);
#endif
    if (hexaKeys[ROW - 1][COL - 1] == "#") {
     Keyboard.press(KEY_LEFT_ALT);
     Keyboard.press(KEY_LEFT_CTRL);
     Keyboard.press(23);
     delay(100);
     Keyboard.releaseAll();
    }
    delay(200);
    keyPressed = false;
  }


}
