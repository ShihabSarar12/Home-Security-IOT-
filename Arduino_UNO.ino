#include <Keypad.h>
#include <EEPROM.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//-----OLED-----
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
//-----------------------------------

const char passwordInput = '*';
const char passwordStore = '#';
const char passwordValidate = 'D';
const char lockSystem = 'A';
const char shiftToWifi = 'C';
String password = "";
bool unlocked = false;

//-----Servo------
Servo servo;
const int servoPin = 10;
void moveAngle(int newAngle) {
  servo.write(newAngle);
  delay(15);
}
//----------

//------Keypad-----
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//------------------------------------------------------------------

//--------EEPROM-----------
const int addressEEPROM = 0;
bool writeInEEPROM = false;

void writeStringToEEPROM(int address, String data) {
  for (unsigned int i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + data.length(), '\0');
}

String readStringFromEEPROM(int address) {
  String result = "";
  char character;
  while ((character = EEPROM.read(address)) != '\0') {
    result += character;
    address++;
  }
  return result;
}
//-------------

//----Nodemcu-32----
const int espCommandPin = 11;
bool espControl = false;
String wifiControl = "";
//---------------------------

void setup() {
  servo.attach(servoPin);
  pinMode(espCommandPin, INPUT);
  servo.write(0);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("LOCKED");
  display.display();
}

void loop() {
  display.clearDisplay();
  char key = kpd.getKey();
  if (key == shiftToWifi) {
    espControl = !espControl;
  } else {
    if (espControl) {
      if (digitalRead(espCommandPin) && !unlocked) {
        unlocked = true;
        moveAngle(90);
        Serial.println("UNLOCKED!!");
        wifiControl = "WIFI:     unlocked";
      } else if (!digitalRead(espCommandPin) && unlocked) {
        unlocked = false;
        moveAngle(0);
        Serial.println("LOCKED!!");
        wifiControl = "WIFI:     locked";
      }
      Serial.println("Using WIFI");
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(wifiControl);
      display.display();
    } else {
      if (key == passwordStore && unlocked && writeInEEPROM) {
        writeInEEPROM = false;
        Serial.println("Password: " + password);
        writeStringToEEPROM(addressEEPROM, password);
        unlocked = false;
        moveAngle(0);
        password = "";
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Input     Written");
        display.display();
      } else if (key == passwordStore && unlocked && !writeInEEPROM) {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Change Password      before");
        display.display();
      } else if (key == passwordStore && !unlocked) {
        Serial.println("Unlock the system before changing password");
        password = "";
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Unlock the system     before");
        display.display();
      } else if (key == passwordInput && unlocked) {
        writeInEEPROM = true;
        password = "";
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Changing    password");
        display.display();
      } else if (key == passwordInput && !unlocked) {
        Serial.println("Unlock the system before changing password");
        password = "";
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Unlock the system     before");
        display.display();
      } else if (key == lockSystem && unlocked) {
        unlocked = false;
        moveAngle(0);
        Serial.println("LOCKED!!");
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("LOCKED");
        display.display();
      } else if (key == lockSystem && !unlocked) {
        unlocked = false;
        moveAngle(0);
        Serial.println("Already LOCKED!!");
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Already    LOCKED");
        display.display();
      } else if (key == passwordValidate && !unlocked) {
        Serial.println("EEPROM: " + readStringFromEEPROM(addressEEPROM));
        Serial.println("Password: " + password);
        if (readStringFromEEPROM(addressEEPROM) == password) {
          unlocked = true;
          moveAngle(90);
          Serial.println("UNLOCKED!!");
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("UNLOCKED");
          display.display();
        } else {
          Serial.println("Password doesn't match!!Try Again");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("Password doesn't     match!!Try Again");
          display.display();
        }
        password = "";
      } else if (key == passwordValidate && unlocked) {
        Serial.println("Password doesn't match!!Try Again");
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Already Unlocked");
        display.display();
      } else if (key) {
        password += key;
        Serial.println(password);
      }
    }
  }
}