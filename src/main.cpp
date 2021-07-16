#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>
#include <iostream>
#include <eco-err.h>
#include <Tag.h>
#include <EEPROM.h>
#include <ArduinoJson.h>  
#include <MFRC522.h>
#include <string>
#include <TimeLib.h>
#include <ctimer.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER

#include <FastLED.h>


#define NUM_LEDS 8
#define LED_PIN 2
#define CS 15
#define RST 5

#define ledNumber 8
#define BRIGHTNESS 255   /* Control the brightness of your leds */
#define SATURATION 255 

StaticJsonDocument<512> doc;

HTTPClient http;
Ctimer timer;

WiFiClientSecure client;

int wifiLed = 1;
int winled = 1;
CRGB leds[8];

CRGB green = CRGB::Green;
CRGB red = CRGB::Red;
CRGB blue = CRGB::Blue;
CRGB black = CRGB::Black;
CRGB orange = CRGB::Orange;
CRGB white = CRGB::White;
int winLedLit = 0;
bool isAvailable;
int startTime = 0;

time_t espTime = now();

String postKey = "bd824a00-53da-4281-bb85-e4839eebbf59";
String getKey = "19096661-1fb2-427e-9c55-c4839bcb3f02";

bool ledTimer;
bool winAnimation = false;
bool isLit = false;
int StatusAnimationCount = 0;
CRGB statusColor;


struct GameConfig_t {
    int status;
    String teamColorString;
    int teamColor;
    int gameTime;

    int fingerprintAddr;
    int userNameAddr;
    int UIDAddr;
    int passAddr;
    int newUIDAddr;
    int newpassAddr;


    int QCM;
    int Trouver;
    int Ordre;
    int Sondage;

    int tagDetectedNumber = 0;
    Tag *tagQuestionStarted = NULL;
    Tag detectedTags[25];

} GameConfiguration;

String wifiUID;
String wifiPass;

String newWifiUID ;
String newWifiPass;

String fingerPrint;

String user;

Tag *detectedTags;

int tagArraySize;


const char* host = "ecocathlon.fr";
const int httpsPort = 443;

                        
MFRC522 rfid(CS, RST);

void SaveConfig(){
    EEPROM.put(0, GameConfiguration);
    EEPROM.commit();

}

int WriteStringInMemory(int addr, const char* string){
    int stringLenght = strlen(string);
    Serial.println("Writing " + String(stringLenght) + " bytes at the address " + String(addr));
    EEPROM.write(addr, stringLenght);
    for (int i = 1; i <= stringLenght;i++){
        EEPROM.write(addr + i,string[i - 1]);
    }
    EEPROM.commit();
    return addr + stringLenght + 1;
}

String ReadStringInMemory(int addr){
    int stringLenght = EEPROM.read(addr);
    Serial.println("reading at address : " + String(addr));
    String read;
    for (int i = 1; i <= stringLenght;i++){
        char character = char(EEPROM.read(addr+i));
        read.concat(character);
    }
    Serial.println(read);
    return read;
}

void WriteAllStringsInMemory(const char* fingerPrint, const char* userName, const char* UID, const char* Pass, const char* newUID, const char* newPass){
    int addr = 0x600;

    GameConfiguration.fingerprintAddr = addr;
    addr = WriteStringInMemory(addr, fingerPrint);
    
    GameConfiguration.userNameAddr = addr;
    addr = WriteStringInMemory(addr, userName);

    GameConfiguration.UIDAddr = addr;
    addr = WriteStringInMemory(addr, UID);

    GameConfiguration.passAddr = addr;
    addr = WriteStringInMemory(addr, Pass);

    GameConfiguration.newUIDAddr = addr;
    addr = WriteStringInMemory(addr, newUID);

    GameConfiguration.newpassAddr = addr;
    addr = WriteStringInMemory(addr, newPass);

    SaveConfig();
}

