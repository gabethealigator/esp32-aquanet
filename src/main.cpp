/* 
  TODOS: Add the multicolored leds indicating the status like
  reconecting to wifi, waiting wifi response, connected sucessfuly, sending data, waiting for the input
  to enter setup mode, etc. 12 echo 13 trigger
*/

#include "esp_system.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define API_KEY "AIzaSyBhj3If5etw9wk-QXnNnU0vvRxBKk2syFw"
#define DATABASE_URL "https://site-aqua-54d76-default-rtdb.firebaseio.com"

#define SETUP_PIN 0
#define LED 2
#define ONE_WIRE_BUS 4

#define TRIGGER_PIN 13
#define ECHO_PIN 12
#define SOUND_SPEED 0.034

FirebaseData fbdo;
FirebaseData fbdoStream;
FirebaseAuth auth;
FirebaseConfig config;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int count = 0;
bool connected = false;
unsigned long previousMillis = 0;
unsigned long lastFirebaseOperation = 0;
const unsigned long interval = 1000;
const unsigned long firebaseInterval = 5000;
long duration;
float distanceCm;

class FireBaseManager {
private:
  bool signupOK = false;

public:
  bool checkAuthCredentials(String email, String password) {
    signupOK = Firebase.signUp(&config, &auth, email.c_str(), password.c_str());
    return signupOK;
  }
};

void ConnectToWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin();
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      connected = true;
      Serial.println("Wi-Fi connected");
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi connection failed");
    }
  }
}

String GetDeviceUniqueId() {
  uint32_t chipId = ESP.getEfuseMac();
  return String(chipId, HEX);
}

struct Settings {
  char email[60];
  char password[20];
  char DeviceUniqueId[7];
} settings;

FireBaseManager firebaseManager;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  pinMode(LED, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Press the button to enter setup mode");
  for (int i = 3; i > 0; i--) {
    Serial.print(i);
    Serial.print("...");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  EEPROM.begin(512);
  EEPROM.get(0, settings);
  Serial.println("Settings loaded from EEPROM");

  if (digitalRead(SETUP_PIN) == LOW) {
    SetupMode:
    Serial.println("Entering setup mode");

    for (int i = 0; i < 2; i++) {
      digitalWrite(LED, HIGH);
      delay(300);
      digitalWrite(LED, LOW);
      delay(300);
    }

    WiFiManager wm;

    String DeviceUniqueId = GetDeviceUniqueId();
    DeviceUniqueId.toCharArray(settings.DeviceUniqueId,
                               sizeof(settings.DeviceUniqueId));

    settings.password[19] = '\0';
    settings.email[59] = '\0';

    WiFiManagerParameter email_parameter("email", "Email", settings.email, 60);
    WiFiManagerParameter password_parameter(
        "password", "Password", settings.password, 20, "type='password'");

    wm.addParameter(&email_parameter);
    wm.addParameter(&password_parameter);

    wm.startConfigPortal("AquaNet");

    strcpy(settings.email, email_parameter.getValue());
    strcpy(settings.password, password_parameter.getValue());

    EEPROM.put(0, settings);
    if (EEPROM.commit()) {
      Serial.println("Settings saved to EEPROM");
    }
    if (!EEPROM.commit()) {
      Serial.println("Error saving settings to EEPROM");
    }
  }

  Serial.println("Starting the device...");
  ConnectToWifi();

  if (WiFi.status() != WL_CONNECTED) {
    goto SetupMode;
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = settings.email;
  auth.user.password = settings.password;

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(4096);

  bool checkAuthCredentials =
      firebaseManager.checkAuthCredentials(settings.email, settings.password);
  if (checkAuthCredentials) {
    Serial.println("Auth credentials are OK");
  }
  if (!checkAuthCredentials) {
    Serial.println("Auth credentials are not OK");
  }

  Firebase.RTDB.beginStream(&fbdoStream,
                            "/UsersData/" + String(settings.DeviceUniqueId));

  sensors.begin();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  Serial.println("Email: " + String(settings.email));
  Serial.println("Password: " + String(settings.password));
  Serial.println("Pairing Code: " + String(settings.DeviceUniqueId));
}

void loop() {
  sensors.requestTemperatures();
  unsigned long currentMillis = millis();
  float temperatureC = sensors.getTempCByIndex(0);

  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distanceCm = duration * SOUND_SPEED / 2;

  if (WiFi.status() != WL_CONNECTED) {
    ConnectToWifi();
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(LED, !digitalRead(LED));
  }

  if (Firebase.ready() &&
      currentMillis - lastFirebaseOperation >= firebaseInterval) {
    lastFirebaseOperation = currentMillis;

    FirebaseJson json;
    json.set("TEMP", temperatureC);
    json.set("PH", count++);
    json.set("LEVEL", distanceCm);
    json.set("timestamp", currentMillis);

    String path = "/UsersData/";
    path += auth.token.uid.c_str();
    path += "/modules/";
    path += settings.DeviceUniqueId;
    path += "/statistics";
    if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
      Serial.println("Data sent successfully");
    }
    if (!Firebase.RTDB.setJSON(&fbdo, path, &json)) {
      Serial.printf("Error sending data: %s\n", fbdo.errorReason().c_str());
    }
  }

  if (Firebase.ready() && Firebase.RTDB.readStream(&fbdoStream)) {
    if (fbdoStream.streamAvailable()) {
      String path = fbdoStream.streamPath();
      String data = fbdoStream.stringData();
      Serial.printf("Stream data available at path: %s, data: %s\n",
                    path.c_str(), data.c_str());
    }
  }

  yield();
}