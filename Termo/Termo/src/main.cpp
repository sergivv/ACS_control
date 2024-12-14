#include <WiFi.h>
#include "config.h"

void setup()
{
  Serial.begin(115200);

  // Configura la IP estática de la conexión
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("Fallo en la configuración de IP estática");
  }
  else
  {
    Serial.println("Configuración de IP estática exitosa");
  }

  // Inicia la conexión
  WiFi.begin(ssid, password);

  // Espera la conexión Wi-Fi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }

  // Muestra la IP configurada y la MAC del dispositivo
  Serial.println("Conexión exitosa!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Dirección MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop()
{
}