void ReadAllStringsInMemory(){

    fingerPrint = ReadStringInMemory(GameConfiguration.fingerprintAddr);
    user = ReadStringInMemory(GameConfiguration.userNameAddr);
    wifiUID = ReadStringInMemory(GameConfiguration.UIDAddr);
    wifiPass = ReadStringInMemory(GameConfiguration.passAddr);
    newWifiPass = ReadStringInMemory(GameConfiguration.newpassAddr);
    newWifiUID = ReadStringInMemory(GameConfiguration.newUIDAddr);

}


void SetStatus(int status)
{
    GameConfiguration.status = status;
    SaveConfig();
}

void DebugTagInfo(Tag tag){
    Serial.printf("id : %d    type : %d    isComplete : %d    timeToComplete : %d\n", tag.GetID(), tag.GetType(), tag.GetBaliseCompletionState(), tag.timeTocomplete);
}

void LedShowProgression(){
    FastLED.clear(true);
    int baliseCompleted = 0;

    for(int i = 0; i < tagArraySize;i++){
        if(detectedTags[i].GetBaliseCompletionState()){
            baliseCompleted++;
        }
    }
    Serial.print(baliseCompleted);
    float baliseCompletedPercent = ((float)baliseCompleted/(float)tagArraySize);

    int ledToLight = baliseCompletedPercent * ledNumber;


    for(int i = 0; i < ledToLight;i++){
        leds[i] = GameConfiguration.teamColor;
    }
    for(int i = ledToLight; i < ledNumber; i++){
        leds[i] = black;
    }
    isAvailable = true;
    FastLED.show();

}

void LedWifiConnection(){
    for(int i = 0; i < wifiLed;i++){
        leds[i] = green;
    }
    for(int i = wifiLed; i < 8;i++){
        leds[i] = black;
    }
    wifiLed++;
    if(wifiLed > 8){
        wifiLed = 1;
    }
    FastLED.show();
}

void SetLedColor(CRGB color){
    FastLED.clear(true);
    for(int i = 1; i < ledNumber; i++){
        leds[i] = color;
    }
    FastLED.show();
}


int WIFIInit(){
    
    WiFi.begin(wifiUID, wifiPass);
    timer.ResetTimer();
    FastLED.clear(true);
    Serial.println();
    Serial.print("Connecting with UID : " + wifiUID + " pass : " + wifiPass);
    for(int i = 0; i < 100;i++)
    {
        Serial.print(".");
        LedWifiConnection();
        if(WiFi.status() == WL_CONNECTED)
        {
            SetLedColor(green);
            return LINKED_STATUS;
            
        }
        delay(1000);
    }

    return BUG_STATUS;
}

void DownloadNewCertificate(){

    String certificateHost = "54.36.98.8";

    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");

    WiFiClient certificateClient;
    HTTPClient http;

    http.begin(certificateClient, "http://54.36.98.8:3500/");
    int httpCode = http.GET();
    while(httpCode == -1){
        Serial.println("retrying to pull certificate");
        httpCode = http.GET();
    }
    Serial.print(httpCode);

    String payload = http.getString();

    Serial.print(payload);
    fingerPrint = payload;
    const char * fingerprintTemp = payload.c_str();
    WriteAllStringsInMemory(fingerprintTemp, user.c_str(), wifiUID.c_str(), wifiPass.c_str(), newWifiUID.c_str(), newWifiPass.c_str());
    SaveConfig();
    Serial.print("New certificate is" + fingerPrint);

    certificateClient.stop();

}

