#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Setup for nRF24L01 radio module
RF24 radio(9, 10);  
const byte address1[6] = "00002";  // Primary address
const byte address2[6] = "00001";  // Secondary address
byte currentAddress[6] = "00002";  // Start with address 00002

// Define the ID to send
char dataToSend[32];  // Buffer to hold the ID, Tower ID, and GPS data
LiquidCrystal_I2C lcd(0x3F, 16, 2); 
// GPS Setup
SoftwareSerial gpsSerial(6, 7);  // RX to pin 6, TX to pin 7
TinyGPSPlus gps;
float latitude = 0.0, longitude = 0.0;

// Initial values
int timeLeft = 60;  // Time in seconds (2 min)
float distance = 2.0;  // Distance in km
float decrement = 2.0 / 60.0;  // Distance decrement per second
String message = "Medical assistance is arriving   "; 
// Pin Definitions
const int buttonPin = 8;  
const int D5Pin = 5;      
const int D8Pin = 8;      
const int D2Pin = 2;      
bool lastButtonState = HIGH; 
int LED = A0;
float b=A1;

// Function to check the channel availability
bool checkChannel() {
  radio.openWritingPipe(currentAddress);
  // radio.openWritingPipe(address2);
  radio.openReadingPipe(1, address2);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  char testMessage[] = "Test";  
  bool result = radio.write(&testMessage, sizeof(testMessage));  
  if (result) {
    Serial.println("Channel is clear. Ready to send data.");
  } else {
    Serial.println("Channel is busy or unavailable.");
  }
  return result;
}

// Function to check both addresses before sending data
bool checkAddresses() {
  // Check address 00002 first
  memcpy(currentAddress, address1, sizeof(address1));
  if (checkChannel()) {
    Serial.println("Address 00002 is available.");
    return true;
  }
  
  // If 00002 is unavailable, check 00001
  memcpy(currentAddress, address2, sizeof(address2));
  if (checkChannel()) {
    Serial.println("Address 00001 is available.");
    return true;
  }
  
  Serial.println("Both addresses are busy. Data not sent.");
  return false;
}

// Function to send data after address check
void sendData() {
  if (checkAddresses()) {  // Check addresses before sending data
    bool result = radio.write(&dataToSend, sizeof(dataToSend));
    if (result) {
      // lcd.clear();
            lcd.setCursor(2, 1);
      lcd.print("Message Send");
      Serial.println("Data sent successfully on address: " + String((char*)currentAddress));
      digitalWrite(3, HIGH);  
      digitalWrite(4, LOW);   
      delay(500);  
      digitalWrite(3, LOW);  
      digitalWrite(4, HIGH); 
    } else {
      Serial.println("Failed to send data.");

      digitalWrite(3, HIGH);  
      digitalWrite(4, LOW);   
    }
  }
}

// Function to read GPS data
void readGPSData() {
  while (gpsSerial.available()) {
    int data = gpsSerial.read();
    if (gps.encode(data)) {
      if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);
      } else {
        Serial.println("Waiting for GPS signal...");
      }
    }
  }
}

void checkD2Condition() {
  int d2LowCount = 0;
  unsigned long startTime = millis();

  Serial.println("D2 LOW detected, waiting 3 seconds...");
  
  while (millis() - startTime < 3000) {
    digitalWrite(3, !digitalRead(3));
    digitalWrite(4, !digitalRead(4));
    delay(100);

    if (digitalRead(D2Pin) == LOW) {
      d2LowCount++;
      Serial.print("D2 LOW count: ");
      Serial.println(d2LowCount);
    }
    delay(100);
    if (d2LowCount >= 3) {
      lcd.clear();
      lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("SOS Alert");
      Serial.println("D2 went LOW twice, sending data...");
      prepareAndSendData();
      return;
    }
  }
  Serial.println("D2 did not go LOW twice, no data sent.");
}

void prepareAndSendData() {
  String fullData;
  int randomValue1 = random(10, 30); 
  int randomValue2 = random(10, 30); 
  
  if (latitude != 0.0 && longitude != 0.0) {
    fullData = "ACC124," + String(randomValue1) + "," + String(randomValue2) + "," + String(latitude, 6) + "," + String(longitude, 6);  
  } else {
    fullData = "ACC124," + String(randomValue1) + "," + String(randomValue2) + ",0.000000,0.000000";  
  }
  fullData.toCharArray(dataToSend, sizeof(dataToSend));  
  for (int i = 0; i < 3; i++) {  
    sendData();  
    delay(1000);  
  }
}

