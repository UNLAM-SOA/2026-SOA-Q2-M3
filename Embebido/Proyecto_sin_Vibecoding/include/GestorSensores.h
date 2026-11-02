#ifndef GESTOR_SENSORES_H
#define GESTOR_SENSORES_H

#include <SPI.h>
#include <MFRC522.h>
#include <DHTesp.h>
#include "Configuracion.h"
#include "GestorMQTT.h"

MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
DHTesp  dht;

inline void sensoresInicializarHardware() {
  SPI.begin();
  rfid.PCD_Init();
  vTaskDelay(pdMS_TO_TICKS(PAUSA_INIT_RFID_MS));

  byte versionRFID = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.printf("[RFID] VersionReg = 0x%02X\n", versionRFID);
  if (versionRFID == REGISTRO_RFID_VACIO_00 || versionRFID == REGISTRO_RFID_VACIO_FF) {
    Serial.println("[RFID] SIN COMUNICACION con el RC522. Revisar cableado y 3.3V.");
  } else {
    Serial.println("[RFID] RC522 detectado correctamente.");
  }

  dht.setup(PIN_DHT, DHTesp::DHT11);
  Serial.println("[DHT11] Sensor inicializado en GPIO " + String(PIN_DHT));
}

inline String extraerHashTarjetaRFID() {
  String hashTarjeta = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < BYTE_HEX_LIMITE_CERO) {
      hashTarjeta += "0";
    }
    hashTarjeta += String(rfid.uid.uidByte[i], HEX);
  }
  return hashTarjeta;
}

inline void muestrearLectorRFID(unsigned long &ultimoEscaneo) {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (ultimoEscaneo == 0 || millis() - ultimoEscaneo > TIEMPO_ANTIRREBOTE_RFID_MS) {
      String hashTarjeta = extraerHashTarjetaRFID();

      Serial.println("\n[RFID] Tarjeta detectada en lector fisico: " + hashTarjeta);
      Serial.println("[MQTT] Publicando consulta a Node-RED...");

      String payload = "{\"hash\": \"" + hashTarjeta + "\"}";
      if (mqttPublicarSeguro(TOPIC_MQTT_CONSULTA, payload)) {
        enviarEvento(TIPO_EVENTO_TARJETA_LEYENDO);
      } else {
        Serial.println("[MQTT] No se pudo publicar (sin conexion al broker).");
        enviarEvento(TIPO_EVENTO_SIN_SERVIDOR);
      }
      ultimoEscaneo = millis();
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

inline void muestrearSensorPuerta(int &estadoPuertaAnterior) {
  int estadoPuertaActual = digitalRead(PIN_PUERTA);
  if (estadoPuertaActual != estadoPuertaAnterior) {
    int tipoEvento = (estadoPuertaActual == HIGH) ? TIPO_EVENTO_PUERTA_ABIERTA : TIPO_EVENTO_PUERTA_CERRADA;
    enviarEvento(tipoEvento);
    estadoPuertaAnterior = estadoPuertaActual;
  }
}

inline void tareaGetEvent(void *parameter) {
  int estadoPuertaAnterior = ESTADO_PUERTA_INDEFINIDO;
  unsigned long ultimoEscaneoRFID = 0;

  while (true) {
    muestrearSensorPuerta(estadoPuertaAnterior);
    muestrearLectorRFID(ultimoEscaneoRFID);
    vTaskDelay(pdMS_TO_TICKS(CADENCIA_SENSORES_MS));
  }
}

inline void tareaMonitoreoClimatico(void *parameter) {
  while (true) {
    TempAndHumidity datosClima = dht.getTempAndHumidity();

    if (dht.getStatus() != DHTesp::ERROR_NONE) {
      Serial.println("[DHT11] Error de lectura: " + String(dht.getStatusString()));
      enviarEvento(TIPO_EVENTO_TEMP_ERROR);
    } else {
      float temp = datosClima.temperature;
      float hum  = datosClima.humidity;

      int eventoTermico = (temp > UMBRAL_TEMP_MAXIMA_C) ? TIPO_EVENTO_TEMP_ALTA : TIPO_EVENTO_TEMP_NORMAL;
      enviarEvento(eventoTermico, temp);

      String payloadDHT = "{\"temperatura\": " + String(temp) + ", \"humedad\": " + String(hum) + "}";
      mqttPublicarSeguro(TOPIC_MQTT_AMBIENTE, payloadDHT);
    }
    vTaskDelay(pdMS_TO_TICKS(INTERVALO_DHT_MS));
  }
}

#endif // GESTOR_SENSORES_H