String httpsGetRequest(String url){
	/* connect to wifi */

	WiFiClientSecure client;
  	Serial.print("connecting to ");
  	Serial.println(host);

  	Serial.printf("Using fingerprint '%s'\n", fingerPrint.c_str());
  	client.setFingerprint(fingerPrint.c_str());

  	if (!client.connect(host, httpsPort)) {
    	Serial.println("connection failed");
        client.stop();
        DownloadNewCertificate();
    	return "0";
	  }
  	Serial.print("requesting URL: ");
  	Serial.println(url);

  	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

	while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      	Serial.println("headers received");
      	break;
   		}
  	}
    String buffer;
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      buffer += c;
    }
    if (buffer[0] != '{'){
        return "0";
    }
    client.stop();
	return buffer;	
}

String httpsPostRequest(String url, String data){

    WiFiClientSecure client;
  	Serial.print("connecting to ");
  	Serial.println(host);

  	Serial.printf("Using fingerprint '%s'\n", fingerPrint.c_str());
  	client.setFingerprint(fingerPrint.c_str());

  	if (!client.connect(host, httpsPort)) {
    	Serial.println("connection failed");
        client.stop();
        DownloadNewCertificate();
    	return "0";
	  }
    Serial.print("test");
  	Serial.print("requesting URL: ");
  	Serial.println(url);
    int contentLen = data.length();

    Serial.println(String("POST ")  + " HTTP/1.1\n" +
                "Host: " + host + "\n" +
                "Content-Type : application/json;charset=UTF-8\n" +
                "Content-Lenght : " + String(contentLen) + "\n\n" + data + "\n");

    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: ecocathlon.fr");
    client.println("Cache-Control: no-cache");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    
    while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      	Serial.println("headers received");
      	break;
   		}
  	}
    String buffer;
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      buffer += c;
    }
	return "1";
}




/*
void WinLedAnimation(){

    if(winled < ledNumber){
        winled++;
        SetLedColor(black);
        for(int i = 0; i < winled;i++)
            leds[i] = GameConfiguration.teamColor;
        return;
    }

    if(winAnimation){
        SetLedColor(black);
        winAnimation = !winAnimation;
    }
    else{
        SetLedColor(GameConfiguration.teamColor);
        winAnimation = !winAnimation;
    }
}
*/


// win animation
void rainbow(){
    FastLED.clear(true);
	winAnimation = true;
    if(winLedLit == 8) winLedLit = 0;
    winLedLit++;
  	for (int  i = 0; i < winLedLit; i++){
        leds[i] = CHSV(i * (255 / 8), SATURATION, BRIGHTNESS);
      }
    for (int  i = winLedLit; i <= NUM_LEDS;i++){
        leds[i] = black;
    }
    FastLED.show();

    timer.AddTimer(millis(), 750, rainbow);
}

void StatusAnimation(){
    FastLED.clear(true);
    StatusAnimationCount++;
    if(isLit)
    {    
        if(StatusAnimationCount == 14){
            Serial.print(StatusAnimationCount);
            StatusAnimationCount = 0;
        }
        else{
            leds[0] = statusColor;
        }
    }
    isLit = !isLit;
    FastLED.show();
    timer.AddTimer(millis(),750, StatusAnimation);
}

void ShowStatusLed(CRGB color){
    timer.ResetTimer();
    FastLED.clear(true);
    statusColor = color;
    StatusAnimation();
}

void SetGameInfo()
{
    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;
    detectedTags = new Tag[tagArraySize];
    detectedTags = GameConfiguration.detectedTags;

}



void LedInit(){

    FastLED.addLeds<NEOPIXEL, 4>(leds, 8);
    FastLED.setBrightness(128);

    FastLED.clear(true);
    FastLED.show();
}

void SystemInit(){

    Serial.begin(115200);
    EEPROM.begin(2048);

    if(EEPROM.read(0) == 0 || EEPROM.read(0) == 255){
        SetStatus(HARDRESET_STATUS);
        detectedTags = new Tag[1];
        memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * 1);
        WriteAllStringsInMemory(fingerPrint.c_str(), user.c_str(), "ecocathlon", "ecocathlon", "ecocathlon", "ecocathlon");
        SaveConfig();
    }


    EEPROM.get(0, GameConfiguration);
    ReadAllStringsInMemory();
    WiFi.persistent(false); 
    LedInit();
    SPI.begin();
	rfid.PCD_Init();

}

