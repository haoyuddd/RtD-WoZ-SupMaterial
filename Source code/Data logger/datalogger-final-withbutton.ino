// declaring participant number and create files
// no-String, but file.print
// https://forum.arduino.cc/t/storing-data-to-sd-card-in-under-8ms-problems-understanding-sdfat-library/847823/18

#include <SPI.h>
#include <SD.h>
#include <Wire.h> //helps the Arduino with i2C
#include "RTClib.h"//catting with the real time clock
#include <Adafruit_GPS.h>
#include <ADXL335.h>

RTC_DS1307 RTC;
DateTime now;

// declaring participant number and create files
File dataFile;

//GPS
// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  false

unsigned long timer = millis();
unsigned long startTimer;
unsigned long msTimer = 0;
unsigned long last_data_save = 0; // variable for the last time data was read

//How often should be flushed?
int flush_timer = 10000; //milliseconds
int gps_timer = 1000; //milliseconds

int counter = 0; //counts the columns with values (samples)

//managing sample timing
int last_data_read = 0; // variable for the last time data was read
const int SAMPLE_RATE = 100; //100 Hz
const int SAMPLE_TIME = 1000 / SAMPLE_RATE; // 1000 = 1 sec divided by the Sample Rate = Time between samples
const int GPS_TIME = 1000;

const int zpin = A1;
const int ypin = A2;            // x-axis changing to A2 pin due to ADXL placement inside car
const int xpin = A3;            // y-axis changing to A3 pin due to ADXL placement inside car

float xRawMin = 403;
float xRawMax = 624;
float yRawMin = 386;
float yRawMax = 604;
float zRawMin = 628;
float zRawMax = 419;

float xg, yg, zg;

const int BUTTON_PIN = 2;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); //high-nocue(1), low-withcue(0)

  //Initialize I2C communication
  Wire.begin();
  RTC.begin();
  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  now = RTC.now();   //write down the date (year / month / day)

  Serial.println("Data logging test GPS&Accelerometer");
  Serial.print(" On ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.println(" ");

  //setup SD card
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(10, 11, 12, 13)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }

  Serial.println(F("card initialized."));

  // set file name
  checkParticipantNum();

  // GPS setting
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);   // turn on RMC only
  //  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);   // turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);

  startTimer = millis();
}

