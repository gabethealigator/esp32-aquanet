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

FirebaseData fbdo;
FirebaseData fbdoStream;
FirebaseAuth auth;
FirebaseConfig config;

bool connected = false;
unsigned long previousMillis = 0;
unsigned long lastFirebaseOperation = 0;
const unsigned long interval = 1000;
const unsigned long firebaseInterval = 5000;
int count = 0;

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

struct Settings {
    char email[60];
    char password[20];
    char pairingCode[4];
} settings;

FireBaseManager firebaseManager;

void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    pinMode(LED, OUTPUT);

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
        Serial.println("Entering setup mode");

        for (int i = 0; i < 2; i++) {
            digitalWrite(LED, HIGH);
            delay(300);
            digitalWrite(LED, LOW);
            delay(300);
        }

        WiFiManager wm;

        settings.password[19] = '\0';
        settings.email[59] = '\0';

        WiFiManagerParameter email_parameter("email", "Email", settings.email, 60);
        WiFiManagerParameter password_parameter("password", "Password", settings.password, 20, "type='password'");
        WiFiManagerParameter pairingCode_parameter("pairingCode", "Pairing Code", settings.pairingCode, 4, "type='number'");

        wm.addParameter(&email_parameter);
        wm.addParameter(&password_parameter);
        wm.addParameter(&pairingCode_parameter);

        wm.startConfigPortal("AquaNet");

        strcpy(settings.email, email_parameter.getValue());
        strcpy(settings.password, password_parameter.getValue());
        strcpy(settings.pairingCode, pairingCode_parameter.getValue());

        EEPROM.put(0, settings);
        if (EEPROM.commit()) {
            Serial.println("Settings saved to EEPROM");
        } 
        if (!EEPROM.commit()) {
            Serial.println("Error saving settings to EEPROM");
        }
    } 

    Serial.println("Starting the device");
    ConnectToWifi();

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = settings.email;
    auth.user.password = settings.password;

    config.token_status_callback = tokenStatusCallback;
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

    Firebase.RTDB.beginStream(&fbdoStream, "/UsersData/" + String(settings.pairingCode));

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    Serial.println("Email: " + String(settings.email));
    Serial.println("Password: " + String(settings.password));
    Serial.println("Pairing Code: " + String(settings.pairingCode));
}

void loop() {
    unsigned long currentMillis = millis();

    if (WiFi.status() != WL_CONNECTED) {
        ConnectToWifi();
    }

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        digitalWrite(LED, !digitalRead(LED));
    }

    if (Firebase.ready() && currentMillis - lastFirebaseOperation >= firebaseInterval) {
        lastFirebaseOperation = currentMillis;

        FirebaseJson json;
        json.set("TEMP", count++);
        json.set("PH", count++);
        json.set("LEVEL", count++);
        json.set("timestamp", currentMillis);

        String path = "/UsersData/" + String(settings.pairingCode);
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
            Serial.printf("Stream data available at path: %s, data: %s\n", path.c_str(), data.c_str());
        }
    }

    yield();
}