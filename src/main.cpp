#include <WiFiManager.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#include <Firebase_ESP_Client.h>

#define API_KEY ""
#define DATABASE_URL ""
#define USER_EMAIL ""
#define USER_PASSWORD ""

#define SETUP_PIN 0
#define LED 2

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool connected = false;
unsigned long previousMillis = 0;
const long interval = 1000;

class FireBaseManager {
private:
    unsigned long sendDataPreviousMillis = 0;
    bool signupOK = false;

public:
    bool checkAuthCredentials() {
        signupOK = Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD);
        return true;
    }
};

void ConnectToWifi() {
    WiFi.begin();
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        connected = true;
    }

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        connected = false;
    }
}

FireBaseManager firebaseManager;

void setup() {
    // Setup serial communication and wifi mode to station mode (STA)   
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    pinMode(LED, OUTPUT);

    // Time to enter setup mode
    Serial.println("Press the button to enter setup mode");
    for (int i = 3; i > 0; i--) {
        Serial.print(i);
        Serial.print("...");
        digitalWrite(LED, HIGH);
        delay(500);
        digitalWrite(LED, LOW);
        delay(500);
    }

    // Check if the setup button is pressed
    if (digitalRead(SETUP_PIN) == LOW) {
        Serial.println("Entering setup mode");

        for (int i = 0; i < 2; i++) {
            digitalWrite(LED, HIGH);
            delay(300);
            digitalWrite(LED, LOW);
            delay(300);
        }

        // Start the wifi manager to configure the wifi
        WiFiManager wm;

        wm.startConfigPortal("AquaNet");
    } 
    if (digitalRead(SETUP_PIN) == HIGH) {
        Serial.println("Starting the device");
        ConnectToWifi();

        config.api_key = API_KEY;
        config.database_url = DATABASE_URL;
        auth.user.email = USER_EMAIL;
        auth.user.password = USER_PASSWORD;

        config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
        Firebase.begin(&config, &auth);
        Firebase.reconnectNetwork(true);

        fbdo.setBSSLBufferSize(4096, 1024);
        fbdo.setResponseSize(4096);
        
        bool checkAuthCredentials = firebaseManager.checkAuthCredentials();
        if (checkAuthCredentials) {
          Serial.println("Auth credentials are OK");
        }
        if (!checkAuthCredentials) {
          Serial.println("Auth credentials are not OK");
        }
    }

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
}

int TEMP_TEST = 0;

void loop() {
    ConnectToWifi();
    if (connected) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            digitalWrite(LED, HIGH);
            delay(100);
            digitalWrite(LED, LOW);
        }
    }
}