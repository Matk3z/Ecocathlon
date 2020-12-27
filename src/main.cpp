#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>
#include <iostream>
#include <eco-err.h>
#include <EEPROM.h>
#include <ArduinoJson.h>  
#include <MFRC522.h>
#include <FastLED.h>
#include <string>

#define StatusAddr 0x01
#define UserAddr 0x02


#define NUM_LEDS 8
#define LED_PIN 4
#define CS 15
#define RST 5


StaticJsonDocument<200> doc;

esp_status SYSTEMSTATUS;

String ReadEEPROMString(uint8 addr, int size);

int ledPin;
int RFIDRST;

char UID[10];

CRGB leds[NUM_LEDS];

int status = WL_IDLE_STATUS;

char ssid[] = "LivingLab";
char pass[] = "123fablab";

MFRC522 rfid(CS, RST);

String user;
bool WriteEEPROMString(uint8 addr,String string){

    EEPROM.write(addr, sizeof(user));
    for(int i = 1; i<=sizeof(user);i++){
        EEPROM.write(addr + i, user.c_str()[i]);
    }
    EEPROM.commit();

    if(ReadEEPROMString(addr, sizeof(user)) == user){
        return true;
    }
    else{
        return false;
    }
}

String ReadEEPROMString(uint8 addr, int size)
{
    char* buffer = new char[16];

    for(int i = 0; i < size; i++){
    buffer[i] = EEPROM.read(addr);
    }
    String data = buffer;
    return data;
}
esp_status SetUser(String user)
{
    if(WriteEEPROMString(UserAddr, user)){
        Serial.print("User has been set");
    }
    else{
        Serial.print("User set error");
    }
}

esp_status GetStatus()
{
    esp_status status;

    status = EEPROM.read(StatusAddr);


    return status;
}

void ReadRFID(){

}

void SetStatus(esp_status status)
{

    EEPROM.write(StatusAddr, status);
    if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
}


esp_status WIFIInit(){

    status = WiFi.begin(ssid, pass);
    Serial.println();
    Serial.print("Connecting");
    for(int i = 0; i < 100;i++)
    {
        Serial.print(".");
        if(WiFi.status() == WL_CONNECTED)
        {
            return CONN_STATUS;
            
        }
        delay(1000);
    }
    
 
    return BUG_STATUS;
}


void SystemInit(){

    Serial.begin(115200);
    EEPROM.begin(512);
    while ( (SYSTEMSTATUS = WIFIInit())  != CONN_STATUS);
    Serial.println("Successfully connected to the network");

    SPI.begin();
	rfid.PCD_Init();

}

void setup() {
  // put your setup code here, to run once:


    SYSTEMSTATUS = HARDRESET_STATUS;
    SetStatus(SYSTEMSTATUS);
    SystemInit();

    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(60);

    if(SYSTEMSTATUS == LINKED_STATUS){
        Serial.print("Esp started successfully");
    }
    

}

void loop() {
  // put your main code here, to run repeatedly:

        leds[0] = CRGB::White; 
        FastLED.show(); 
        delay(30); 

    if ( ! rfid.PICC_IsNewCardPresent())
    {
        return;
    }
    // Select one of the cards
    if ( ! rfid.PICC_ReadCardSerial()){
        return;
    }

    byte PSWBuff[] = {0xFF, 0xFF, 0xFF, 0xFF}; //32 bit PassWord default FFFFFFFF
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag

    Serial.print("Auth: ");
    Serial.println(rfid.PCD_NTAG216_AUTH(&PSWBuff[0], pACK)); //Request Authentification if return STATUS_OK we are good

    //Print PassWordACK
    Serial.print(pACK[0], HEX);
    Serial.println(pACK[1], HEX);

    byte WBuff[] = {0x00, 0x00, 0x00, 0x04};
    byte RBuff[18];

    //Serial.print("CHG BLK: ");
    //Serial.println(mfrc522.MIFARE_Ultralight_Write(0xE3, WBuff, 4));  //How to write to a page

	rfid.PICC_DumpMifareUltralightToSerial();


    HTTPClient http;
    WiFiClient client;
    
    http.begin("http://68.183.211.90:80/get/config/Sydney");
	

    // int httpCode = http.GET();
    int httpCode = 0;

    if(httpCode > 0)
    {
        String data = http.getString();

        String user = doc["ordre"];
        Serial.print(user);

    }

  http.end();
  delay(500);
}