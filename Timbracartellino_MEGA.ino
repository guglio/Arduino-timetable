#include <Adafruit_NFCShield_I2C.h>
#include <Adafruit_FONA.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// pin da usare con il mega per il Serial1
//#define FONA_RX 18
//#define FONA_TX 19
#define FONA_RST 50
#define IRQ 25 //da mettere a seconda dei collegamenti
#define RESET 24 //idem IRQ
//collegare SCL e SDA del PN532 al pin 20 (SDA) e 21 (SCL)
//Blocchi:
//cognome -> 4
//nome -> 5
//azienda -> 6
const int blocchiRead[] = {4, 5, 6};
LiquidCrystal lcd(22, 23, 41, 40, 43, 42);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
Adafruit_NFCShield_I2C nfc(IRQ, RESET);
RTC_DS1307 rtc;
bool lastSyncDate = false;
DateTime lastSync;
int timezone = 25;

void setup() {
  //Serial.begin(115200);
  Serial1.begin(4800);
  lcd.begin(16, 2);
  Wire.begin();
  rtc.begin();
  pinMode(SS, OUTPUT);
  rtc.begin();
  lcd.setCursor(0, 0);
  lcd.print(F("Fona..."));
  if (! fona.begin(Serial1))
    lcd.print(F("ERROR"));
  else
    lcd.print(F("OK"));
  lcd.setCursor(0, 1);
  lcd.print(F("TIM..."));
  while (fona.getNetworkStatus() != 1);
  lcd.print(F("OK"));
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("GPRS..."));
  fona.setGPRSNetworkSettings(F("ibox.tim.it"), F(""), F(""));
  while (!fona.enableGPRS(true));
  lcd.print(F("OK"));
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print(F("Clock..."));
  if (! rtc.isrunning()) {
    lcd.print(F("ERROR"));
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  else
    lcd.print(F("OK"));
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("SD card..."));
  if (!SD.begin(10, 11, 12, 13))
    lcd.print(F("ERROR"));
  else
    lcd.print(F("OK"));
  delay(500);
  nfc.begin();
  lcd.setCursor(0, 1);
  lcd.print(F("NFC..."));
  if (! nfc.getFirmwareVersion()) {
    lcd.print(F("ERROR"));
    while (1);
  }
  lcd.print(F("OK"));
  delay(500);
  lcd.clear();
  nfc.setPassiveActivationRetries(0x00);
  nfc.SAMConfig();
  if (fona.enableNTPTimeSync(true, F("pool.ntp.org")))
    syncTime(rtc.now());
    
}

