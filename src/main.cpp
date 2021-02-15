#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>
#include <FastLED.h>
#include <iostream>
#include <eco-err.h>
#include <Tag.h>
#include <EEPROM.h>
#include <ArduinoJson.h>  
#include <MFRC522.h>
#include <string>
#include <TimeLib.h>
#include <ctimer.h>


#define NUM_LEDS 8
#define LED_PIN 2
#define CS 15
#define RST 5
#define tagDataOffset 64

#define ledNumber 8
#define BRIGHTNESS 255   /* Control the brightness of your leds */
#define SATURATION 255 

StaticJsonDocument<200> doc;

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
int winLedLit = 0;
bool isAvailable;

time_t espTime = now();

String postKey = "bd824a00-53da-4281-bb85-e4839eebbf59";
String getKey = "19096661-1fb2-427e-9c55-c4839bcb3f02";

bool ledTimer;
bool winAnimation = false;

int sondageAnswered;

struct GameConfig_t {
    int status;
    int teamID;
    bool gameStarted;
    int teamColor;
    char startTime[7];
 
    char wifiUID[20];
    char wifiPass[20];

    int QCM;
    int Trouver;
    int Ordre;
    int Sondage;
    char user[40];
    Tag *tagQuestionStarted = NULL;
    Tag detectedTags[25];

} GameConfiguration;



Tag *detectedTags;
Tag *tagQuestionStarted = NULL;
int tagDetectedNumber = 0;

int tagArraySize;

int QuestionType;
// int status = WL_IDLE_STATUS;
/*
char ssid[] = "k3z";
char pass[] = "krOT9DjR4t2dyFDdDQJs";
*/

char ssid[] = "ecocathlon";
char pass[] = "ecocathlon";

const char* host = "ecocathlon.fr";
const int httpsPort = 443;

const char fingerprint[] PROGMEM = "C4:3C:BA:9D:89:60:8F:39:3C:53:7B:5B:D8:66:6B:51:D7:77:29:BD";
                        
MFRC522 rfid(CS, RST);

void SaveConfig(){
    EEPROM.put(0, GameConfiguration);
    EEPROM.commit();

}


void SetUser(const char* user)
{
    strcpy(GameConfiguration.user,user);
    SaveConfig();

}

void SetStatus(int status)
{
    GameConfiguration.status = status;
    SaveConfig();
}

void DebugTagInfo(Tag tag){
    Serial.printf("id : %d    type : %d    isComplete : %d    timeToComplete : %d\n", tag.tagID, tag.tagType, tag.baliseComplete, tag.timeTocomplete);
}


String httpsGetRequest(String url){
	/* connect to wifi */

	WiFiClientSecure client;
  	Serial.print("connecting to ");
  	Serial.println(host);

  	Serial.printf("Using fingerprint '%s'\n", fingerprint);
  	client.setFingerprint(fingerprint);

  	if (!client.connect(host, httpsPort)) {
    	Serial.println("connection failed");
    	return "connection error";
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
	String line = client.readStringUntil('\n');
	return line;	
}

String httpsPostRequest(String url, String data){

    WiFiClientSecure client;
  	Serial.print("connecting to ");
  	Serial.println(host);

  	Serial.printf("Using fingerprint '%s'\n", fingerprint);
  	client.setFingerprint(fingerprint);

  	if (!client.connect(host, httpsPort)) {
    	Serial.println("connection failed");
    	return "connection error";
	  }
  	Serial.print("requesting URL: ");
  	Serial.println(url);
    int contentLen = data.length();
    Serial.println(String("POST ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Content-Type: application/json; charset=utf-8\r\n" +
                "Content-Lenght: " + String(contentLen) + "\r\n\r\n" +
                data);

  	client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Content-Type : application/json; charset=utf-8\r\n" +
                "Content-Lenght : " + String(contentLen) + "\r\n\r\n" +
                data + "\r\n\r\n");
    
    while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      	Serial.println("headers received");
      	break;
   		}
  	}
	String line = client.readStringUntil('\r\n\r\n');
	return line;
}




