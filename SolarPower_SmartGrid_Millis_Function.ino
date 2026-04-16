#include <LiquidCrystal.h>
//------------Pin Config---------------//
#define VoltagePin A0
#define GridPin 2
#define RelayHigh_Pin 10
#define RelayMedium_Pin 11
#define RelayLow_Pin 13

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

const float ADC_REF = 5.0;
const float ADC_RES = 1023.0;

//--------Resistor Values---------------//
const float R1 = 10000.0;
const float R2 = 2200.0;

//----------Battery Threshold------------//
const float FULL_BATTERY = 12.0;
const float LOW_BATTERY = 11.0;
const float CRITICAL_BATTERY = 10.8;

unsigned long lcdTimer = 0;
bool lcdToggle = false;
float lastVoltage = 0.0;

void setup() {
  // put your setup code here, to run once:
  pinMode(GridPin, INPUT);

  pinMode(RelayHigh_Pin, OUTPUT);
  pinMode(RelayMedium_Pin, OUTPUT);
  pinMode(RelayLow_Pin, OUTPUT);

  //digitalWrite(RelayHigh_Pin, HIGH);  // High Priority Load Always On Initially

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Solar Power");
  lcd.setCursor(0, 1);
  lcd.print("Smart Grid");
  delay(2000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  float BatterVoltage = readBatteryVoltage();
  bool gridAvailable = digitalRead(GridPin);

  // Serial.print("Battery Voltage : ");
  // Serial.print(BatterVoltage);
  // Serial.println("V");
  if (abs(BatterVoltage - lastVoltage) > 0.05) {
    Serial.print("Battery Voltage : ");
    Serial.print(BatterVoltage, 2);
    Serial.println(" V");
    lastVoltage = BatterVoltage;
  }

  lcd.setCursor(0, 0);
  lcd.print("Vbat: ");
  lcd.print(BatterVoltage, 2);
  lcd.print("V   ");

  lcd.setCursor(0, 1);
  if (BatterVoltage < CRITICAL_BATTERY) {
    digitalWrite(RelayHigh_Pin, LOW);
    digitalWrite(RelayMedium_Pin, LOW);
    digitalWrite(RelayLow_Pin, LOW);

    unsigned long currentMillis = millis();

    // Show CRITICAL -> GRID for 1 sec
    if (!lcdToggle && (currentMillis - lcdTimer >= 1000)) {
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("CRITICAL-> GRID ");
      lcdTimer = currentMillis;
      lcdToggle = true;
    }
    // Show MED & LOW OFF for 2 sec
    else if (lcdToggle && (currentMillis - lcdTimer >= 2000)) {
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Medium & Low OFF");
      lcdTimer = currentMillis;
      lcdToggle = false;
    }
    return;
    //lcd.print("CRITICAL -> GRID");
    Serial.println("CRITICAL : Switched to Grid");
    lcdToggle = false;
    lcdTimer = millis();
  } else {
    digitalWrite(RelayHigh_Pin, HIGH);  // HIGH Priority Loads Always ON
  }

  if (BatterVoltage >= FULL_BATTERY) {
    digitalWrite(RelayMedium_Pin, HIGH);
    digitalWrite(RelayLow_Pin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("ALL LOADS ARE ON");
    Serial.println("All Loads are ON");
  } else if (BatterVoltage >= LOW_BATTERY) {
    digitalWrite(RelayMedium_Pin, HIGH);
    digitalWrite(RelayLow_Pin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("LOW LOAD OFF    ");
    Serial.println("Low Priority OFF");
  } else {
    digitalWrite(RelayMedium_Pin, LOW);
    digitalWrite(RelayLow_Pin, LOW);
    lcd.setCursor(0, 1);
    //lcd.print("Medium & Low OFF");
    Serial.println("Medium & Low OFF");
  }
  delay(5000);
}
float readBatteryVoltage() {
  int adcValue = analogRead(VoltagePin);
  // float adcVoltage = (adcValue * ADC_REF) * ADC_RES;
  // float batteryVoltage = adcVoltage * ((R1 + R2) / R2);

  float adcVoltage = adcValue * (ADC_REF / ADC_RES);
  float batteryVoltage = adcVoltage * 5.545;

  return batteryVoltage;
}