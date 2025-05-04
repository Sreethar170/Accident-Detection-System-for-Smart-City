#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN pins

const byte receiveAddress[6] = "00002";  // Address to receive data from
const byte sendAddress[6] = "00001";     // Address to send data to

char receivedData[32];  // Array to store received data
bool newData = false;   // Flag to check if new data is received

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  radio.begin();
  radio.openReadingPipe(1, receiveAddress);  // Open pipe for receiving
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();  // Start listening for data
  
  Serial.println("Receiver Ready...");
}

void loop() {
  // Check if data is available
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));  // Read the received data
    newData = true;
    Serial.print("Data Received: ");
    digitalWrite(5, HIGH);
    digitalWrite(6, LOW);
    Serial.println(receivedData);
  }
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  if (newData) {
    radio.stopListening();                 // Stop listening to transmit data
    radio.openWritingPipe(sendAddress);     // Open pipe to send data
    bool sent = radio.write(&receivedData, sizeof(receivedData));
    if (sent) {
      Serial.println("Data Sent Successfully!");
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
    } else {
      Serial.println("Failed to Send Data.");
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
    }
    radio.startListening();  // Switch back to listening mode
    newData = false;         // Reset the new data flag
  }
digitalWrite(5, HIGH);
  delay(1000);
}
