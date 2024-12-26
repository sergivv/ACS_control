#include <Wire.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// Configuración SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Pin del DS18B20
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Conexión del cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variables de medición
float temperatura = 0.0;

unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 300000; // Verificar wifi cada 5 min
int reintentos = 0;

void setupOLED()
{
  // Inicializa la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    Serial.println("Error al inicializar OLED");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void connectWiFi()
{
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("Fallo en la configuración de IP estática");
    display.setCursor(0, 0);
    display.println("Error IP estatica");
    display.display();
    while (true)
      ;
  }
  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  display.setCursor(0, 0);
  display.println("Conectando WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }

  display.clearDisplay();
  display.println("WiFi conectado!");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();

  // Generamos el topic a partir de la mac
  String macAddress = WiFi.macAddress();                     // Obtiene la MAC
  mqtt_topic = "ACS_Control/" + macAddress + "/Temperatura"; // Crea el topic
  Serial.print("Topic MQTT: ");
  Serial.println(mqtt_topic);
}

void checkWiFiConnection()
{
  if (millis() - lastWiFiCheck >= wifiCheckInterval)
  {
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi desconectado. Intentando reconectar...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }
}

void connectToMQTT()
{
  // Conexión al broker MQTT
  while (!client.connected())
  {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ESP32Client"))
    { // Identificador único para el cliente
      Serial.println("Conectado al broker MQTT!");
    }
    else
    {
      if (reintentos < 5)
      {
        Serial.print("Fallo, rc=");
        Serial.print(client.state());
        Serial.println(" Reintentando en 5 segundos...");
        reintentos++;
        delay(5000);
      }
      else
      {
        reintentos = 0;
        break;
      }
    }
  }
}

void publishTemperature()
{
  // Convertir temperatura a cadena
  char tempString[10];
  dtostrf(temperatura, 5, 1, tempString);

  // Publicar en el tema MQTT
  if (client.publish(mqtt_topic.c_str(), tempString))
  {
    Serial.print("Temperatura publicada: ");
    Serial.println(tempString);
  }
  else
  {
    Serial.println("Error al publicar temperatura");
  }
}

void setup()
{
  Serial.begin(115200);

  setupOLED();
  connectWiFi();
  sensors.begin();
  client.setServer(servidor_mqtt, puerto_mqtt);
}

void loop()
{
  // Verificar y reconectar Wi-Fi
  checkWiFiConnection();

  // Reconecta MQTT si es necesario
  if (!client.connected())
  {
    connectToMQTT();
  }
  client.loop();

  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0); // Obtiene la temperatura en grados Celsius
  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1); // Temperatura con un decimal
  Serial.println(" ºC");

  // Muestra la temperatura en la pantalla
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(0, 0);
  display.print(temperatura, 1);
  display.println("C");
  display.display();

  // Publicar en MQTT y desconecta el cliente.
  publishTemperature();
  client.disconnect();
  Serial.println("Cliente desconectado...");

  delay(60000); // Espera un minuto antes de la próxima medición
}
