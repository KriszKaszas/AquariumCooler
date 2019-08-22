#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//----------------------LCD I2C Controller Setup----------------------

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//------------------Temperature Sensor Global Scope-------------------

OneWire oneWire(2);
DallasTemperature sensors(&oneWire);

//-----------------------Global Scope Variables-----------------------

#define outputA 10
#define outputB 11
#define outputC 12
#define heater 7
#define cooler 4

float celsius = 23;
int setTemp = 23;
int aState;
int aLastState;
int cState = 1;
bool menu = false;

//---------------------------Initialization---------------------------

void setup()
{
  bootSequence();
  bootScreen();
}

//--------------------------------Main--------------------------------

void loop() {
  if (menu == true) {
    encoderInput();
  }
  if (menu == false) {
    manageTemp();
  }
}

//----------------------------Boot Sequence---------------------------

void bootSequence() {
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);
  pinMode (outputC, INPUT);
  pinMode(heater, OUTPUT);
  pinMode(cooler, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);
  aLastState = digitalRead(outputA);
  sensors.begin();
}

//---------------Temperature Measurement And Management---------------

void manageTemp() {
  menu = false;
  while (menu == false) {
    if (celsius > setTemp + 3) {
      digitalWrite(cooler, HIGH);
      digitalWrite(heater, LOW);
    }
    if (celsius < setTemp -3) {
      digitalWrite(heater, HIGH);
      digitalWrite(cooler, LOW);
    }
    if (celsius <= setTemp + 3 && celsius >= setTemp - 3) {
      digitalWrite(heater, LOW);
      digitalWrite(cooler, LOW);
    }
    isButtonPressed();
    measureAndPrintTemp();
  }
}


void measureAndPrintTemp() {
  sensors.requestTemperatures();
  celsius = sensors.getTempCByIndex(0);
  printCurrentTemp();
  printSetTemp();
}

//----------------------------LCD Printing----------------------------

void bootScreen() {
  lcd.print("    ");
  lcd.print("AquaCube");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.print("Alpha Ver: 1.0");
  lcd.setCursor(0, 0);
  delay(6000);
  lcd.clear();
}


void printCurrentTemp() {
  lcd.clear();
  lcd.print("WaterTemp:");
  lcd.print(celsius, 1);
  lcd.print((char)223);
  lcd.print("C");
}


void printSetTemp() {
  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print("SetTemp:");
  lcd.print(setTemp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 0);
}


void menuPrint() {
  lcd.clear();
  lcd.print("Set Aquarium");
  lcd.setCursor(0, 1);
  lcd.print("Temperature:");
  lcd.print(setTemp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 0);
}

//---------------------------Encoder Control--------------------------

void isButtonPressed() {
  cState = digitalRead(outputC);
  if (cState == 0) {
    menu = !menu;
    while (cState != 1) {
      cState = digitalRead(outputC);
    }
  }
}


void encoderInput() {
  menu = true;
  menuPrint();
  while (menu == true) {
    aState = digitalRead(outputA);
    if (aState != aLastState) {
      if (digitalRead(outputB) != aState && setTemp < 40) {
        setTemp ++;
      } else {
        if (setTemp > 10) {
          setTemp --;
        }
      }
      menuPrint();
    }
    aLastState = aState;
    isButtonPressed();
  }
}
