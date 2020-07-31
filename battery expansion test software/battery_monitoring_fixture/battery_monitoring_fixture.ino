#include <SD.h>
#include <SPI.h>
enum State { CHARGING, DISCHARGING };
State state;
State prevState;

long currentTime;
long lastTime;

const int chargeStatus = 0;
const int chargeEnableLV = 1;
const int drainEnableLV = 2;
const int chargeIndicator = 3;
const int drainIndicator = 4;

const int batteryADC = 23;

const int chipSelect = BUILTIN_SDCARD;
const int builtInLED = 13;

int batteryCounts = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(chargeStatus, INPUT);
  pinMode(chargeEnableLV, OUTPUT);
  pinMode(drainEnableLV, OUTPUT);
  pinMode(chargeIndicator, OUTPUT);
  pinMode(drainIndicator, OUTPUT);

  pinMode(batteryADC, INPUT);

  pinMode(builtInLED, OUTPUT);

  // set max battery counts to 4095
  // 3.3V = 4095 counts
  // 1 count = .81mV
  analogReadResolution(12);

  if (!SD.begin(chipSelect)) {
    digitalWrite(builtInLED, HIGH);
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  digitalWrite(builtInLED, LOW);
  Serial.println("card initialized.");

  digitalWrite(drainEnableLV, HIGH);
  delay(100);
  digitalWrite(chargeEnableLV, LOW);

  digitalWrite(drainIndicator, LOW);
  digitalWrite(chargeIndicator, HIGH);
  delay(100);
}

void loop()
{

  //batteryCounts = analogRead(batteryADC);

  String dataString = "";
  dataString += String(millis());
  dataString += ",";
  dataString += String(analogRead(batteryADC));
  dataString += ",";
  dataString += String(state);
  // open the file.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  * / Take a reading every 5 minutes * /
  delay(5 * 60 * 1000);

  prevState = state;

  Serial.println(digitalRead(chargeStatus));

  // TODO: Change this (for voltage divider)
  if ((analogRead(batteryADC) < 3600) && (state == DISCHARGING)) {
    state = CHARGING;
    Serial.println("charging");
  } else if ((digitalRead(chargeStatus)) && (state == CHARGING)) {
    Serial.println("discharging");
    state = DISCHARGING;
  }



  if (prevState != state) {
    if (state == CHARGING) {
      digitalWrite(drainEnableLV, HIGH);
      delay(100);
      digitalWrite(chargeEnableLV, LOW);

      digitalWrite(drainIndicator, LOW);
      digitalWrite(chargeIndicator, HIGH);
      delay(100);
      //TODO: read & log temperature
    } else if (state == DISCHARGING) {
      digitalWrite(chargeEnableLV, HIGH);
      delay(100);
      digitalWrite(drainEnableLV, LOW);

      digitalWrite(drainIndicator, HIGH);
      digitalWrite(chargeIndicator, LOW);
      delay(100);
      //TODO: read & log temperature
    }
  }
}
