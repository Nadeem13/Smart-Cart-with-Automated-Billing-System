#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 5
#define RST_PIN 22

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ssm8050"
#define WIFI_PASSWORD "aaqib123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyB-fObEQ5kv6G-kFOOkNYEn-LX5pAkgrHo"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "smart-shopping-5a6f8-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;



MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];
bool signupOK = false;

void setup() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Smart");
  lcd.setCursor(0, 1);
  lcd.print("Shopping Cart");
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  int num = nuidPICC[0] + nuidPICC[1] + nuidPICC[2] + nuidPICC[3];
  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));

    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    if (num == 180) {
      if (Firebase.ready() && signupOK ) {
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item1/Price", 10);
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item1/Qty", 1);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 221) {
      if (Firebase.ready() && signupOK ) {
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item2/Price", 20);
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item2/Qty", 1);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 770) {
      if (Firebase.ready() && signupOK ) {
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item3/Price", 30);
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item3/Qty", 1);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 184) {
      if (Firebase.ready() && signupOK ) {
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item4/Price", 40);
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item4/Qty", 1);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
  }
  else {
    Serial.println(F("Card read previously."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    if (num == 180) {
      if (Firebase.ready() && signupOK ) {
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item1/Qty", 0);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 221) {
      if (Firebase.ready() && signupOK ) {       
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item2/Qty", 0);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 770) {
      if (Firebase.ready() && signupOK ) {        
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item3/Qty", 0);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
    if (num == 184) {
      if (Firebase.ready() && signupOK ) {        
        Firebase.RTDB.setInt(&fbdo, "Cart1/Item4/Qty", 0);
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
