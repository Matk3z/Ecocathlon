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

StaticJsonDocument<200> doc;

HTTPClient http;
Ctimer timer;

int wifiLed = 1;
int winled = 1;
int ledNumber = 8;
CRGB leds[8];

CRGB green = CRGB::Green;
CRGB red = CRGB::Red;
CRGB blue = CRGB::Blue;
CRGB black = CRGB::Black;

int teamColor = CRGB::White;

bool isAvailable;

time_t espTime = now();

String postKey = "bd824a00-53da-4281-bb85-e4839eebbf59";
String getKey = "19096661-1fb2-427e-9c55-c4839bcb3f02";

bool ledTimer;
bool winAnimation;

bool sondageAnswered;
int sondageData;

struct GameConfig_t {
    int status;
    int teamID;
    bool gameStarted;
    int teamColor;
    char startTime[7];
    char user[40]; 

    char wifiUID[20];
    char wifiPass[20];

    int QCM;
    int Trouver;
    int Ordre;


    Tag detectedTags[30];

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

char ssid[] = "LivingLab";
char pass[] = "123fablab";


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


String httpsRequest(String address)
{


    http.begin("https://www.ecocathlon.fr" + address, "74 FB 04 B3 BF 73 B3 29 3E 85 2F FE 7A 9B 0F 29 59 D5 4D DC");
 
    int httpCode = http.GET();
    Serial.print(httpCode);

        if(httpCode > 0)
    {
        String data = http.getString();
        return data;
    }

    return "0";

  http.end();


}
void LedShowProgression(){
    int baliseCompleted = 0;

    for(int i = 0; i < tagArraySize;i++){
        if(detectedTags[i].baliseComplete){
            baliseCompleted++;
        }
    }

    float baliseCompletedPercent = ((float)baliseCompleted/(float)tagArraySize);

    int ledToLight = baliseCompletedPercent * ledNumber;


    for(int i = 0; i < ledToLight;i++){
        leds[i] = teamColor;
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


void WinLedAnimation(){

    if(winled < ledNumber){
        winled++;
        SetLedColor(black);
        for(int i = 0; i < winled;i++)
            leds[i] = teamColor;
        return;
    }

    if(winAnimation){
        SetLedColor(black);
        winAnimation = !winAnimation;
    }
    else{
        SetLedColor(teamColor);
        winAnimation = !winAnimation;
    }
}


void SetGameInfo()
{
    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre;
    detectedTags = new Tag[tagArraySize];
    detectedTags = GameConfiguration.detectedTags;

    teamColor = GameConfiguration.teamColor;
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

    if(EEPROM.read(0) == 0){
        SetStatus(HARDRESET_STATUS);
        detectedTags = new Tag[1];
        memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * 1);
        SaveConfig();
    }
    EEPROM.get(0, GameConfiguration);

    LedInit();

    while (WIFIInit()  != LINKED_STATUS);
    Serial.println("Successfully connected to the network");
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

    Serial.print("Auth: ");
    Serial.println(rfid.PCD_NTAG216_AUTH(&PSWBuff[0], pACK)); //Request Authentification if return STATUS_OK we are good

    /*
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

    if (detectedTag.tagType >= 2 && detectedTag.tagData > 0){
        Serial.println("Answer tag detected");
        return detectedTag;
    }

    if(detectedTag.tagType > 3){
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
            break;
        case 3:
            tagTime = new int[GameConfiguration.Ordre];
            tagMaxNumber = GameConfiguration.Ordre;
            break;
    }
    for(int x = 0; x < tagMaxNumber;x++);
        for (int y = 0; y < tagArraySize;y++){
            if(detectedTags[y].tagType == type && detectedTags[y].tagID == tagMaxNumber){
                DebugTagInfo(detectedTags[y]);
                tagTime[detectedTags[y].tagID] = detectedTags[y].timeTocomplete;
                break;
            }
    }

    return tagTime;
}

void DownloadGame(){

/*
Download the game data in json for the user from "http://68.183.211.90:80" 
Deserialize the json into readable data and store the data in the EEPROM
*/
    String user = GameConfiguration.user;
    Serial.print("Downloading game data");
    String StringData = httpsRequest("/api/config/" + user + "?key="+ getKey + "&teamId=" + String(GameConfiguration.teamID));
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

    strcpy(GameConfiguration.startTime, doc["time"]);

    tagArraySize = GameConfiguration.QCM + GameConfiguration.Trouver + GameConfiguration.Ordre;

    detectedTags = new Tag[tagArraySize];
    memcpy(GameConfiguration.detectedTags,detectedTags, sizeof(Tag) * tagArraySize);
    
    for(int i = 0; i < tagArraySize; i++){
        detectedTags[i] = Tag();
    }

    if(doc["uid"] == GameConfiguration.wifiUID || doc["password"] == GameConfiguration.wifiPass){
        strcpy(GameConfiguration.wifiUID, doc["uid"]);
        strcpy(GameConfiguration.wifiPass, doc["pass"]);
    }

    String teamColor = doc["couleur"];
    teamColor.remove(0);

    long colorLong = strtol(teamColor.c_str(), 0, 16);

    GameConfiguration.teamColor = colorLong;
    teamColor = colorLong;

    EEPROM.put(0, GameConfiguration);
    EEPROM.commit();
    SetStatus(DOWNLOADED_STATUS);
    
    Serial.print("GameData set");

}

void DownloadAdminData(){
    String stringData = httpsRequest("/api/getUser");

    
    SetUser(stringData.c_str());
}

void UploadResult(){
    String json;
    DynamicJsonDocument result(2048);

    result["equipe"] = GameConfiguration.teamID;

    JsonArray QCMArray = result.createNestedArray("qcm");


    for(int i = 0; i<GameConfiguration.QCM;i++){
        QCMArray.add(String(GetTime(2)[i + 1]));
    }
    
    JsonArray OrdreArray = result.createNestedArray("ordre");

    for(int i = 0; i<GameConfiguration.Ordre;i++){
        OrdreArray.add(String(GetTime(3)[i + 1]));
    }    
    
    JsonArray trouverArray = result.createNestedArray("trouver");

    for(int i = 0; i<GameConfiguration.Trouver;i++){
        trouverArray.add(String(GetTime(1)[i + 1]));
    }

    result["repTt"] = GetTotalTime();

    result["time"] = GameConfiguration.startTime;

    result["sondage"] = sondageData;

    http.begin("https://www.ecocathlon.fr/api/resultat/" + String(GameConfiguration.user) + "/?key=" + postKey + "&teamId=" + String(GameConfiguration.teamID), "B9 91 18 B0 E9 2A 74 73 CA 4F AD E1 75 89 F8 48 2B 84 9A 09");
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(json);

    switch(code){
        case 200:
            Serial.print("OK");
            break;
        default:
            Serial.print("Error while sending data");
            break;
    }


    http.end();
}




void setup() {
  // put your setup code here, to run once:
    SystemInit();


    if(GameConfiguration.status == LINKED_STATUS)
        Serial.print("Esp started successfully");
    GameConfiguration.teamID = 1;
    SetUser("Mathias");
    SetStatus(USERSET_STATUS);

    isAvailable = true;

    switch (GameConfiguration.status)
    {
    case HARDRESET_STATUS:
        break;
        
    case USERSET_STATUS:
        DownloadGame();
        break;
    case ONGOING_STATUS:
        SetGameInfo();
        break;
    default:
        break;
    }

    /*
    GameConfiguration.Trouver = 1;
    GameConfiguration.QCM = 1;
    GameConfiguration.Ordre = 1;

    tagArraySize = 3;
    detectedTags = new Tag[tagArraySize];

    for(int i = 0; i < tagArraySize; i++){
        detectedTags[i] = Tag();
    }
    */
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
    if(GameConfiguration.detectedTags != detectedTags){
        memcpy(GameConfiguration.detectedTags, detectedTags, sizeof(Tag) * tagArraySize);
        SaveConfig();
    }


    if(CheckWin() && GameConfiguration.status == ONGOING_STATUS){
        Serial.print("Win!");
        WinLedAnimation();
        if(sondageAnswered){
            UploadResult();
            SetStatus(USERSET_STATUS);
            SetLedColor(green);
            return;
        }
        delay(1000);
    }

    if ( ! rfid.PICC_IsNewCardPresent())    return;
    if ( ! rfid.PICC_ReadCardSerial())      return;

    Tag detectedTag = ReadNtagContent();

    if(detectedTag.tagID == 9999){
        delay(1000);
        return;
    }



    if(GameConfiguration.status != ONGOING_STATUS && detectedTag.tagType != 4) return;
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
            isAvailable = false;
            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            tagQuestionStarted = &detectedTags[tagDetectedNumber - 1];
            break;
        }else if (detectedTag.tagData > 0)
        {
            if(tagQuestionStarted == NULL){
                Serial.print("Aucune question n'a été commencée");
                break;
            }
            if(tagQuestionStarted->TestQCM(detectedTag.tagData)){
                    // Question reussi
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);    
                tagQuestionStarted->timeTocomplete = (minute(espTime) * 60 + second(espTime)) - (minute(tagQuestionStarted->QuestionStartTime) * 60 + second(tagQuestionStarted->QuestionStartTime));
                Serial.printf("question réussi en %d", tagQuestionStarted->timeTocomplete);
                tagQuestionStarted = NULL;
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
            detectedTags[tagDetectedNumber - 1].StartQuestion(espTime);
            isAvailable = false;
            SetLedColor(blue);
            timer.AddTimer(millis(), 2000, *LedShowProgression); 
            tagQuestionStarted = &detectedTags[tagDetectedNumber - 1];
            break;
        }else if (detectedTag.tagData > 0)
        {
            if(tagQuestionStarted == NULL){
                Serial.println("Aucune question n'a été commencée");
                isAvailable = false;
                SetLedColor(red);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                break;
            }
            if(tagQuestionStarted->QuestionHasStarted && tagQuestionStarted->TestOrder(detectedTag.tagData)){
                // Question reussi
                Serial.println("bon ordre");
                isAvailable = false;
                SetLedColor(green);
                timer.AddTimer(millis(), 2000, *LedShowProgression);
                if(tagQuestionStarted->baliseComplete){
                    tagQuestionStarted->timeTocomplete = (minute(espTime) * 60 + second(espTime)) - (minute(tagQuestionStarted->QuestionStartTime) * 60 + second(tagQuestionStarted->QuestionStartTime));
                    Serial.printf("question réussi en %d secondes\n", tagQuestionStarted->timeTocomplete);
                    tagQuestionStarted = NULL;
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
        Serial.println("Starting the game");
        SetStatus(ONGOING_STATUS);
        SetLedColor(teamColor);
        timer.AddTimer(millis(), 5000, *LedShowProgression);
        break;
    case 5:
        // Force the download of a new game
        SetStatus(USERSET_STATUS);
        ESP.restart();
        break;
    case 6:
        // download User and wifi data
        DownloadAdminData();
        break;
    case 7:
        // balise sondage
        sondageAnswered = true;
        sondageData = detectedTag.tagType;
        break;
    default:
        Serial.print("Invalid type");
        break;
    }
    
    for(int i = 0; i < tagArraySize;i++){
            DebugTagInfo(detectedTags[i]);
    }

    delay(500);
}