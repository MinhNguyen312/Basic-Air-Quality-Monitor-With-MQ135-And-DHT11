//  Blynk IoT Connection Setup
#define BLYNK_TEMPLATE_ID "YOUR_DEVICE_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_DEVICE_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_DEVICE_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>
#include "MQ135.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include <TimeLib.h>
#include <BlynkSimpleEsp8266.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

//  DHT11 Set up
#define DHT_PIN 12  // D6 on ESP8266
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE); 

//  MQ135 gasSensor Setup
const int ANALOGPIN = 0;
MQ135 gasSensor = MQ135(ANALOGPIN,237.97,88); //  237.97 (RZERO at clean outdoor air), 88 (Sensor On-board Resistance)


//  WiFi Setup

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";


// Firebase Connection Setup
#define DATABASE_URL "YOUR_DATABASE_URL"
#define API_KEY "YOUR_WEB_API_KEY"

// Define Firebase Data Object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//  Flag value to indicate time to 
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//  Database main path
String databasePath = "AQM/";

//  Database Child Path
String co2Path = "/CO2_ppm";
String timePath = "/time";

//  Database Parent Path
String parentPath;

//  Initialize firebase JSON Object for update
FirebaseJson json;

//  Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//  Variable to store current epoch time
int timestamp;

 //  LCD Screen Setup
  int lcdColumns = 16;  //  Width
  int lcdRows = 2;  //  Height

  //  LCD Address, Width and Height Setup
  LiquidCrystal_I2C lcd(0x27,lcdColumns,lcdRows);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //  Initialize LCD
  lcd.init();
  //  Turn on LCD Backlight
  lcd.backlight();


  // Wait for Sensor to heat up 
  lcd.setCursor(0,0);
  lcd.print("Please wait 20s ");
  delay(20000);
  lcd.setCursor(0,1);
  lcd.print("Done!");
  delay(1000);
  lcd.clear();
  //  Start BlynK

  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);

  //  Turn on DHT
  dht.begin();

  //  Initialize the time client
  timeClient.begin();
  

  //  Connect to Wifi
  wifiConnect();

  //  Initializing Connection to Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  /* Sign up */
if (Firebase.signUp(&config, &auth, "", "")){
  Serial.println("ok");
  signupOK = true;
}
else{
  Serial.printf("%s\n", config.signer.signupError.message.c_str());
}

 /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  
  // Run Blynk
  Blynk.run();
  //  Air Quality Evaluation MSG
  String msg = "";

  //  Database path update
  if(databasePath != "AQM/" +getDate()){
    databasePath = "AQM/" + getDate();
  }
  // Read temperature value from DHT11
  float t = dht.readTemperature();

  // Read Humidity value from DHT11
  float h = dht.readHumidity();

  if(isnan(t) || isnan(h)){
    Serial.println("Failed to read from DHT sensor!");
  }

  // Read corrected PPM with temperature and humidity value from DHT11
  // for accurate readings of CO2 PPM value
  float ppm = gasSensor.getCorrectedPPM(t,h);

  // Preparing JSON object for Firebase Upload
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    timestamp = getTime();
    parentPath = databasePath + "/" + String(timestamp);

    // Create a JSON object to send to firebase 
    json.set(co2Path.c_str(),String(ppm));
    json.set(timePath.c_str(),String(timeClient.getFormattedTime()));
   
    // Write the json data to firebase, display error if fail
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }


  Serial.println("ppm: " + String(ppm));
  //  Displaying Corrected PPM level to LCD display 
  //  and give feedbacks on air quality
  lcd.setCursor(0,0);
  lcd.print("ppm: " + String(ppm));

  lcd.setCursor(0,1);
  if(ppm<=400){
      msg += "Excellent!";
      
  } else{
    if(ppm<=1000){
      msg += "Normal!";
    } else{
      
      msg += "Dangerous!!";
    }
  }
  lcd.print(msg);
  sendDataToBlynk(t,h,ppm,msg);
  delay(2000);
  lcd.clear();
}

//  Connection to WiFi
void wifiConnect(){
  WiFi.begin(ssid,pass);
  lcd.setCursor(0, 0);
  lcd.print("Conecting to ");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  lcd.print("...");

  int flag = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(++flag);
    Serial.print(' ');
  }

  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected!!");  
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP()); 
  delay(1500);
  lcd.clear();
}

//  Get Epoch Time
unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();
}

//  Get Formatted Date to store in Firebase
String getDate(){
  timeClient.update();
  unsigned long time = timeClient.getEpochTime();
  char date[32];
  sprintf(date,"%02d %02d %02d", day(time),month(time),year(time));
  return date;
}

//  Write to Blynk IoT cloud device
void sendDataToBlynk(float t, float h, float ppm,String msg){
  Blynk.virtualWrite(V0,ppm);
  Blynk.virtualWrite(V1,t);
  Blynk.virtualWrite(V2,h);
  Blynk.virtualWrite(V3,msg);
}