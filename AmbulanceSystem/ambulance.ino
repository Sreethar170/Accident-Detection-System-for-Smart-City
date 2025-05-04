#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);  // CE, CSN pins for nRF24L01
const byte address1[6] = "00001"; // First transmitter address
const byte address2[6] = "00002"; // Second transmitter address

// WiFi credentials
const char* ssid = "SREETHAR";
const char* password = "SREETHAR,ECE";
bool ar=false;
ESP8266WebServer server(80);

struct VehicleData {
  String id;
  String name;
  String age;
  String bloodGroup;
  String phoneNumber;
  String emergencyNumber;
  String homeAddress;
  String vehicleNumber;
  String accidentLocation;
  String towerId;
  String cellId;
  bool accepted = false; // Track if data has been accepted
};

// Initialize stored data with multiple users
VehicleData userData[] = {
  {"ACC123", "Sreethar", "20", "O+", "9385462893", "8686867211", "BK Pudur, Kuniyamuthur, Coimbatore, TamilNadu", "TN-30-2005", "", "", ""},
  {"ACC125", "Murugan", "28", "A+", "2345678901", "1098765432", "456 Elm St, Coimbatore, TamilNadu", "TN-07-J15", "", "", ""},
  {"ACC124", "Saravanan", "20", "AB+", "4567890213", "9876512345", "123 Main St, Coimbatore, TamilNadu-641008", "TN-37-2545", "", "", ""}
};

#define PIN_D3 D3
#define PIN_D4 D4

VehicleData storedData[20]; // Increased size to support up to 20 different IDs
int dataCount = 0;          // Tracks how many different IDs have been received
bool gps = false;

// History array to store accepted data
struct HistoryEntry {
  String id;
  String name;
  String vehicleNumber;
  String contactNumber;
  String towerId;
  String cellId;
};

HistoryEntry history[20];  // Store up to 20 history entries
int historyCount = 0;      // Tracks how many entries are in history

bool newDataReceived = false;  // Flag to track if new data is received

