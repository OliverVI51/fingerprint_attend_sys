#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

SoftwareSerial fingerPrint(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

#define register_back 11
#define delete_ok 10
#define forward 9
#define reverse 8
#define match 12
#define indFinger 7
#define buzzer 6
#define records 10

int users[records];
DateTime now;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  
  pinMode(register_back, INPUT_PULLUP);
  pinMode(forward, INPUT_PULLUP);
  pinMode(reverse, INPUT_PULLUP);
  pinMode(delete_ok, INPUT_PULLUP);
  pinMode(match, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(indFinger, OUTPUT);
  digitalWrite(buzzer, LOW);

  if (digitalRead(register_back) == 0) downloadData();
  if (digitalRead(delete_ok) == 0) resetSystem();

  lcdPrint(" Fingerprint ", "Attendance sys", 2000);
  beep(500);
  
  for (int i = 1000; i < 1000 + records; i++) 
    if (EEPROM.read(i) == 0xff) EEPROM.write(i, 0);

  finger.begin(57600);
  lcdPrint("Finding Module..", "", 2000);
  
  if (!finger.verifyPassword()) {
    lcdPrint("Module Not Found", "Check Connections", 0);
    while(1);
  }

  if (!rtc.begin()) lcd.println("Couldn't find RTC");
  if (rtc.lostPower()) rtc.adjust(DateTime(2018, 6, 7, 11, 0, 0));
  
  lcdPrint(" Press Match to ", " Start System", 3000);
  for (int i = 0; i < records; i++) users[i] = EEPROM.read(1000 + i);
  digitalWrite(indFinger, HIGH);
}

void loop() {
  now = rtc.now();
  displayDateTime();
  
  int result = getFingerprintIDez();
  if (result > 0) {
    digitalWrite(indFinger, LOW);
    beep(100);
    lcdPrint("ID:" + String(result), "Please Wait....", 1000);
    attendance(result);
    lcdPrint("Attendance ", "Registered", 1000);
    digitalWrite(indFinger, HIGH);
    return;
  }
  checkKeys();
  delay(300);
}

void lcdPrint(String line1, String line2, int delayTime) {
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  if (delayTime > 0) delay(delayTime);
}

void beep(int duration) {
  digitalWrite(buzzer, HIGH);
  delay(duration);
  digitalWrite(buzzer, LOW);
}

void displayDateTime() {
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  printDigits(now.hour());
  lcd.print(':');
  printDigits(now.minute());
  lcd.print(':');
  printDigits(now.second());
  
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  printDigits(now.day());
  lcd.print('/');
  printDigits(now.month());
  lcd.print('/');
  lcd.print(now.year());
}

void printDigits(int digits) {
  if (digits < 10) lcd.print('0');
  lcd.print(digits);
}

void downloadData() {
  beep(500);
  lcdPrint("Please wait !", "Downloding Data", 0);
  Serial.println("Please wait\nDownloding Data..\n");
  
  Serial.print("S.No. ");
  for (int i = 0; i < records; i++) {
    beep(500);
    Serial.print(" User ID");
    Serial.print(i + 1);
    Serial.print(" ");
  }
  Serial.println();
  
  for (int i = 0; i < 30; i++) {
    if (i + 1 < 10) Serial.print('0');
    Serial.print(i + 1);
    Serial.print(" ");
    
    for (int j = 0; j < records; j++) 
      download((i * 7) + (j * 210));
    Serial.println();
  }
}

void resetSystem() {
  lcdPrint("Please Wait", "Reseting.....", 0);
  for (int i = 1000; i < 1005; i++) EEPROM.write(i, 0);
  for (int i = 0; i < 841; i++) EEPROM.write(i, 0xff);
  lcdPrint("System Reset", "", 1000);
}

void attendance(int id) {
  if (id < 1 || id > records) return;
  
  int eepLoc = (id - 1) * 210;
  int user = users[id - 1]++;
  int eepIndex = (user * 7) + eepLoc;
  
  EEPROM.write(eepIndex++, now.hour());
  EEPROM.write(eepIndex++, now.minute());
  EEPROM.write(eepIndex++, now.second());
  EEPROM.write(eepIndex++, now.day());
  EEPROM.write(eepIndex++, now.month());
  EEPROM.write(eepIndex++, now.year() >> 8);
  EEPROM.write(eepIndex++, now.year());
  
  EEPROM.write(1000 + (id - 1), users[id - 1]);
}

void checkKeys() {
  if (digitalRead(register_back) == 0) {
    lcdPrint("Please Wait", "", 1000);
    while (digitalRead(register_back) == 0);
    Enroll();
  }
  else if (digitalRead(delete_ok) == 0) {
    lcdPrint("Please Wait", "", 1000);
    delet();
  }
}

void Enroll() {
  int count = 1;
  lcd.clear();
  lcd.print("Enter Finger ID:");
  
  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    
    if (digitalRead(forward) == 0) {
      count = (count % records) + 1;
      delay(500);
    }
    else if (digitalRead(reverse) == 0) {
      count = (count == 1) ? records : count - 1;
      delay(500);
    }
    else if (digitalRead(delete_ok) == 0) {
      getFingerprintEnroll(count);
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i) == 0xff) {
          EEPROM.write(i, count);
          break;
        }
      }
      return;
    }
    else if (digitalRead(register_back) == 0) return;
  }
}

