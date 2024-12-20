#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// Resolución de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Dirección I2C del SSD1306
#define OLED_ADDRESS 0x3C

// Pin del DS18B20
#define ONE_WIRE_BUS 4

// Configuración de objetos
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables de medición
float temperatura = 0.0;

void setup()
{
  Serial.begin(115200);

  // Inicializa la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    Serial.println("¡Fallo al inicializar la pantalla SSD1306!");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Inicializa el sensor DS18B20
  sensors.begin();

  // Configuración de Wi-Fi
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("Fallo en la configuración de IP estática");
    display.setCursor(0, 0);
    display.println("Error IP estatica");
    display.display();
    while (true)
      ;
  }
  WiFi.begin(ssid, password);

  display.setCursor(0, 0);
  display.println("Conectando WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }

  Serial.println("WiFi Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Conectado");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
}

void loop()
{
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

  delay(60000); // Espera un minuto antes de la próxima medición
}
