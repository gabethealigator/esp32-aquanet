#include <WiFiManager.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#include <Firebase_ESP_Client.h>

#define API_KEY "AIzaSyBhj3If5etw9wk-QXnNnU0vvRxBKk2syFw"
#define DATABASE_URL "https://site-aqua-54d76-default-rtdb.firebaseio.com"

#define SETUP_PIN 0
#define LED 2

bool connected = false;
unsigned long previousMillis = 0;
const long interval = 1000;

class FireBaseManager {
private:
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    bool signupOK = false;
    unsigned long sendDataPreviousMillis = 0;

public:
    FireBaseManager() {
        config.api_key = API_KEY;
        config.database_url = DATABASE_URL;
        Firebase.begin(&config, &auth);
    }

    void authenticateUser(const String& email, const String& password) {
        auth.user.email = email;
        auth.user.password = password;
        signupOK = Firebase.signUp(&config, &auth, email, password);

        if (signupOK) {
            Serial.println("User authenticated successfully");
        } else {
            Serial.println("Failed to authenticate user");
            Serial.println(fbdo.errorReason());
        }
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
        String email = "";
        String password = "";
        firebaseManager.authenticateUser(email, password);
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

    firebaseManager.updateData("UserData/4422/TEMP", TEMP_TEST);
}