void setup() {
  Serial.begin(115200);

  pinMode(PIN_D3, OUTPUT);
  pinMode(PIN_D4, OUTPUT);
  digitalWrite(PIN_D3, HIGH);
  digitalWrite(PIN_D4, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/accept", handleAccept); // Handle accept with ID
  server.on("/data", handleData); // New route for checking new data status
  server.begin();
  Serial.println("Web server started");

  radio.begin();
  radio.openReadingPipe(1, address1); // Open first reading pipe
  radio.openReadingPipe(2, address2); // Open second reading pipe
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  server.handleClient();

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
    if (receivedString.startsWith("200,")) {  
      String receivedID = receivedString.substring(4);  
      Serial.println("Acknowledgment Received for ID: " + receivedID);
      ar=true;
      for (int i = 0; i < dataCount; i++) {
        if (storedData[i].id == receivedID) {
          storedData[i].accepted = true;
          storedData[i].id = "null";  
          Serial.println("Data Accepted: " + receivedID);
        }
      }
    }
    else if (firstComma != -1 && secondComma != -1 && thirdComma != -1 && fourthComma != -1) {
      String receivedID = receivedString.substring(0, firstComma);
      String towerId = receivedString.substring(firstComma + 1, secondComma);
      String cellId = receivedString.substring(secondComma + 1, thirdComma);
      String latitude = receivedString.substring(thirdComma + 1, fourthComma);
      String longitude = receivedString.substring(fourthComma + 1);

      
      if(longitude == "0.000000" && latitude == "0.000000"){
        gps=false;
      }else{
        gps= true;
      }

      Serial.print("ID: ");
      Serial.println(receivedID);
      Serial.print("Tower ID: ");
      Serial.println(towerId);
      Serial.print("Cell ID: ");
      Serial.println(cellId);
      Serial.print("Latitude: ");
      Serial.println(latitude);
      Serial.print("Longitude: ");
      Serial.println(longitude);
      // Check received ID against the user database and add to storedData array
      for (int i = 0; i < sizeof(userData) / sizeof(userData[0]); i++) {
        if ((receivedID == userData[i].id) ) {
          bool idAlreadyStored = false;
          // Check if ID is already in storedData
          for (int j = 0; j < dataCount; j++) {
            if (storedData[j].id == receivedID) {
              idAlreadyStored = true;
              break;
            }
          }

          // If ID is new, store it
          if (!idAlreadyStored && dataCount < 20 && gps ) {
            storedData[dataCount] = userData[i];

            if (latitude == "0.000000" && longitude == "0.000000") {
              storedData[dataCount].towerId = receivedID;
              storedData[dataCount].towerId = towerId;
            storedData[dataCount].cellId = cellId;
              storedData[dataCount].accidentLocation = "";
            } else {
              storedData[dataCount].accidentLocation = "https://www.google.com/maps?q=" + latitude + "," + longitude;
              storedData[dataCount].towerId = "";
            }

            
            if (historyCount < 20) {
              bool add=true;
              for(int i=0;i<historyCount;i++)
              {
                if(receivedID==history[i].id)
                {
                  add=false;
                }
              }
              if(add==true)
              {
                
              history[historyCount].id = receivedID;
              history[historyCount].name = userData[i].name;
              history[historyCount].vehicleNumber = userData[i].vehicleNumber;
              history[historyCount].contactNumber = userData[i].phoneNumber;
              history[historyCount].towerId = towerId;
              history[historyCount].cellId = cellId;
              
              historyCount++;

              }
              
            }

            dataCount++; // Increase the count of stored data
            Serial.println("New data stored.");
            newDataReceived = true; // Set flag to indicate new data received

          }

          digitalWrite(PIN_D4, HIGH);
          digitalWrite(PIN_D3, LOW);
          delay(1000);
          digitalWrite(PIN_D4, LOW);
          digitalWrite(PIN_D3, HIGH);
          break;
        }
      }
    }
  } else {
    digitalWrite(PIN_D3, HIGH);
    digitalWrite(PIN_D4, LOW);
  }
}

