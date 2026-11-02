#ifndef GESTOR_MQTT_H
#define GESTOR_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Configuracion.h"

WiFiClient   espClient;
PubSubClient client(espClient);
bool         mqttConectadoPrevio = false;

inline bool mqttPublicarSeguro(const char* topic, const String& payload) {
  bool publicado = false;
  if (xSemaphoreTake(mutexMQTT, pdMS_TO_TICKS(TIMEOUT_MUTEX_MQTT_MS)) == pdTRUE) {
    if (client.connected()) {
      publicado = client.publish(topic, payload.c_str());
    }
    xSemaphoreGive(mutexMQTT);
  }
  return publicado;
}

inline void callbackRecepcionMQTT(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.println("\n[NODE-RED] Respuesta recibida: " + mensaje);

  StaticJsonDocument<CAPACIDAD_JSON_MQTT> doc;
  if (deserializeJson(doc, mensaje)) {
    Serial.println("[ERROR] Falla al parsear el JSON de Node-RED.");
    enviarEvento(TIPO_EVENTO_FIN_MENSAJE_LCD);
    return;
  }

  if (doc.containsKey("error")) {
    Serial.println("[ACCESO DENEGADO]: " + doc["error"].as<String>());
    enviarEvento(TIPO_EVENTO_TARJETA_DENEGADA);
  } else {
    String nombre = doc["nombre_usuario"].as<String>();
    Serial.println("[ACCESO AUTORIZADO]: Bienvenido/a " + nombre);
    enviarEvento(TIPO_EVENTO_TARJETA_VALIDA);
  }
}

inline void wifiInicializarConexion() {
  Serial.print("[WIFI] Conectando a ");
  Serial.print(SSID_WIFI);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(PAUSA_WIFI_CONEXION_MS));
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Conectado. IP: " + WiFi.localIP().toString());
}

inline void mqttInicializarCliente() {
  client.setServer(SERVIDOR_MQTT, PUERTO_MQTT);
  client.setCallback(callbackRecepcionMQTT);
  client.setSocketTimeout(TIMEOUT_SOCKET_MQTT_SEG);
}

inline void tareaMQTT(void *parameter) {
  unsigned long ultimoIntentoConexion = 0;

  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      if (xSemaphoreTake(mutexMQTT, pdMS_TO_TICKS(TIMEOUT_POLL_MUTEX_MQTT_MS)) == pdTRUE) {
        if (client.connected()) {
          client.loop();
        } else {
          if (mqttConectadoPrevio) {
            Serial.println("\n[MQTT] Desconectado. Reintentando...");
            mqttConectadoPrevio = false;
          }
          if (ultimoIntentoConexion == 0 || millis() - ultimoIntentoConexion > INTERVALO_REINTENTO_MQTT_MS) {
            ultimoIntentoConexion = millis();
            if (client.connect(ID_CLIENTE_MQTT)) {
              Serial.println("[MQTT] Conectado exitosamente a Node-RED.");
              client.subscribe(TOPIC_MQTT_RESPUESTA);
              mqttConectadoPrevio = true;
            } else {
              Serial.printf("[MQTT] Falla al conectar, rc=%d\n", client.state());
            }
          }
        }
        xSemaphoreGive(mutexMQTT);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(PAUSA_TAREA_MQTT_MS));
  }
}

#endif // GESTOR_MQTT_H