void ResetGameConfig(){
    wifiUID = "ecocathlon";
    wifiPass = "ecocathlon";
    WriteAllStringsInMemory(fingerPrint.c_str(), user.c_str(), wifiUID.c_str(), wifiPass.c_str(), newWifiUID.c_str(), newWifiPass.c_str());
    SetStatus(USERSET_STATUS);
    ESP.reset();
}

void ResetGameData(){

    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;

    detectedTags = new Tag[tagArraySize];
    memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
}

Tag ReadNtagContent(){
/*
Read the content of an NTAG213 and return the UID array
*/

    byte PSWBuff[] = {0xFF, 0xFF, 0xFF, 0xFF}; //32 bit PassWord default FFFFFFFF
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
    
    Serial.print("Auth: ");
    Serial.println(rfid.PCD_NTAG216_AUTH(&PSWBuff[0], pACK)); //Request Authentification if return STATUS_OK we are good

    /*
    byte WBuff[] = {0x01, 0x06, 0x01, 0x00};
    byte RBuff[18]; 
    
    Serial.print("CHG BLK: ");
    Serial.println(rfid.MIFARE_Ultralight_Write(0x04, WBuff, 4));  //How to write to a page
    */
	uint8_t buffer[128];


    Tag detectedTag = Tag();
    memcpy(buffer, rfid.PICC_DumpMifareUltralightToSerial(), 128);

    for(int i = 0; i < 128; i += 4){
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }

    int detectedTagID = buffer[16 * 4];
    int detectedTagType = buffer[17 * 4];
    int detectedTagData = buffer[18 * 4];

    detectedTag.SetTagInformation(detectedTagID, detectedTagType, detectedTagData);

    Serial.println();
    Serial.print(detectedTagID, HEX);
    Serial.print(detectedTagType, HEX);
    Serial.println(detectedTagData, HEX);



    if((detectedTagType <=3 || detectedTagType == 7) && GameConfiguration.status != ONGOING_STATUS){
        detectedTag.SetID(9999);
        return detectedTag;
    }

    if ((detectedTagType == 2 || detectedTagType == 3 || detectedTagType == 7) && detectedTagData > 0){
        if(detectedTagID != GameConfiguration.tagQuestionStarted->GetID()){
            Serial.print(GameConfiguration.tagQuestionStarted->GetID());
            Serial.println("Wrong ID !");
            detectedTag.SetID(9999);
            return detectedTag;
        }
        Serial.println("Answer tag detected");
        return detectedTag;
    }

    if(detectedTagType > 3 && detectedTagType!=7){
        Serial.print("Admin tag detected");
        return detectedTag;
    }
    Serial.println(detectedTags[GameConfiguration.tagDetectedNumber - 1].GetID());
    if(GameConfiguration.tagQuestionStarted != NULL){
        if(detectedTags[GameConfiguration.tagDetectedNumber - 1].GetID() != detectedTagID || detectedTags[GameConfiguration.tagDetectedNumber - 1].GetType() != detectedTagType){
            if(detectedTags[GameConfiguration.tagDetectedNumber - 1].GetBaliseCompletionState() == 0){
                Serial.println("Previous question not completed");
                detectedTag.SetID(9999);
                return detectedTag;
            }
        }
    }

    // Check if the tag has already been detected
    // if it has been if its a found then nothing happens if it's a question it's not added twice to the detectedTags array but it's processed anyway
    int tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;
    for (int i = 0; i < tagArraySize - 1; i++){
        if(detectedTags[i].GetID() == detectedTagID){
            if(detectedTags[i].GetType() == detectedTagType){
                if(detectedTags[i].GetBaliseCompletionState() == 0){
                    GameConfiguration.tagDetectedNumber--;
                }
                else{
                    Serial.println("Tag already detected");
                    detectedTag.SetID(9999);
                    return detectedTag;
                }
            }
        }
    }

    detectedTags[GameConfiguration.tagDetectedNumber] = detectedTag;
    GameConfiguration.tagDetectedNumber++;


    return detectedTags[GameConfiguration.tagDetectedNumber - 1];
}


