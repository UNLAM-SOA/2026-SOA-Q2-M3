#ifndef GESTOR_LCD_H
#define GESTOR_LCD_H

#include <Wire.h>
#include "rgb_lcd.h"
#include "Configuracion.h"

rgb_lcd lcd;
float ultimaTempRegistrada = NAN;
bool  ultimoEstadoErrorDHT = false;

inline void lcdLimpiarSeguro() {
  lcd.clear();
  vTaskDelay(pdMS_TO_TICKS(PAUSA_BORRADO_LCD_MS));
}

inline void lcdEscribirLinea(uint8_t fila, const char* texto) {
  if (fila >= FILAS_LCD) return;

  char bufferLinea[TAMANO_BUFFER_LINEA_LCD];
  size_t indice = 0;

  if (texto != nullptr) {
    while (indice < INDICE_MAX_CARACTERES_LCD && texto[indice] != '\0') {
      char caracter = texto[indice];
      bufferLinea[indice] = (caracter >= ASCII_MIN_IMPRIMIBLE && caracter <= ASCII_MAX_IMPRIMIBLE) ? caracter : ' ';
      indice++;
    }
  }

  while (indice < INDICE_MAX_CARACTERES_LCD) {
    bufferLinea[indice++] = ' ';
  }
  bufferLinea[INDICE_MAX_CARACTERES_LCD] = '\0';

  lcd.setCursor(COLUMNA_INICIAL_LCD, fila);
  lcd.print(bufferLinea);
}

inline void lcdRenderizarTemperatura() {
  if (ultimoEstadoErrorDHT) {
    lcdEscribirLinea(FILA_LCD_SUPERIOR, "TEMP: ERROR DHT");
  } else if (isnan(ultimaTempRegistrada) || ultimaTempRegistrada < TEMP_MIN_VALIDA_C || ultimaTempRegistrada > TEMP_MAX_VALIDA_C) {
    lcdEscribirLinea(FILA_LCD_SUPERIOR, "TEMP: --.- C");
  } else {
    char textoTemp[TAMANO_BUFFER_LINEA_LCD];
    snprintf(textoTemp, sizeof(textoTemp), "TEMP: %.1f C", ultimaTempRegistrada);
    lcdEscribirLinea(FILA_LCD_SUPERIOR, textoTemp);
  }
}

inline void lcdActualizarValorTemperatura(float nuevaTemp) {
  ultimaTempRegistrada = nuevaTemp;
  ultimoEstadoErrorDHT = false;
  lcdRenderizarTemperatura();
}

inline void lcdMostrarErrorSensorTemperatura() {
  ultimoEstadoErrorDHT = true;
  lcdRenderizarTemperatura();
}

inline void lcdCambiarPantallaEstado(uint8_t r, uint8_t g, uint8_t b, const char* mensajeEstado) {
  lcdLimpiarSeguro();
  lcd.setRGB(r, g, b);
  vTaskDelay(pdMS_TO_TICKS(PAUSA_ESTABILIZACION_RGB_MS));
  lcdRenderizarTemperatura();
  lcdEscribirLinea(FILA_LCD_INFERIOR, mensajeEstado);
}

inline void lcdInicializarHardware() {
  lcd.begin(COLUMNAS_LCD, FILAS_LCD);
  lcdLimpiarSeguro();
  lcd.setRGB(COLOR_NULO, COLOR_NULO, COLOR_MAXIMO);
  vTaskDelay(pdMS_TO_TICKS(PAUSA_ESTABILIZACION_RGB_MS));
  lcdEscribirLinea(FILA_LCD_SUPERIOR, "INFRAGUARD OS");
  lcdEscribirLinea(FILA_LCD_INFERIOR, "CONECTANDO WIFI");
}

#endif // GESTOR_LCD_H