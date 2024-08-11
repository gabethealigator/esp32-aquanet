#include <WiFiManager.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#include <Firebase_ESP_Client.h>

#define API_KEY "APIKEY"
#define DATABASE_URL "DATABASEURL"

#define SETUP_PIN 0
#define LED 2

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool connected = false;
unsigned long previousMillis = 0;
unsigned long dataMillis = 0;
const long interval = 1000;
int count = 0;

class FireBaseManager {
private:
    bool signupOK = false;

public:
    bool checkAuthCredentials(String email, String password) {
        signupOK = Firebase.signUp(&config, &auth, email.c_str(), password.c_str());
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

struct Settings {
  char email[60];
  char password[20];
} settings;

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

    EEPROM.begin(512);
    EEPROM.get(0, settings);
    Serial.println("Settings loaded from EEPROM"); 

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

        settings.password[19] = '\0';
        settings.email[59] = '\0';

        WiFiManagerParameter email_parameter("email", "Email", settings.email, 60);
        WiFiManagerParameter password_parameter("password", "Password", settings.password, 20, "type='password'");

        wm.addParameter(&email_parameter);
        wm.addParameter(&password_parameter);

        wm.startConfigPortal("AquaNet");

        strcpy(settings.email, email_parameter.getValue());
        strcpy(settings.password, password_parameter.getValue());

        EEPROM.put(0, settings);
        if (EEPROM.commit())
        {
          Serial.println("Settings saved to EEPROM");
        }
        else
        {
          Serial.println("Error saving settings to EEPROM");
        }

    } 
    if (digitalRead(SETUP_PIN) == HIGH) {
        Serial.println("Starting the device");
        ConnectToWifi();

        config.api_key = API_KEY;
        config.database_url = DATABASE_URL;
        auth.user.email = settings.email;
        auth.user.password = settings.password;

        config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
        Firebase.begin(&config, &auth);
        Firebase.reconnectNetwork(true);

        fbdo.setBSSLBufferSize(4096, 1024);
        fbdo.setResponseSize(4096);
        
        bool checkAuthCredentials = firebaseManager.checkAuthCredentials(settings.email, settings.password);
        if (checkAuthCredentials) {
          Serial.println("Auth credentials are OK");
        }
        if (!checkAuthCredentials) {
          Serial.println("Auth credentials are not OK");
        }
    }

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    Serial.println("Email: " + String(settings.email));
    Serial.println("Password: " + String(settings.password));
}

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
    // TODO: Add the pairing code logic
    if (millis() - dataMillis > 5000 && Firebase.ready())
    {
        dataMillis = millis();
        String path = "/UsersData/4422/TEMP";
        Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, path, count++) ? "ok" : fbdo.errorReason().c_str());
    }
}