bool CheckWin(){
    for(int i = 0; i < tagArraySize; i++){
        if (detectedTags[i].GetType() == 1){
            detectedTags[i].BaliseFound();
        }
        if(!detectedTags[i].GetBaliseCompletionState()){
            return false;
        }
    }
    return true;
}

int GetTotalTime(){
    int answTime = 0;
    for (int i = 0;i < tagArraySize;i++){
        answTime += detectedTags[i].timeTocomplete; 
    }
    return answTime;
}

int* GetTime(int type){
    int* tagTime = 0;
    int tagMaxNumber = 0;

    switch(type){
        case 1:
            tagTime = new int[GameConfiguration.Trouver];
            tagMaxNumber = GameConfiguration.Trouver;
            break;
        case 2:
            tagTime = new int[GameConfiguration.QCM];
            tagMaxNumber = GameConfiguration.QCM;
            Serial.println(tagMaxNumber);
            break;
        case 3:
            tagTime = new int[GameConfiguration.Ordre];
            tagMaxNumber = GameConfiguration.Ordre;
            break;
    }
    Serial.print("type :");
    Serial.println(type);

    for(int x = 1; x <= tagMaxNumber;x++)
    {
        Serial.print(x);
        for (int y = 0; y < tagArraySize;y++){
            if(detectedTags[y].GetType() == type && detectedTags[y].GetID() == x){
                DebugTagInfo(detectedTags[y]);
                tagTime[detectedTags[y].GetID() - 1] = detectedTags[y].timeTocomplete;
                break;
            }
        }
    }

    return tagTime;
}
void StartGame(){

    if(GameConfiguration.status == DOWNLOADED_STATUS){
        Serial.println("Starting the game");
        startTime = millis();
        timer.ResetTimer();
        GameConfiguration.gameTime = 0;
        SetStatus(ONGOING_STATUS);
        SetLedColor(GameConfiguration.teamColor);
        GameConfiguration.tagDetectedNumber = 0;
        SaveConfig();
        timer.AddTimer(millis(), 5000, *LedShowProgression);
        delay(500);
    }
}
int DownloadGame(){

/*
Download the game data in json for the user from "http://ecocathlon.fr" 
Deserialize the json into readable data and store the data in the EEPROM
*/
    timer.ResetTimer();
    wifiUID = newWifiUID;
    wifiPass = newWifiPass;
    Serial.println("Set password :" + wifiPass + " uid :" + wifiUID);
    WriteAllStringsInMemory(fingerPrint.c_str(), user.c_str(), wifiUID.c_str(), wifiPass.c_str(), newWifiUID.c_str(), newWifiPass.c_str());

    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");
    Serial.print("Downloading game data");
    Serial.print("/api/config/" + String(user) + "?key="+ getKey);
    String StringData = httpsGetRequest("/api/config/" + String(user) + "?key="+ getKey);
    if(StringData == "0"){
        //retrying to pull data from server
        StringData = httpsGetRequest("/api/config/" + String(user) + "?key="+ getKey);
        if(StringData == "0"){
            Serial.print("HTTP request failed");
            SetLedColor(red);
            return 0 ;
        }

    }
    deserializeJson(doc, StringData.c_str());
    
    if(doc["user"] != user)
        SetStatus(BUG_STATUS);
    
    
    GameConfiguration.QCM = doc["qcm"];
    GameConfiguration.Trouver = doc["trouver"];
    GameConfiguration.Ordre = doc["ordre"];
    GameConfiguration.Sondage = doc ["sondage"];
    

    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;

    detectedTags = new Tag[tagArraySize];    
    for(int i = 0; i < tagArraySize; i++){
        detectedTags[i] = Tag();
    }
    memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * tagArraySize);


    String teamColorData = doc["couleur"];
    GameConfiguration.teamColorString = teamColorData;
    Serial.println(teamColorData);
    teamColorData.remove(0, 1);
    teamColorData = "0x" + teamColorData;
    long colorLong = strtol(teamColorData.c_str(), NULL, 16);
    GameConfiguration.teamColor = colorLong;
    teamColorData = colorLong;

    String uid = doc["uid"];
    String password = doc["password"];
    Serial.println("Set new password : " + password + " new uid : " + uid);
    newWifiUID = uid;
    newWifiPass = password;
    WriteAllStringsInMemory(fingerPrint.c_str(), user.c_str(), wifiUID.c_str(), wifiPass.c_str(), newWifiUID.c_str(), newWifiPass.c_str());

    int sondage = doc["sondage"];

    EEPROM.put(0, GameConfiguration);
    EEPROM.commit();

    SetStatus(DOWNLOADED_STATUS);

    WiFi.disconnect();
    doc.clear();

    Serial.println("GameData set");
    Serial.printf("Trouver : %d QCM : %d Ordre : %d sondage : %d", GameConfiguration.Trouver, GameConfiguration.QCM, GameConfiguration.Ordre, sondage);
    return 1;

}