void checkParticipantNum() {
  bool isParticipantExist = false;
  String participantNum;
  char filename[] = "LOGGER00.csv";

  do {
    if (isParticipantExist) {
      Serial.println(F("Participant exist"));
    }

    Serial.println(F("Type the participant number:"));

    // wait for the participant number
    while (Serial.available () == 0) { }

    if (Serial.available() > 0) {
      String participantNum = Serial.readString();

      filename[6] = participantNum.charAt(0);
      filename[7] = participantNum.charAt(1);

      Serial.println (filename);

      if (SD.exists(filename)) {
        isParticipantExist = true;
      } else {
        isParticipantExist = false;
      }
    }
  } while (isParticipantExist);

  if  (!SD.exists(filename)) {
    // only open a new file if it doesn't exist
    dataFile = SD.open(filename, FILE_WRITE);
    if (! dataFile) {
      Serial.println("error opening datalog.csv");
      // Wait forever since we cant write data
      while (1) ;
    }

    if (dataFile) {
      Serial.print (F("Datalog for participant:"));
      Serial.println (participantNum);
      dataFile.print(" On ");
      dataFile.print(now.year(), DEC);
      dataFile.print('/');
      dataFile.print(now.month(), DEC);
      dataFile.print('/');
      dataFile.print(now.day(), DEC);
      dataFile.println(" ");
      dataFile.println("-,h,m,s,ms,x,y,z,cue,Lat,Lon,Speed,Angle,Altitude");
      dataFile.flush();
      //        saveData();
    }
  }
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  if (GPSECHO)
    if (c)
      Serial.print(c);
  if (GPS.newNMEAreceived()) {
    //    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }

  unsigned long millisNow = millis();
  // 100Hz data reading
  if (millisNow -  last_data_read >= SAMPLE_TIME) {
    last_data_read = millisNow;
    counter = counter + 1;

    /* ________________________________________________________________________________ */
    stopWatch();    //starts stopwatch

    /*______TIMER_____*/
    //    Serial.print("\nTime: ");
    //    if (GPS.hour < 10) {
    //      Serial.print('0');
    //    }
    //    Serial.print(GPS.hour, DEC); Serial.print(':');
    //    if (GPS.minute < 10) {
    //      Serial.print('0');
    //    }
    //    Serial.print(GPS.minute, DEC); Serial.print(':');
    //
    //    if (GPS.seconds < 10) {
    //      Serial.print('0');
    //    }
    //    Serial.print(GPS.seconds, DEC); Serial.print('=');
    msTimer = millisNow - startTimer;
    //    Serial.println(msTimer);

    dataFile.print("data,");
    dataFile.print(GPS.hour, DEC);
    dataFile.print(",");
    dataFile.print(GPS.minute, DEC);
    dataFile.print(",");
    dataFile.print(GPS.seconds, DEC);
    dataFile.print(",");
    dataFile.print(msTimer);
    dataFile.print(",");

    /*______ACCELEROMETER_____*/
    int xreading = analogRead (xpin);
    int x = map(xreading, xRawMin, xRawMax, 100, -100);
    float xg = (float) x / (-100.00);
    //    Serial.print (xg);
    //    Serial.print (",");

    int yreading = analogRead (ypin);
    int y = map(yreading, yRawMin, yRawMax, 100, -100);
    float yg = (float) y / (-100.00);
    //    Serial.print (yg);
    //    Serial.print (",");

    int zreading = analogRead (zpin);
    int z = map(zreading, zRawMin, zRawMax, 100, -100);
    float zg = (float) z / (-100.00);
    //    Serial.println (zg);
    //    Serial.print ("\n");

    dataFile.print(xg);
    dataFile.print(",");
    dataFile.print(yg);
    dataFile.print(",");
    dataFile.print(zg);
    dataFile.print(",");
    dataFile.print(buttonState);
    dataFile.print(",");

    /*______GPS READING_____*/
    // approximately every 1 seconds or so, print out the current stats
    if (millisNow - timer > GPS_TIME) {

      timer = millisNow;   // reset the timer
      Serial.print("Fix: "); Serial.print((int)GPS.fix);
      Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

      dataFile.print(GPS.latitude, 4);
      dataFile.print(",");
      dataFile.print(GPS.longitude, 4);
      dataFile.print(",");
      dataFile.print(GPS.speed);
      dataFile.print(",");
      dataFile.print(GPS.angle);
      dataFile.print(",");
      dataFile.print(GPS.altitude);
      dataFile.print("\n");

      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4);
      Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4);
      Serial.print(GPS.lon);
      Serial.print(", ");
      Serial.print("Speed (knots): ");
      Serial.print(GPS.speed);
      Serial.print(", ");
      Serial.print("Angle: ");
      Serial.print(GPS.angle);
      Serial.print(", ");
      Serial.print("Altitude: ");
      Serial.println(GPS.altitude);
      //      Serial.println("");
    } else {
      dataFile.print(GPS.latitude, 4);
      dataFile.print(",");
      dataFile.print(GPS.longitude, 4);
      dataFile.print(",");
      dataFile.print(GPS.speed);
      dataFile.print(",");
      dataFile.print(GPS.angle);
      dataFile.print(",");
      dataFile.print(GPS.altitude);
      dataFile.print("\n");

    }
    // stopWatch();

    if (millisNow -  last_data_save >= flush_timer) {
      last_data_save = millisNow;
      saveData();
    }
  }
}

void saveData() {
  dataFile.flush(); //writing data to SD
  Serial.println("Writing to SD");
}

// ------------------ Stoppuhr ----------------------------------
void stopWatch() {
  boolean stopWatch_started = false; // function to measure time
  unsigned long stopWatch_time;

  if (stopWatch_started == false) {
    stopWatch_time = millis();
    stopWatch_started = true;
  } else {
    int duration = millis() - stopWatch_time;
    Serial.print("Duartion: ");
    Serial.print(duration);
    Serial.println(" ms");

    stopWatch_started = false;
  }
}
