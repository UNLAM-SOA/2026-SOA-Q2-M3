#ifndef GESTOR_ACTUADORES_H
#define GESTOR_ACTUADORES_H

#include <Adafruit_NeoPixel.h>
#include "Configuracion.h"
#include "GestorLCD.h"

Adafruit_NeoPixel tiraLed(CANTIDAD_LEDS_TIRA, PIN_TIRA_LED, NEO_GRB + NEO_KHZ800);
bool sirenaTonoAgudoActivo = false;

inline void actuadoresInicializarPines() {
  pinMode(PIN_PUERTA, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELE_MOTOR, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_RELE_MOTOR, LOW);
}

inline void actuadoresInicializarNeoPixel() {
  tiraLed.begin();
  tiraLed.clear();
  tiraLed.show();
}

inline void prenderReleCooler() {
  digitalWrite(PIN_RELE_MOTOR, HIGH);
}

inline void apagarReleCooler() {
  digitalWrite(PIN_RELE_MOTOR, LOW);
}

inline void apagarTiraLed() {
  tiraLed.fill(tiraLed.Color(COLOR_NULO, COLOR_NULO, COLOR_NULO));
  tiraLed.show();
}

inline void prenderTiraLedBlanco() {
  tiraLed.fill(tiraLed.Color(COLOR_MAXIMO, COLOR_MAXIMO, COLOR_MAXIMO));
  tiraLed.show();
}

inline void prenderTiraLedRojo() {
  tiraLed.fill(tiraLed.Color(COLOR_MAXIMO, COLOR_NULO, COLOR_NULO));
  tiraLed.show();
}

inline void silenciarBuzzer() {
  noTone(PIN_BUZZER);
}

inline void conmutarTonoSirena() {
  sirenaTonoAgudoActivo = !sirenaTonoAgudoActivo;
  uint32_t frecuencia = sirenaTonoAgudoActivo ? FRECUENCIA_SIRENA_AGUDA_HZ : FRECUENCIA_SIRENA_GRAVE_HZ;
  tone(PIN_BUZZER, frecuencia);
}

inline void mostrarPantallaReposo() {
  lcdCambiarPantallaEstado(COLOR_NULO, COLOR_NULO, COLOR_MAXIMO, "PUERTA CERRADA");
}

inline void mostrarPantallaAccesoConcedido() {
  lcdCambiarPantallaEstado(COLOR_NULO, COLOR_MAXIMO, COLOR_NULO, "ACCESO CONCEDIDO");
}

inline void mostrarPantallaPuertaAbierta() {
  lcdCambiarPantallaEstado(COLOR_MAXIMO, COLOR_MAXIMO, COLOR_MAXIMO, "PUERTA ABIERTA");
}

inline void mostrarPantallaAlertaIntrusion() {
  lcdCambiarPantallaEstado(COLOR_MAXIMO, COLOR_NULO, COLOR_NULO, "ALERTA INTRUSION");
}

inline void mostrarPantallaAccesoDenegado() {
  lcdCambiarPantallaEstado(COLOR_MAXIMO, COLOR_NULO, COLOR_NULO, "ACCESO DENEGADO");
}

inline void mostrarPantallaValidando() {
  lcdCambiarPantallaEstado(COLOR_MAXIMO, COLOR_MAXIMO, COLOR_NULO, "VALIDANDO...");
}

inline void mostrarPantallaSinServidor() {
  lcdCambiarPantallaEstado(COLOR_MAXIMO, COLOR_MEDIO, COLOR_NULO, "SIN SERVIDOR");
}

inline void ejecutarAccionFisica(stAccion accion) {
  switch (accion.tipo) {
    case ACCION_RELE_COOLER_ON:        prenderReleCooler();                        break;
    case ACCION_RELE_COOLER_OFF:       apagarReleCooler();                         break;
    case ACCION_LED_APAGAR:            apagarTiraLed();                            break;
    case ACCION_LED_BLANCO:            prenderTiraLedBlanco();                     break;
    case ACCION_LED_ROJO:              prenderTiraLedRojo();                       break;
    case ACCION_BUZZER_SILENCIAR:      silenciarBuzzer();                          break;
    case ACCION_SIRENA_PWM_TOGGLE:     conmutarTonoSirena();                       break;
    case ACCION_LCD_ESTADO_REPOSO:     mostrarPantallaReposo();                    break;
    case ACCION_LCD_ESTADO_ACCESO:     mostrarPantallaAccesoConcedido();           break;
    case ACCION_LCD_ESTADO_ABIERTA:    mostrarPantallaPuertaAbierta();             break;
    case ACCION_LCD_ESTADO_INTRUSION:  mostrarPantallaAlertaIntrusion();           break;
    case ACCION_LCD_TEMP_ACTUALIZAR:   lcdActualizarValorTemperatura(accion.valor); break;
    case ACCION_LCD_TEMP_ERROR:        lcdMostrarErrorSensorTemperatura();         break;
    case ACCION_LCD_DENEGADO:          mostrarPantallaAccesoDenegado();            break;
    case ACCION_LCD_VALIDANDO:         mostrarPantallaValidando();                 break;
    case ACCION_LCD_SIN_SERVIDOR:      mostrarPantallaSinServidor();               break;
  }
}

inline void tareaEjecutarAcciones(void *parameter) {
  stAccion accion;
  while (true) {
    if (xQueueReceive(colaAcciones, &accion, portMAX_DELAY) == pdTRUE) {
      ejecutarAccionFisica(accion);
    }
  }
}

#endif // GESTOR_ACTUADORES_H