int DownloadAdminData(){
    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");

    String stringData = httpsGetRequest("/api/seeUser");
    if(stringData == "0"){
        // retry to pull data with a new certificate
        Serial.print("retrying to pull data");
        stringData = httpsGetRequest("/api/seeUser");
        if(stringData == "0"){
            Serial.print("HTTP request failed");
            SetLedColor(red);
            WiFi.disconnect();
            return 0;
        }

    }
    WiFi.disconnect();

    deserializeJson(doc, stringData);
    const char* userTemp = doc["orga"];
    Serial.println(userTemp);
    WriteAllStringsInMemory(fingerPrint.c_str(), userTemp, wifiUID.c_str(), wifiPass.c_str(), newWifiUID.c_str(), newWifiPass.c_str());

    doc.clear();
    return 1;
}

void UploadResult(){
    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");

    String json;
    DynamicJsonDocument result(512);


    JsonArray QCMArray = result.createNestedArray("qcm");


    for(int i = 0; i<GameConfiguration.QCM;i++){
        QCMArray.add(String(GetTime(2)[i]));
    }
    
    JsonArray OrdreArray = result.createNestedArray("ordre");

    for(int i = 0; i<GameConfiguration.Ordre;i++){
        OrdreArray.add(String(GetTime(3)[i]));
    }    
    
    JsonArray trouverArray = result.createNestedArray("trouver");

    for(int i = 0; i<GameConfiguration.Trouver;i++){
        trouverArray.add(String(GetTime(1)[i]));
    }

    result["repTt"] = GetTotalTime();

    result["tempsTt"] = GameConfiguration.gameTime;

    result["couleur"] = GameConfiguration.teamColorString;

    JsonArray sondageArray = result.createNestedArray("sondage");

    for(int x = 1; x<=GameConfiguration.Sondage;x++){
        for(int y = 0; y < tagArraySize;y++){
            if(detectedTags[y].GetType() == 7 && detectedTags[y].GetID() == x){
                sondageArray.add(String(detectedTags[y].GetData()));
            }
        }
    }

    serializeJson(result, json);
    Serial.print(json);
    //json = "{\"equipe\":2,\"qcm\":[\"1\"],\"ordre\":[\"2\"],\"trouver\":[\"3\"],\"repTt\":15,\"tempsTt\":\"17:22:10\",\"sondage\":[]}";
    String StringData = httpsPostRequest("/api/resultat/" + String(user) + "/?key=" + postKey + "&couleur=" + GameConfiguration.teamColorString, json);
    if(StringData == "0"){
        //retrying to pull data from server
        StringData = httpsPostRequest("/api/resultat/" + String(user) + "/?key=" + postKey + "&couleur=" + GameConfiguration.teamColorString, json);
        if(StringData == "0"){
            Serial.print("HTTPs request failed");
            SetLedColor(red);
            return;
        }
    }
    
    http.end();
    doc.clear();
    WiFi.disconnect();

}




