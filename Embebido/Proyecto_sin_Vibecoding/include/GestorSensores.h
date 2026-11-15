#ifndef GESTOR_SENSORES_H
#define GESTOR_SENSORES_H

#include <SPI.h>
#include <MFRC522.h>
#include <DHTesp.h>
#include "Configuracion.h"
#include "GestorMQTT.h"

MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);
DHTesp  dht;

inline void reiniciarLectorRFID() {
  pinMode(PIN_RFID_RST, OUTPUT);
  digitalWrite(PIN_RFID_RST, LOW);
  vTaskDelay(pdMS_TO_TICKS(PAUSA_RESET_FISICO_RFID_MS));
  digitalWrite(PIN_RFID_RST, HIGH);
  vTaskDelay(pdMS_TO_TICKS(PAUSA_RESET_FISICO_RFID_MS));

  rfid.PCD_Init();
  rfid.PCD_AntennaOn();
  rfid.PCD_SetAntennaGain(GANANCIA_MAXIMA_RFID);
}

inline void sensoresInicializarHardware() {
  SPI.begin();
  SPI.setFrequency(FRECUENCIA_SPI_RFID_HZ);
  reiniciarLectorRFID();
  vTaskDelay(pdMS_TO_TICKS(PAUSA_INIT_RFID_MS));

  byte versionRFID = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.printf("[RFID] VersionReg = 0x%02X\n", versionRFID);
  if (versionRFID == REGISTRO_RFID_VACIO_00 || versionRFID == REGISTRO_RFID_VACIO_FF) {
    Serial.println("[RFID] SIN COMUNICACION con el RC522. Revisar cableado y 3.3V.");
  } else {
    Serial.println("[RFID] RC522 detectado correctamente.");
  }

  dht.setup(PIN_DHT, MODELO_SENSOR_DHT);
  Serial.println("[DHT11] Sensor inicializado en GPIO " + String(PIN_DHT));
}

inline String extraerHashTarjetaRFID() {
  String hashTarjeta = "";
  for (byte i = INDICE_INICIAL_CERO; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < BYTE_HEX_LIMITE_CERO) {
      hashTarjeta += PREFIJO_HEX_CERO;
    }
    hashTarjeta += String(rfid.uid.uidByte[i], HEX);
  }
  return hashTarjeta;
}

inline void verificarSaludRFID() {
  byte versionRFID  = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  byte estadoAntena = rfid.PCD_ReadRegister(MFRC522::TxControlReg);

  if (versionRFID == REGISTRO_RFID_VACIO_00 || versionRFID == REGISTRO_RFID_VACIO_FF || ((estadoAntena & MASCARA_ANTENA_RFID) != MASCARA_ANTENA_RFID)) {
    reiniciarLectorRFID();
  } else {
    rfid.PCD_AntennaOn();
    rfid.PCD_SetAntennaGain(GANANCIA_MAXIMA_RFID);
  }
}

inline void muestrearLectorRFID(unsigned long &ultimoEscaneo) {
  byte estadoAntena = rfid.PCD_ReadRegister(MFRC522::TxControlReg);
  if ((estadoAntena & MASCARA_ANTENA_RFID) != MASCARA_ANTENA_RFID) {
    reiniciarLectorRFID();
  }

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (ultimoEscaneo == TIEMPO_INICIAL_CERO || millis() - ultimoEscaneo > TIEMPO_ANTIRREBOTE_RFID_MS) {
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
  unsigned long ultimoEscaneoRFID = TIEMPO_INICIAL_CERO;
  unsigned long ultimoChequeoSaludRFID = TIEMPO_INICIAL_CERO;

  while (true) {
    muestrearSensorPuerta(estadoPuertaAnterior);
    muestrearLectorRFID(ultimoEscaneoRFID);

    if (millis() - ultimoChequeoSaludRFID > TIEMPO_ANTIRREBOTE_RFID_MS) {
      verificarSaludRFID();
      ultimoChequeoSaludRFID = millis();
    }

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

#endif