// Handles the main page rendering
void handleRoot() {
  String page = "<!DOCTYPE html><html>";
  page += "<head><title>Accident Monitoring System</title>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<script>";
  page += "setInterval(function() { fetch('/data').then(response => response.json()).then(data => {";
  page += "if (data.newData) { location.reload(); }";
  page += "}); }, 3000);";  // Check for new data every 5 seconds
  page += "</script>";
  page += "<style>";
  page += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4; }";
  page += "h1 { text-align: center; color: #333; }";
  page += ".container { background-color: #fff; padding: 20px; border-radius: 5px; max-width: 600px; margin: auto; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  page += ".data-item { margin: 10px 0; }";
  page += ".data-label { font-weight: bold; color: #333; }";
  page += ".data-value { color: #555; }";
  page += ".no-data { color: red; }";
  page += "button { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; border-radius: 5px; cursor: pointer; width: 100%; }";
  page += "button:hover { background-color: #45a049; }";
  page += "</style></head><body>";
  page += "<div class='container'><h1>Ambulance </h1>";

  // Generate data sections for each stored data
  // Generate data sections for each stored data
  for (int i = 0; i < dataCount; i++) {
    if (!storedData[i].accepted) {  // Only display data if not yet accepted
      page += "<div id='dataSection_" + String(i) + "'>";
      page += "<div class='data-item'><span class='data-label'>Name:</span> <span class='data-value'>" + storedData[i].name + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Age:</span> <span class='data-value'>" + storedData[i].age + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Blood Group:</span> <span class='data-value'>" + storedData[i].bloodGroup + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Phone Number:</span> <span class='data-value'>" + storedData[i].phoneNumber + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Emergency Number:</span> <span class='data-value'>" + storedData[i].emergencyNumber + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Vehicle Number:</span> <span class='data-value'>" + storedData[i].vehicleNumber + "</span></div>";
      page += "<div class='data-item'><span class='data-label'>Home Address:</span> <span class='data-value'>" + storedData[i].homeAddress + "</span></div>"; // Display Home Address

      if (storedData[i].accidentLocation != "") {
        page += "<div class='data-item'><span class='data-label'>Accident Location:</span> <a href='" + storedData[i].accidentLocation + "' target='_blank'>View Location</a></div>";
      } else {
        page += "<div class='data-item'><span class='data-label'>Accident detected tower ID:</span> <span class='data-value'>" + storedData[i].towerId + "</span></div>";
        page += "<div class='data-item'><span class='data-label'>Accident detected cell ID:</span> <span class='data-value'>"+storedData[i].cellId+"</span></div>";
      }

      page += "<button onclick=\"acceptData('" + storedData[i].id + "')\">Accept</button>"; // Added accept button
      page += "</div><hr>";
    }
  }

  // Add History button
  page += "<button onclick='showHistory()'>Show History</button>";
  page += "<div id='historyPopup' style='display:none; position:fixed; left:0; top:0; width:100%; height:100%; background-color:rgba(0,0,0,0.8); color:white;'>";
  page += "<div style='position:relative; top:50%; left:50%; transform:translate(-50%, -50%);transition: all 0.5s linear; background-color:#333; padding:40px; border-radius:10px;'>";
  page += "<h2>History</h2><div id='historyContent'></div><button onclick='document.getElementById(\"historyPopup\").style.display=\"none\"'>Close</button>";
  page += "</div></div>";

  page += "<script>";
 

  page += "function acceptData(id) { fetch('/accept?id=' + id).then(response => location.reload()); }"; // Refresh page after accepting
  page += "function showHistory() {";
  page += "let historyContent = '<ol>';";
  for (int i = 0; i < historyCount; i++) {
    
    page += "historyContent += '<li>ID: " + history[i].id + ",<br> Name: " + history[i].name + ",<br> Vehicle: " + history[i].vehicleNumber + ", <br> Contact: " + history[i].contactNumber + ", <br> Tower Id: " + history[i].towerId + ", <br> Cell Id: " + history[i].cellId + "';";
  }
  page += "historyContent += '</ol>';";
  page += "document.getElementById('historyContent').innerHTML = historyContent;";
  page += "document.getElementById('historyPopup').style.display = 'block';";
  page += "}";
  page += "</script>";
  page += "</div></body><footer><p style='position: fixed;bottom: 2rem;width: 100%;text-align: center;'>Developed By Sreethar</p> <p style='position: fixed;bottom: 0rem; width: 100%;text-align: center;;'>CopyRight@2025</p></footer</html>";

  server.send(200, "text/html", page);

}

// Handles the accept button click
void handleAccept() {
  String id = server.arg("id");
  for (int i = 0; i < dataCount; i++) {
    if (storedData[i].id == id) {
      storedData[i].accepted = true;
      Serial.println("Data accepted for ID: " + id);
      storedData[i].id="null";
    }
  }
  radio.stopListening();  // Switch to transmitter mode
      radio.openWritingPipe(address1);  // Send back to 00001
      
      String message = "200," + id;
      char sendData[32];
      message.toCharArray(sendData, sizeof(sendData));

      bool success = radio.write(&sendData, sizeof(sendData));  // Transmit the message
      if (success) {
        Serial.println("Acknowledgment sent successfully: " + message);
      } else {
        Serial.println("Failed to send acknowledgment");
      }

      radio.startListening();  // Switch back to receiver mode
  server.sendHeader("Location", "/");
  server.send(303);
}

// Handles checking if new data is received
void handleData() {
  if(newDataReceived || ar)
  {
    String jsonResponse = "{ \"newData\": " + String(newDataReceived || ar ? "true" : "false") + " }";
    server.send(200, "application/json", jsonResponse);
  }
  // Reset the new data flag after responding
  newDataReceived = false;
  gps = false;
  ar=false;
}