void setup() {
  // put your setup code here, to run once:
    SystemInit();
    ShowStatusLed(orange);
    Serial.println("Esp started successfully");
    Serial.println(user);
    Serial.println(GameConfiguration.status);
    /*
    DownloadAdminData();
    SetStatus(DOWNLOADED_STATUS);

    DownloadGame();
    StartGame();
    delay(2000);
    UploadResult();
    */
    for(int i = 0; i < tagArraySize;i++){
        DebugTagInfo(detectedTags[i]);
    }
    
    memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
    isAvailable = true;

    switch (GameConfiguration.status)
    {
    case HARDRESET_STATUS:
        Serial.print("HARDRESET");
        break;
        
    case USERSET_STATUS:
        Serial.print("USERSET");
        FastLED.clear();
        ShowStatusLed(white);
        break;
    case DOWNLOADED_STATUS:
        Serial.print("DOWNLOADED");
        SetGameInfo();
        ShowStatusLed(green);

        break;
    case ONGOING_STATUS:
        Serial.print("ONGOING");
        SetGameInfo();
        timer.ResetTimer();
        LedShowProgression();
        for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
        }
        break;
    default:
        break;
    }



}

void loop() {
  // put your main code here, to run repeatedly:
    /*
    detectedTags[0].baliseComplete = 1;
    detectedTags[0].timeTocomplete = 10;
    detectedTags[1].baliseComplete = 1;
    detectedTags[1].timeTocomplete = 20;
    memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * tagArraySize);
    SaveConfig();
    */
   /*
    for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(GameConfiguration.detectedTags[i]);
    }
    */
 	timer.UpdateTimer(millis());

    if((millis() - startTime) > 5000 && GameConfiguration.status == ONGOING_STATUS){
        startTime = millis();
        GameConfiguration.gameTime += 5;
        SaveConfig();
    }





    

    espTime = now();
    if(CheckWin() && !winAnimation && GameConfiguration.status == ONGOING_STATUS) {
        isAvailable = true;
        Serial.print("win xd");
        timer.ResetTimer();
        for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
        }
        rainbow();
    }
    

    if ( ! rfid.PICC_IsNewCardPresent())    return;
    if ( ! rfid.PICC_ReadCardSerial())      return;
    
