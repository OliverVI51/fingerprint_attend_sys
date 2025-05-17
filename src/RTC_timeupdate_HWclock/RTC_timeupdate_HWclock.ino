#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.println("Send current time in format: YYYY,MM,DD,HH,MM,SS");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int year, month, day, hour, minute, second;
    char separator;

    sscanf(input.c_str(),"%d,%d,%d,%d,%d,%d",&year, &month, &day, &hour, &minute, &second);

      rtc.adjust(DateTime(year, month, day, hour, minute, second));
      Serial.println("RTC time updated.");
    }
  }