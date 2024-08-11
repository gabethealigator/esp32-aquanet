#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "LEO"
#define WIFI_PASSWORD "mp15kl22"

#define API_KEY ""
#define DATABASE_URL ""

class FirebaseManager {
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;
  bool signupOK = false;
  unsigned long sendDataPreviousMillis = 0;

public:
  FirebaseManager() {
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
  }

  bool checkAuthCredentials() {
    signupOK = Firebase.signUp(&config, &auth, "", "");
    return signupOK;
  }

  void updateData(const String& path, int& data) {
    if (Firebase.ready() && signupOK && (millis() - sendDataPreviousMillis > 2000 || sendDataPreviousMillis == 0)) {
      sendDataPreviousMillis = millis();
      data++;
      bool result = Firebase.RTDB.setInt(&fbdo, path, data);

      if (result) {
        Serial.println("Data sent to Firebase");
      }
      if (!result) {
        Serial.println("Failed to send data to Firebase");
        Serial.println(fbdo.errorReason());
      }
    }
  }
};

FirebaseManager firebaseManager;
int TEMP_TEST = 0;
int PH_TEST = 0;
int HUMIDITY_TEST = 0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  bool checkAuthCredentials = firebaseManager.checkAuthCredentials();
  if (checkAuthCredentials) {
    Serial.println("Auth credentials are OK");
  }
  if (!checkAuthCredentials) {
    Serial.println("Auth credentials are not OK");
  }
}

void loop() {
  firebaseManager.updateData("DATA/TEMPERATURA", TEMP_TEST);
  firebaseManager.updateData("DATA/PH", PH_TEST);
  firebaseManager.updateData("DATA/UMIDADE", HUMIDITY_TEST);
}