void setup() {
  Serial.begin(9600);  
  gpsSerial.begin(9600);  
    lcd.init();
    
  radio.begin();  
  radio.openWritingPipe(currentAddress);
  radio.setPALevel(RF24_PA_MIN);  
  radio.stopListening();  

  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(D8Pin, INPUT_PULLUP);      
  pinMode(D5Pin, INPUT_PULLUP);      
  pinMode(D2Pin, INPUT_PULLUP);      
  pinMode(3, OUTPUT);  
  pinMode(4, OUTPUT);
   pinMode(LED, OUTPUT);
   pinMode(b,OUTPUT);
   analogWrite(b, 75);
  digitalWrite(3, HIGH);  
  digitalWrite(4, LOW);
  digitalWrite(LED, LOW);
  randomSeed(analogRead(0));  

}

void loop() {
  lcd.noBacklight();
  lcd.clear();
  readGPSData();

  if (digitalRead(D2Pin) == LOW) {
    checkD2Condition();
  }
  if (digitalRead(D8Pin) == LOW) {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Accident Detected");
    for (int i = 0; i < 20; i++) {
      digitalWrite(4, LOW);
      digitalWrite(3, !digitalRead(3));  
      delay(500);
      
    }

    unsigned long startTime = millis();
    bool sendDataFlag = true;
    while (millis() - startTime < 5000) {
      digitalWrite(3, LOW);
      digitalWrite(4, !digitalRead(4));
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(1, 0);
      lcd.print("If you Ok,");
      lcd.setCursor(0, 1);
      lcd.print("Press the Button");
      delay(500);
      if (digitalRead(D5Pin) == LOW) {
        sendDataFlag = false;
       
        break;
      }
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Accident ");
    }
    if (sendDataFlag) {
      prepareAndSendData();
    } else {
      Serial.println("D5 was low during the 5-second window, not sending data.");
       lcd.clear();
       lcd.backlight();
        lcd.setCursor(3, 0);
        lcd.print("Thank u God");
        lcd.setCursor(4, 1);
        lcd.print("I'm Safe");
        delay(4000);
    }
    return;
  }
  radio.startListening();
  if (radio.available()) {
    char receivedData[32] = "";
    
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Received data: ");
    Serial.println(receivedData);

    String receivedString = String(receivedData);
    int firstComma = receivedString.indexOf(',');
    int secondComma = receivedString.indexOf(',', firstComma + 1);
    int thirdComma = receivedString.indexOf(',', secondComma + 1);
    int fourthComma = receivedString.indexOf(',', thirdComma + 1);
    if (receivedString.startsWith("200,ACC124")) {  
      digitalWrite(LED, HIGH);
      lcd.clear();
      String receivedID = receivedString.substring(4);  
      Serial.println("Acknowledgment Received for ID: " + receivedID);
      Serial.println("Data Accepted: " + receivedID);
      lcd.backlight();
    while (timeLeft >= 0) {  
    for (int i = 0; i <= message.length() - 16; i++) {  // Scroll message
        lcd.clear();
        digitalWrite(LED, LOW);

        // Display scrolling message
        lcd.setCursor(0, 0);
        lcd.print(message.substring(i, i + 16));  // Show 16-character section

        // Display time and distance in the second row
        lcd.setCursor(0, 1);
        lcd.print("T: ");
        lcd.print(timeLeft);
        lcd.print("s ");

        lcd.setCursor(7, 1);
        lcd.print("D: ");
        lcd.print(distance, 1);
        lcd.print("km");

        delay(300);  // Delay for scrolling effect
        digitalWrite(LED, HIGH);

        // Reduce values
        
            timeLeft--;  
            distance -= decrement;
            i = -1;  // Restart scrolling from the beginning
        

        if (timeLeft < 0) break;  // Exit if time runs out
    }
}

// When time reaches 0, show "Arrived!"
lcd.clear();
lcd.setCursor(4, 0);
lcd.print("Arrived!");
while (1);  // Stop further updates
        }
      }
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  lastButtonState = digitalRead(buttonPin);
  digitalWrite(LED, LOW);
}