void delet() {
  int count = 1;
  lcd.clear();
  lcd.print("Enter Finger ID");
  
  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    
    if (digitalRead(forward) == 0) {
      count = (count % records) + 1;
      delay(500);
    }
    else if (digitalRead(reverse) == 0) {
      count = (count == 1) ? records : count - 1;
      delay(500);
    }
    else if (digitalRead(delete_ok) == 0) {
      deleteFingerprint(count);
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i) == count) {
          EEPROM.write(i, 0xff);
          break;
        }
      }
      return;
    }
    else if (digitalRead(register_back) == 0) return;
  }
}

uint8_t getFingerprintEnroll(uint8_t id) {
  int p = -1;
  lcdPrint("finger ID:" + String(id), "Place Finger", 2000);
  
  while ((p = finger.getImage()) != FINGERPRINT_OK) {
    switch (p) {
      case FINGERPRINT_NOFINGER: lcdPrint("No Finger Found", "", 0); break;
      case FINGERPRINT_PACKETRECIEVEERR: lcdPrint("Comm Error", "", 0); break;
      case FINGERPRINT_IMAGEFAIL: lcdPrint("Imaging Error", "", 0); break;
      default: lcdPrint("Unknown Error", "", 0); break;
    }
  }
  
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK: lcdPrint("Image converted", "", 0); break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }

  lcdPrint("Remove Finger", "", 2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);
  
  lcdPrint("Place Finger", " Again", 0);
  while ((p = finger.getImage()) != FINGERPRINT_OK);
  
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;
  
  p = finger.createModel();
  if (p != FINGERPRINT_OK) return p;
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) lcdPrint(" Finger Stored!", "", 2000);
  return p;
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcdPrint("Finger Not Found", "Try Later", 2000);
    return -1;
  }
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  lcdPrint("Please wait", "", 0);
  uint8_t p = finger.deleteModel(id);
  
  if (p == FINGERPRINT_OK) {
    lcdPrint("Finger Deleted", "Successfully", 1000);
  } else {
    lcdPrint("Something Wrong", "Try Again Later", 2000);
  }
  return p;
}

void download(int eepIndex) {
  if (EEPROM.read(eepIndex) == 0xff) {
    Serial.print("--------------------------- ");
    return;
  }
  
  Serial.print("T->");
  printDigit(EEPROM.read(eepIndex++));
  Serial.print(':');
  printDigit(EEPROM.read(eepIndex++));
  Serial.print(':');
  printDigit(EEPROM.read(eepIndex++));
  Serial.print(" D->");
  printDigit(EEPROM.read(eepIndex++));
  Serial.print('/');
  printDigit(EEPROM.read(eepIndex++));
  Serial.print('/');
  Serial.print(EEPROM.read(eepIndex++) << 8 | EEPROM.read(eepIndex++));
  Serial.print(" ");
}

void printDigit(byte digit) {
  if (digit < 10) Serial.print('0');
  Serial.print(digit);
}