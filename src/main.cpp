#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <EEPROM.h>
#include <addons/RTDBHelper.h>
#include <Firebase_ESP_Client.h>

// Define o pino de SETUP e o LED
#define SETUP_PIN 0
#define LED 2

// Define credenciais e objetos do firebase
const char *DATABASE_URL = getenv("DATABASE_URL");
const char *API_KEY = getenv("FIREBASE_API_KEY");

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

class IPAddressParameter : public WiFiManagerParameter
{
public:
  IPAddressParameter(const char *id, const char *placeholder, IPAddress address)
      : WiFiManagerParameter("")
  {
    init(id, placeholder, address.toString().c_str(), 16, "", WFM_LABEL_BEFORE);
  }

  bool getValue(IPAddress &ip)
  {
    return ip.fromString(WiFiManagerParameter::getValue());
  }
};

class IntParameter : public WiFiManagerParameter
{
public:
  IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10)
      : WiFiManagerParameter("")
  {
    init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
  }

  long getValue()
  {
    return String(WiFiManagerParameter::getValue()).toInt();
  }
};

class FloatParameter : public WiFiManagerParameter
{
public:
  FloatParameter(const char *id, const char *placeholder, float value, const uint8_t length = 10)
      : WiFiManagerParameter("")
  {
    init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
  }

  float getValue()
  {
    return String(WiFiManagerParameter::getValue()).toFloat();
  }
};

struct Settings
{
  int i;
  char password[20];
  char email[80];
} sett;

bool connected = false;
unsigned long previousMillis = 0;
const long interval = 4000; // Intervalo de 4 segundos

void setup()
{
  WiFi.mode(WIFI_STA); // explicitamente define o modo, esp defaults to STA+AP
  pinMode(SETUP_PIN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);

  // Delay para pressionar o botão SETUP
  Serial.println("Pressione o botão de setup");
  for (int sec = 3; sec > 0; sec--)
  {
    Serial.print(sec);
    Serial.print("..");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  // Aviso: para exemplo apenas, isso inicializará a memória vazia em suas variáveis
  // sempre inicialize a memória flash ou adicione alguns bits de checksum
  EEPROM.begin(512);
  EEPROM.get(0, sett);
  Serial.println("Configurações carregadas");

  if (digitalRead(SETUP_PIN) == LOW)
  {
    // Botão pressionado
    Serial.println("SETUP");

    // Piscar o LED duas vezes rapidamente
    for (int i = 0; i < 2; i++)
    {
      digitalWrite(LED, HIGH);
      delay(300);
      digitalWrite(LED, LOW);
      delay(300);
    }

    WiFiManager wm;

    IntParameter codigo_pareamento("int", "Código de Pareamento", sett.i);
    wm.addParameter(&codigo_pareamento);

    // Adicionar parâmetros de senha e email
    WiFiManagerParameter email_param("email", "Email", sett.email, 40);
    WiFiManagerParameter senha_param("senha", "Senha", sett.password, 40, "type='password'");
    wm.addParameter(&email_param);
    wm.addParameter(&senha_param);

    // Parâmetros de SSID e senha já incluídos
    wm.startConfigPortal();

    // Adiciona os valores dos parâmetros
    sett.i = codigo_pareamento.getValue();
    strncpy(sett.email, email_param.getValue(), sizeof(sett.email) - 1);
    strncpy(sett.password, senha_param.getValue(), sizeof(sett.password) - 1);

    Serial.print("Código de pareamento: ");
    Serial.println(sett.i, DEC);
    Serial.print("Email: ");
    Serial.println(sett.email);
    Serial.print("Senha: ");
    Serial.println(sett.password);

    EEPROM.put(0, sett);
    if (EEPROM.commit())
    {
      Serial.println("Configurações salvas");
    }
    else
    {
      Serial.println("Erro no EEPROM");
    }

    // Após a configuração, tentar conectar ao WiFi
    WiFi.begin();
    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
      connected = true;
    }
  }
  else
  {
    Serial.println("Setup is ok, connecting to wifi SSID");

    // Conectar ao SSID salvo
    WiFi.begin();
    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
      connected = true;
    }
  }
}

void loop()
{
  WiFi.begin();
  // Verifica o estado da conexão WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    connected = false;
  }
  // Pisca o LED se conectado
  if (connected)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
  }

  Serial.println(sett.i, DEC);
  Serial.println(sett.email);
  Serial.println(sett.password);
}