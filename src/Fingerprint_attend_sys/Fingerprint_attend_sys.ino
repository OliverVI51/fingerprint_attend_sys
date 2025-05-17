#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

SoftwareSerial fingerSerial(2, 3);
Adafruit_Fingerprint finger(&fingerSerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

const uint8_t PIN_REGISTER_BACK = 11;
const uint8_t PIN_DELETE_OK = 10;
const uint8_t PIN_FORWARD = 9;
const uint8_t PIN_REVERSE = 8;
const uint8_t PIN_INDICATOR = 7;
const uint8_t PIN_BUZZER = 6;

const uint8_t MAX_RECORDS = 10;
const int EEPROM_USER_START = 1000;

uint8_t users[MAX_RECORDS];
DateTime now;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  pinMode(PIN_REGISTER_BACK, INPUT_PULLUP);
  pinMode(PIN_FORWARD, INPUT_PULLUP);
  pinMode(PIN_REVERSE, INPUT_PULLUP);
  pinMode(PIN_DELETE_OK, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_INDICATOR, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  if (digitalRead(PIN_REGISTER_BACK) == LOW) downloadData();
  if (digitalRead(PIN_DELETE_OK) == LOW) resetSystem();

  lcdPrint(F("Fingerprint"), F("Attendance sys"), 2000);
  beep(500);

  for (int i = 0; i < MAX_RECORDS; i++) {
    int addr = EEPROM_USER_START + i;
    if (EEPROM.read(addr) == 0xFF) EEPROM.write(addr, 0);
  }

  finger.begin(57600);
  lcdPrint(F("Finding Module.."), F(""), 2000);

  if (!finger.verifyPassword()) {
    lcdPrint(F("Module Not Found"), F("Check Connections"), 0);
    while (true);
  }

  if (!rtc.begin()) {
    lcdPrint(F("RTC Missing"), F(""), 2000);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2018, 6, 7, 11, 0, 0));
  }

  lcdPrint(F("Welcome"), F("Place Finger"), 3000);

  for (int i = 0; i < MAX_RECORDS; i++) {
    users[i] = EEPROM.read(EEPROM_USER_START + i);
  }
  digitalWrite(PIN_INDICATOR, HIGH);
}

void loop() {
  now = rtc.now();
  displayDateTime();

  int id = getFingerprintIDez();
  if (id > 0) {
    digitalWrite(PIN_INDICATOR, LOW);
    beep(100);
    char idStr[10];
    snprintf(idStr, sizeof(idStr), "%d", id);
    lcdPrint(F("ID:"), idStr, 1000);
    attendance(id);
    lcdPrint(F("Attendance"), F("Registered"), 1000);
    digitalWrite(PIN_INDICATOR, HIGH);
  }

  checkKeys();
  delay(300);
}

void lcdPrint(const __FlashStringHelper* line1, const __FlashStringHelper* line2, int delayMs) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  if (delayMs > 0) delay(delayMs);
}

void lcdPrint(const __FlashStringHelper* line1, const char* line2, int delayMs) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  if (delayMs > 0) delay(delayMs);
}

void beep(int duration) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(duration);
  digitalWrite(PIN_BUZZER, LOW);
}

void displayDateTime() {
  char dateStr[11];
  char timeStr[9];
  snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", now.day(), now.month(), now.year());
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 0);
  lcd.print(dateStr);
  lcd.setCursor(0, 1);
  lcd.print(timeStr);
}

void resetSystem() {
  for (int i = 0; i < MAX_RECORDS; i++) {
    EEPROM.write(EEPROM_USER_START + i, 0);
  }
  lcdPrint(F("System Reset"), F("Done"), 2000);
}

void checkKeys() {
}

void downloadData() {
}

int getFingerprintIDez() {
  return -1;
}

void attendance(int id) {
}