void loop() {
  File logFile;
  String nomeFile = String((rtc.now().year()) % 100);
  uint8_t uid[]  = {0, 0, 0, 0, 0, 0, 0};
  String jsonUID  = "";
  char fileNameCharArray[] = "";
  uint8_t uidLength ;
  String timbraturaTime = "";
  uint16_t statuscode ;
  int16_t length ;
  uint32_t szPos;
  uint32_t numBytes = 16;
  String uidCardTMP = "";
  char url[] = "http://www.guglielmo-turco.com/badge/feed.php";
  String cognome = "";
  String nome = "";
  String azienda = "";
  uint8_t currentblock;
  bool authenticated = false;
  uint8_t keyuniversal[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  boolean success;
  uint8_t data[16];
  boolean readCard = false; 

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength)) {
    timbraturaTime += rtc.now().hour();
    timbraturaTime += ":";
    timbraturaTime += rtc.now().minute();
    timbraturaTime += ":";
    timbraturaTime += rtc.now().second();
    for (int i = 0; i < sizeof(blocchiRead) / sizeof(blocchiRead[0]); i++) {
      currentblock = blocchiRead[i];
      if (nfc.mifareclassic_IsFirstBlock(currentblock)) authenticated = false;

      if (!authenticated) {
        if (currentblock == 0) success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
        else success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
        if (success) authenticated = true;
      }
      if (!authenticated);
      else {
        success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
        if (success) {
          for (szPos = 0; szPos < numBytes; szPos++) {
            if (data[szPos] <= 0x1F);
            else {
              switch (currentblock) {
                case 4:
                  cognome += (char)data[szPos];
                  break;
                case 5:
                  nome += (char)data[szPos];
                  break;
                case 6:
                  azienda += (char)data[szPos];
                  break;
                default: break;
              }
            }
          }
        }
      }
    }
    readCard = true;
  }
  if (readCard) {
    lcd.setCursor(0, 1);
    lcd.print(F("                "));
    lcd.setCursor(0, 1);
    lcd.print(F("Timbratura"));
    lcd.print(F("."));
    for (uint8_t i = 0; i < uidLength; i++) {
      jsonUID += String(uid[i], HEX);
    }
    jsonUID.toUpperCase();
    lcd.print(F("."));
    if (rtc.now().month() < 10)
      nomeFile += "0";
    nomeFile += rtc.now().month();
    nomeFile += "01.csv";
    nomeFile.toCharArray(fileNameCharArray, nomeFile.length() + 1);
    logFile = SD.open(fileNameCharArray, FILE_WRITE);
    if (!logFile) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("error opening:"));
      lcd.setCursor(0, 1);
      lcd.print(fileNameCharArray);
    }
    else{
      logFile.print(rtc.now().day());
      logFile.print("/");
      logFile.print(rtc.now().month());
      logFile.print("/");
      logFile.print(rtc.now().year());
      logFile.print(";");
      logFile.print(timbraturaTime);
      logFile.print(";");
      logFile.print(jsonUID);
      logFile.print(";");
      logFile.print(nome);
      logFile.print(";");
      logFile.print(cognome);
      logFile.print(";");
      logFile.print(azienda);
      logFile.print("\n");
      logFile.close();
      uidCardTMP += "{\"UID\":\"";
      uidCardTMP += jsonUID;
      uidCardTMP += "\",\"location\":\"Cuneo\"}";
      char jsonData[uidCardTMP.length() + 1];
      uidCardTMP.toCharArray(jsonData, uidCardTMP.length() + 1);
      lcd.print(F("."));
      fona.enableGPRS(true);
      if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) jsonData , strlen(jsonData), &statuscode, (uint16_t *)&length)) {
        lcd.setCursor(0, 1);
        lcd.print(F("Upload error.   "));
      }
      fona.HTTP_POST_end();
      lcd.print(F("OK"));
    }
  }
  if(rtc.now().hour() == 3 && rtc.now().minute() == 0)
    checkCheckSyncTime(rtc.now());
  stampaData(rtc.now());
  delay(1000);
}
void stampaData(DateTime ora) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (ora.day() < 10)
    lcd.print(F("0"));
  lcd.print(ora.day(), DEC);
  lcd.print(F("/"));
  if (ora.month() < 10)
    lcd.print(F("0"));
  lcd.print(ora.month(), DEC);
  lcd.print(F("/"));
  lcd.print(ora.year(), DEC);
  lcd.print(F(" "));
  if (ora.hour() < 10)
    lcd.print(F(" "));
  lcd.print(ora.hour(), DEC);
  lcd.print(F(":"));
  if (ora.minute() < 10)
    lcd.print(F("0"));
  lcd.print(ora.minute(), DEC);
  lcd.setCursor(0, 1);
  lcd.print(F("S.I.C.A.M. SRL  "));
}

void checkCheckSyncTime(DateTime ora){

  if(timezone == 25)
    timezone = adjustDstEurope(ora);
  
  if(!lastSyncDate)
    lastSync = syncTime(ora);
  else{
    if( ( ora.day() - lastSync.day() ) > 7  || timezone != adjustDstEurope(ora))
      lastSync = syncTime(ora);
  } 
}

DateTime syncTime(DateTime ora){
  timezone = adjustDstEurope(ora);
  char buffer[23];
  char bufferTimeTMZ[21] = "20";
  
//  if (!fona.enableNTPTimeSync(true, F("pool.ntp.org")))
//  
//    Serial.println(F("Failed to enable"));
  fona.getTime(buffer, 23);
  for(int i=1,n=strlen(buffer)-4,j=2;i<n;i++ && j++)
      bufferTimeTMZ[j] = buffer[i];
  
  for(int i=0,n=strlen(bufferTimeTMZ);i<n;i++)
  {
    if(bufferTimeTMZ[i] == '/' || bufferTimeTMZ[i] == ':' || bufferTimeTMZ[i] == ',')
      bufferTimeTMZ[i] = ' ';    
  }
  
  uint16_t yearDate;
  uint16_t monthDate;
  uint16_t dayDate;
  uint16_t hourDate;
  uint16_t minuteDate;
  uint16_t secondDate;
  sscanf(bufferTimeTMZ,"%u %u %u %u %u %u",&yearDate,&monthDate,&dayDate,&hourDate,&minuteDate,&secondDate);
  hourDate += timezone;
  rtc.adjust(DateTime(yearDate,monthDate,dayDate,hourDate,minuteDate,secondDate));
}




int adjustDstEurope(DateTime ora)
{
 // last sunday of march
 int beginDSTDate =  (31 - (5* ora.year() /4 + 4) % 7);
 Serial.println(beginDSTDate);
 int beginDSTMonth = 3;
 //last sunday of october
 int endDSTDate = (31 - (5 * ora.year() /4 + 1) % 7);
 Serial.println(endDSTDate);
 int endDSTMonth = 10;
 // DST is valid as:
 if (((ora.month() > beginDSTMonth) && (ora.month() < endDSTMonth))
     || ((ora.month() == beginDSTMonth) && (ora.day() >= beginDSTDate)) 
     || ((ora.month() == endDSTMonth) && (ora.day() <= endDSTDate)))
 return 2;  // DST europe = utc +2 hour
 else return 1; // nonDST europe = utc +1 hour
}