Serial.print("test");

    Tag detectedTag = ReadNtagContent();

    if(detectedTag.GetID() == 9999){
        delay(1000);
        return;
    }


    if(GameConfiguration.status != ONGOING_STATUS && detectedTag.GetType() != 4 && detectedTag.GetType() != 6 && detectedTag.GetType() != 5) return;
    if(!isAvailable) return;


    switch (detectedTag.GetType())
    {
    case 1:
    // Balise Trouver
        isAvailable = false;
        detectedTags[GameConfiguration.tagDetectedNumber - 1].BaliseFound();
        detectedTags[GameConfiguration.tagDetectedNumber - 1].timeTocomplete = GameConfiguration.gameTime;
        SetLedColor(green);
        timer.AddTimer(millis(), 2000, *LedShowProgression);
        Serial.println("balise trouver");
        break;
    case 2:
    // Balise QCM
        if(detectedTag.GetData() == 0){
            // Start the question
            isAvailable = false;
            Serial.println("Question QCM commencée");
            detectedTags[GameConfiguration.tagDetectedNumber - 1].StartQuestion(espTime);
            detectedTags[GameConfiguration.tagDetectedNumber - 1].secStart = minute(espTime) * 60 + second(espTime);

            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            GameConfiguration.tagQuestionStarted = &detectedTags[GameConfiguration.tagDetectedNumber - 1];
            break;
        }else if (detectedTag.GetData() > 0)
        {
            // If an answer tag is detected but no QCM question has started yet
            if(GameConfiguration.tagQuestionStarted == NULL){
                Serial.print("Aucune question n'a été commencée");
                break;
            }
            if(GameConfiguration.tagQuestionStarted->TestQCM(detectedTag.GetData())){
                 // Right answer
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                Serial.println(second(GameConfiguration.tagQuestionStarted->GetTimeToComplete(espTime)));
                Serial.println(GameConfiguration.tagQuestionStarted->secStart);    
                Serial.printf("question réussi en %d\n", GameConfiguration.tagQuestionStarted->timeTocomplete);
                GameConfiguration.tagQuestionStarted = NULL;
            }
            else{
                //Wrong Answer
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
            }
            break;
        }
    case 3:
    // Balise Trouver
        if(detectedTag.GetData() == 0){
            // Start the question
            Serial.println("Question ordre commencé");
            Serial.println("");
            detectedTags[GameConfiguration.tagDetectedNumber - 1].StartQuestion(espTime);
            detectedTags[GameConfiguration.tagDetectedNumber - 1].secStart = minute(espTime) * 60 + second(espTime);
            isAvailable = false;
            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            GameConfiguration.tagQuestionStarted = &detectedTags[GameConfiguration.tagDetectedNumber - 1];
            break;
        }else if (detectedTag.GetData() > 0)
        {
            // If an answer tag is detected but no Trouver question has started yet
            if(GameConfiguration.tagQuestionStarted == NULL){
                Serial.println("Aucune question n'a été commencée");
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                break;
            }
            if(GameConfiguration.tagQuestionStarted->GetHasStarted() && GameConfiguration.tagQuestionStarted->TestOrder(detectedTag.GetData())){
                // Right Answer
                Serial.println("bon ordre");
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                if(GameConfiguration.tagQuestionStarted->GetBaliseCompletionState()){
                    Serial.println(GameConfiguration.tagQuestionStarted->GetTimeToComplete(espTime));
                    Serial.printf("question réussi en %d secondes\n", GameConfiguration.tagQuestionStarted->timeTocomplete);
                    GameConfiguration.tagQuestionStarted = NULL;
                    break;
                }
            break;
            }
            else{
                // Wrong answer
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                break;
                }
        }
    case 4:
        // Start the game
        if(GameConfiguration.status == DOWNLOADED_STATUS){
            timer.ResetTimer();
            StartGame();
        break;
        }
        else{
            // End the game
            if(CheckWin() && GameConfiguration.status == ONGOING_STATUS){
                    timer.ResetTimer();
                    UploadResult();
                    ResetGameData();
                    SetStatus(DOWNLOADED_STATUS);
                    ShowStatusLed(green);
                    ESP.reset();
                    break;
                }
        }
        ESP.reset();
        break;
    case 5:
        // Force the download of a new game
        timer.ResetTimer();
        if (DownloadGame() == 1){
            ShowStatusLed(green);
        }
        break;
    case 6:
        // download User and wifi data
        if(DownloadAdminData() == 0){
            return;
        }

        SetStatus(USERSET_STATUS);
        ESP.reset();
        break;
    case 7:
        // Balise sondage
        detectedTags[GameConfiguration.tagDetectedNumber - 1].BaliseFound();
        SetLedColor(green);
        timer.AddTimer(millis(), 2000, *LedShowProgression);
        Serial.println("sondage : %i");
        break;

    default:
        Serial.print("Invalid type");
        break;
    }
    
    memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
    SaveConfig();
    FastLED.show();
    for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
    }
    delay(500);
}