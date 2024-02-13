#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h> // Servo Library
#include <lcd_spi.h> //Import lcd_spi library
#include <NewPing.h> // Library for HC-SR04 ultrasonic sensor
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#define SERVO_PIN D0  // Define the pin connected to the servo

// Initialize Telegram BOT
#define BOTtoken "6638522798:AAEO52tXc96heSCt0Yjh1_Oxl9hNv_ZrNB8"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "692879409"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//LCD Message
bool messageDisplayed = false; // Flag to keep track of whether the message has been displayed or not
//LCD
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
//ultra
#define TRIGGER_PIN D1 // Define pin connected to the trigger pin of the ultrasonic sensor
#define ECHO_PIN D2    // Define pin connected to the echo pin of the ultrasonic sensor
#define MAX_DISTANCE 200 // Maximum distance in centimeters (adjust based on your sensor's specifications)
// ThingSpeak settings
char ssid[] = "SINGTEL-3386";       // Your WiFi SSID
char pass[] = "ootijithae";   // Your WiFi password
unsigned long channelID =  2428121;      // Your ThingSpeak channel ID
const char *apiKey = "JN50P0OUQ289IYYA"; // Your ThingSpeak API key

//WiFiClient client; // thingspeak 
Servo servo; // servo 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Initialize the NewPing object
lcd_spi lcd(D8); // Chip Selected Pin (CS), D8 for NodeMCU V3 ESP8266, 10 for any Arduino and Maker
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
String tag;
void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  //TELE
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  rfid.PCD_Init(); // Init MFRC522
  //RFID
  pinMode(D8, OUTPUT);
  //LCD
  lcd.begin(16,2); //LCD intialization, column and row setup setting  
  lcd.setCursor(0, 0); //cursor setting function start with column followed by row
  //Servo
  servo.write(0);
  delay(2000);
   // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  //Ini Tele
  bot.sendMessage(CHAT_ID, "Secure Entry Pro Activated", "");
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}
void loop() {
  // Read distance from ultrasonic sensor
  delay(500); // Delay to stabilize readings
  int distance = sonar.ping_cm();

  // Print distance to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Update ThingSpeak channel with the distance value
  ThingSpeak.writeField(channelID, 1, distance, apiKey);

  delay(1000); // ThingSpeak update interval (in milliseconds)
  
  if ( distance < 50 ) { 
    lcd.print("SCAN UR CARD"); //Print text
    digitalWrite(D8, HIGH);
    delay(3000);
    lcd.clear();
    delay(500);

    if ( ! rfid.PICC_IsNewCardPresent())
      return;
    if (rfid.PICC_ReadCardSerial()) {
      for (byte i = 0; i < 4; i++) {
        tag += rfid.uid.uidByte[i];
      }
      Serial.println(tag);
      if (tag == "2411086627") {
        Serial.println("Access Granted!");
        //LCD
        lcd.print("Open Succesful"); //Print text
        digitalWrite(D8, HIGH);
        delay(3000);
        // Servo
        servo.write(90);
        delay(1000);
        servo.write(0);
        delay(1000);
      } else {
      Serial.println("Access Denied!");
      //LCD
      lcd.print("Unsuccesful"); //Print texta
      digitalWrite(D8, HIGH);
      delay(3000);
      digitalWrite(D8, LOW);
      bot.sendMessage(CHAT_ID, "SOMEONE INTRUDING!", "");
      bot.sendMessage(CHAT_ID, "Press 'A' to capture intruder", "");
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    lcd.clear();
    delay(1000);
    }
  }
  
}