/*
void testhttps(){
    client.connect(host, 80);

    if (client.verify(fingerpr, host)) 
    {  
        http.begin(client, host);

        String payload;
        if (http.GET() == HTTP_CODE_OK)    
            payload = http.getString();
            Serial.print(payload);    
    }
    else 
    {
        Serial.println("certificate doesn't match");
    }
}
*/
void LedShowProgression(){
    int baliseCompleted = 0;

    for(int i = 0; i < tagArraySize;i++){
        if(detectedTags[i].baliseComplete){
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
    for(int i = 0; i < ledNumber; i++){
        leds[i] = color;
    }
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

    timer.AddTimer(millis(), 1000, rainbow);
}


void SetGameInfo()
{
    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;
    detectedTags = new Tag[tagArraySize];
    detectedTags = GameConfiguration.detectedTags;

}

int WIFIInit(){

    WiFi.begin(ssid, pass);

    Serial.println();
    Serial.print("Connecting");
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

void LedInit(){

    FastLED.addLeds<NEOPIXEL, 4>(leds, 8);
    FastLED.setBrightness(63);

    for(int i = 0; i < ledNumber;i++){
        leds[i] = black;
    }
    FastLED.show();
}

void SystemInit(){

    Serial.begin(115200);
    EEPROM.begin(1024);

    if(EEPROM.read(0) == 0 || EEPROM.read(0) == 255){
        SetStatus(HARDRESET_STATUS);
        detectedTags = new Tag[1];
        memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * 1);
        SaveConfig();
    }
    EEPROM.get(0, GameConfiguration);

    LedInit();
    SPI.begin();
	rfid.PCD_Init();

}

void ResetGameConfig(){
    strcpy(GameConfiguration.wifiUID, "ecocathlon");
    strcpy(GameConfiguration.wifiPass, "ecocathlon");

    SetStatus(USERSET_STATUS);
    ESP.restart();
}

Tag ReadNtagContent(){
/*
Read the content of an NTAG213 and return the UID array
*/

    byte PSWBuff[] = {0xFF, 0xFF, 0xFF, 0xFF}; //32 bit PassWord default FFFFFFFF
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
    /*
    Serial.print("Auth: ");
    Serial.println(rfid.PCD_NTAG216_AUTH(&PSWBuff[0], pACK)); //Request Authentification if return STATUS_OK we are good

    
    byte WBuff[] = {0x00, 0x04, 0x00, 0x00};
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

    detectedTag.tagID = buffer[16 * 4];
    detectedTag.tagType = buffer[17 * 4];
    detectedTag.tagData = buffer[18 * 4];

    Serial.println();
    Serial.print(detectedTag.tagID, HEX);
    Serial.print(detectedTag.tagType, HEX);
    Serial.println(detectedTag.tagData, HEX);

    if((detectedTag.tagType <=3 || detectedTag.tagType == 7) && GameConfiguration.status != ONGOING_STATUS){
        detectedTag.tagID = 9999;
        return detectedTag;
    }

    if ((detectedTag.tagType == 2 || detectedTag.tagType == 3 || detectedTag.tagType == 7) && detectedTag.tagData > 0){
        Serial.println("Answer tag detected");
        return detectedTag;
    }

    if(detectedTag.tagType > 3 && detectedTag.tagType!=7){
        Serial.print("Admin tag detected");
        return detectedTag;
    }

    // Check if the tag has already been detected
    // if it has been if its a found then nothing happens if it's a question it's not added twice to the detectedTags array but it's processed anyway
    for (int i = 0; i < sizeof(detectedTags) - 1; i++){
        if(detectedTags[i].tagID == detectedTag.tagID){
            if(detectedTags[i].tagType == detectedTag.tagType){
                Serial.println("Tag already detected");
                detectedTag.tagID = 9999;
                return detectedTag;
            }

        }
    }

    detectedTags[tagDetectedNumber] = detectedTag;
    tagDetectedNumber++;


    return detectedTags[tagDetectedNumber - 1];
}


bool CheckWin(){
    for(int i = 0; i < tagArraySize; i++){
        if(!detectedTags[i].baliseComplete){
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
    int* tagTime;
    int tagMaxNumber;

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
            if(detectedTags[y].tagType == type && detectedTags[y].tagID == x){
                DebugTagInfo(detectedTags[y]);
                tagTime[detectedTags[y].tagID - 1] = detectedTags[y].timeTocomplete;
                break;
            }
        }
    }

    return tagTime;
}

void DownloadGame(){

/*
Download the game data in json for the user from "http://68.183.211.90:80" 
Deserialize the json into readable data and store the data in the EEPROM
*/
    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");
    String user = GameConfiguration.user;
    Serial.print("Downloading game data");
    String StringData = httpsGetRequest("/api/config/" + String(GameConfiguration.user) + "?key="+ getKey + "&teamId=" + String(GameConfiguration.teamID));
    if(StringData == "0"){
        Serial.print("HTTP request failed");
        SetLedColor(red);
        return;
    }
    deserializeJson(doc, StringData.c_str());
    
    if(doc["user"] != GameConfiguration.user)
        SetStatus(BUG_STATUS);
    
    
    GameConfiguration.QCM = doc["qcm"];
    GameConfiguration.Trouver = doc["trouver"];
    GameConfiguration.Ordre = doc["ordre"];
    GameConfiguration.Sondage = doc ["sondage"];
    

    strcpy(GameConfiguration.startTime, doc["time"]);
    Serial.print(GameConfiguration.startTime);
    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre + GameConfiguration.Sondage;

    detectedTags = new Tag[tagArraySize];    
    for(int i = 0; i < tagArraySize; i++){
        detectedTags[i] = Tag();
    }
    memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * tagArraySize);

    if(doc["uid"] == GameConfiguration.wifiUID || doc["password"] == GameConfiguration.wifiPass){
        strcpy(GameConfiguration.wifiUID, doc["uid"]);
        strcpy(GameConfiguration.wifiPass, doc["pass"]);
    }

    String teamColorData = doc["couleur"];
    Serial.println(teamColorData);
    teamColorData.remove(0, 1);
    teamColorData = "0x" + teamColorData;
    long colorLong = strtol(teamColorData.c_str(), NULL, 16);
    GameConfiguration.teamColor = colorLong;
    teamColorData = colorLong;
    int sondage = doc["sondage"];
    EEPROM.put(0, GameConfiguration);
    EEPROM.commit();
    SetStatus(DOWNLOADED_STATUS);
    WiFi.disconnect();
    Serial.println("GameData set");
    Serial.printf("Trouver : %d QCM : %d Ordre : %d sondage : %d", GameConfiguration.Trouver, GameConfiguration.QCM, GameConfiguration.Ordre, sondage);

}

void DownloadAdminData(){
    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");

    String stringData = httpsGetRequest("/api/seeUser");
    // testhttps();
    deserializeJson(doc, stringData);
    String userTemp = doc["orga"];
    GameConfiguration.teamID = doc["nbMedaillon"];
    GameConfiguration.teamID = 2;
    Serial.println(userTemp);
    SetUser(userTemp.c_str());
}

void UploadResult(){
    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");

    String json;
    DynamicJsonDocument result(2048);

    result["equipe"] = GameConfiguration.teamID;

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

    result["tempsTt"] = GameConfiguration.startTime;

    JsonArray sondageArray = result.createNestedArray("sondage");

    for(int x = 1; x<=GameConfiguration.Sondage;x++){
        for(int y = 0; y < tagArraySize;y++){
            if(detectedTags[y].tagType == 7 && detectedTags[y].tagID == x){
                sondageArray.add(String(detectedTags[y].tagData));
            }
        }
    }

    serializeJson(result, json);
    Serial.print(json);
    strcpy(GameConfiguration.user, "Mathias");
    GameConfiguration.teamID = 1;
    json = "{\"equipe\":0,\"qcm\":[\"1\"],\"ordre\":[\"2\"],\"trouver\":[\"3\"],\"repTt\":15,\"tempsTt\":\"17:22:10\",\"sondage\":[]}";
    Serial.print(httpsPostRequest("/api/resultat/" + String(GameConfiguration.user) + "?key=" + postKey + "&teamId=" + String(GameConfiguration.teamID), json));
    http.end();

    WiFi.disconnect();
}




void setup() {
  // put your setup code here, to run once:
    SystemInit();
    SetLedColor(orange);
    Serial.print("Esp started successfully");
    /*
    SetUser("Mathias");
    */
    Serial.print(GameConfiguration.user);
    Serial.print(GameConfiguration.teamID);
    Serial.print(GameConfiguration.status);
    for(int i = 0; i < tagArraySize;i++){
        DebugTagInfo(detectedTags[i]);
    }
    memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
    isAvailable = true;
    switch (GameConfiguration.status)
    {
    case HARDRESET_STATUS:
        break;
        
    case USERSET_STATUS:
        DownloadGame();
        break;
    case DOWNLOADED_STATUS:
        SetLedColor(green);
    case ONGOING_STATUS:
        SetGameInfo();
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

    FastLED.show();
    espTime = now();

    if(CheckWin() && !winAnimation && GameConfiguration.status == ONGOING_STATUS) {
        Serial.print("win xd");
        for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
        }
        rainbow();
    }

    if ( ! rfid.PICC_IsNewCardPresent())    return;
    if ( ! rfid.PICC_ReadCardSerial())      return;

    Tag detectedTag = ReadNtagContent();

    if(detectedTag.tagID == 9999){
        delay(1000);
        return;
    }



    if(GameConfiguration.status != ONGOING_STATUS && detectedTag.tagType != 4 && detectedTag.tagType != 6) return;
    if(!isAvailable) return;


    switch (detectedTag.tagType)
    {
    case 1:
        detectedTags[tagDetectedNumber - 1].BaliseFound();
        detectedTags[tagDetectedNumber - 1].timeTocomplete = espTime;
        GameConfiguration.detectedTags[0];
        isAvailable = false;
        SetLedColor(green);
        timer.AddTimer(millis(), 2000, *LedShowProgression);
        Serial.println("balise trouver");
        break;
    case 2:
        if(detectedTag.tagData == 0){

            Serial.println("Question QCM commencée");
            detectedTags[tagDetectedNumber - 1].StartQuestion(espTime);
            detectedTags[tagDetectedNumber - 1].secStart = minute(espTime) * 60 + second(espTime);
            isAvailable = false;
            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            GameConfiguration.tagQuestionStarted = &detectedTags[tagDetectedNumber - 1];
            break;
        }else if (detectedTag.tagData > 0)
        {
            if(GameConfiguration.tagQuestionStarted == NULL){
                Serial.print("Aucune question n'a été commencée");
                break;
            }
            if(GameConfiguration.tagQuestionStarted->TestQCM(detectedTag.tagData)){
                    // Question reussi
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                Serial.println(second(GameConfiguration.tagQuestionStarted->GetTimeToComplete(espTime)));
                Serial.println(GameConfiguration.tagQuestionStarted->secStart);    
                Serial.printf("question réussi en %d", GameConfiguration.tagQuestionStarted->timeTocomplete);
                GameConfiguration.tagQuestionStarted = NULL;
            }
            else{
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
            }
            break;
        }
    case 3:
        if(detectedTag.tagData == 0){
            Serial.println("Question ordre commencé");
            Serial.println("");
            detectedTags[tagDetectedNumber - 1].StartQuestion(espTime);
            detectedTags[tagDetectedNumber - 1].secStart = minute(espTime) * 60 + second(espTime);
            isAvailable = false;
            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            GameConfiguration.tagQuestionStarted = &detectedTags[tagDetectedNumber - 1];
            break;
        }else if (detectedTag.tagData > 0)
        {
            if(GameConfiguration.tagQuestionStarted == NULL){
                Serial.println("Aucune question n'a été commencée");
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                break;
            }
            if(GameConfiguration.tagQuestionStarted->QuestionHasStarted && GameConfiguration.tagQuestionStarted->TestOrder(detectedTag.tagData)){
                // Question reussi
                Serial.println("bon ordre");
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                if(GameConfiguration.tagQuestionStarted->baliseComplete){
                    Serial.printf("question réussi en %d secondes\n", GameConfiguration.tagQuestionStarted->timeTocomplete);
                    GameConfiguration.tagQuestionStarted = NULL;
                }
            }
            else{
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                }
            break;
        }
    case 4:
        // Start the game
        if(GameConfiguration.status == DOWNLOADED_STATUS){
        Serial.println("Starting the game");
        SetStatus(ONGOING_STATUS);
        SetLedColor(GameConfiguration.teamColor);
        timer.AddTimer(millis(), 5000, *LedShowProgression);
        delay(500);
        break;
        }

    case 5:
        // Force the download of a new game
        SetStatus(USERSET_STATUS);
        ESP.restart();
        break;
    case 6:
        // download User and wifi data
        DownloadAdminData();
        SetStatus(USERSET_STATUS);
        ESP.restart();
        break;
    case 7:
        detectedTags[tagDetectedNumber - 1].baliseComplete = true;
        SetLedColor(green);
        timer.AddTimer(millis(), 2000, *LedShowProgression);
        Serial.println("sondage : %i");
        break;
    case 8:
        if(CheckWin() && GameConfiguration.status == ONGOING_STATUS){
            UploadResult();
            SetStatus(USERSET_STATUS);
            SetLedColor(green);
            return;
            delay(1000);
        }
    default:
        Serial.print("Invalid type");
        break;
    }
    
    memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
    SaveConfig();

    for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
    }
    bool win = CheckWin();
    Serial.print("win :");
    Serial.println(win);
    delay(500);
}