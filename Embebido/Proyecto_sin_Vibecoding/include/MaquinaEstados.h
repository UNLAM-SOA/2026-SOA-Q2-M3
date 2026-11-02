#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include "Configuracion.h"

int estadoActual = ESTADO_EMBEDDED_INIT;

// -- Callbacks de Temporizadores de Software --
inline void callbackTimerAcceso(TimerHandle_t xTimer) {
  enviarEvento(TIPO_EVENTO_TIMEOUT_ACCESO);
}

inline void callbackTimerSirena(TimerHandle_t xTimer) {
  enviarEvento(TIPO_EVENTO_TICK_SIRENA);
}

inline void callbackTimerMensajeLCD(TimerHandle_t xTimer) {
  enviarEvento(TIPO_EVENTO_FIN_MENSAJE_LCD);
}

// -- Funciones Encapsuladas de Actualización de Actuadores para la FSM --
inline void iniciarTemporizadorMensajeTemporal() {
  xTimerStop(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
  xTimerStart(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
}

inline void accionarModoReposo() {
  xTimerStop(timerAcceso, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerSirena, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
  encolarAccion(ACCION_BUZZER_SILENCIAR);
  encolarAccion(ACCION_LED_APAGAR);
  encolarAccion(ACCION_LCD_ESTADO_REPOSO);
}

inline void accionarVentanaAccesoConcedido() {
  xTimerStop(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerAcceso, TIEMPO_NO_BLOQUEANTE);
  xTimerStart(timerAcceso, TIEMPO_NO_BLOQUEANTE);
  encolarAccion(ACCION_LCD_ESTADO_ACCESO);
}

inline void accionarModoPuertaAbierta() {
  xTimerStop(timerAcceso, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerSirena, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
  encolarAccion(ACCION_BUZZER_SILENCIAR);
  encolarAccion(ACCION_LED_BLANCO);
  encolarAccion(ACCION_LCD_ESTADO_ABIERTA);
}

inline void accionarAlertaIntrusion() {
  xTimerStop(timerAcceso, TIEMPO_NO_BLOQUEANTE);
  xTimerStop(timerMensajeLCD, TIEMPO_NO_BLOQUEANTE);
  encolarAccion(ACCION_LED_ROJO);
  encolarAccion(ACCION_LCD_ESTADO_INTRUSION);
  xTimerStart(timerSirena, TIEMPO_NO_BLOQUEANTE);
}

inline void accionarPulsoSirena() {
  encolarAccion(ACCION_SIRENA_PWM_TOGGLE);
}

inline void accionarAvisoValidandoTarjeta() {
  encolarAccion(ACCION_LCD_VALIDANDO);
  iniciarTemporizadorMensajeTemporal();
}

inline void accionarAvisoTarjetaDenegada() {
  encolarAccion(ACCION_LCD_DENEGADO);
  iniciarTemporizadorMensajeTemporal();
}

inline void accionarAvisoSinServidor() {
  encolarAccion(ACCION_LCD_SIN_SERVIDOR);
  iniciarTemporizadorMensajeTemporal();
}

inline void restaurarPantallaEstadoReposo() {
  encolarAccion(ACCION_LCD_ESTADO_REPOSO);
}

inline void restaurarPantallaEstadoAcceso() {
  encolarAccion(ACCION_LCD_ESTADO_ACCESO);
}

inline void restaurarPantallaEstadoPuertaAbierta() {
  encolarAccion(ACCION_LCD_ESTADO_ABIERTA);
}

inline void restaurarPantallaEstadoIntrusion() {
  encolarAccion(ACCION_LCD_ESTADO_INTRUSION);
}

inline void accionarClimaTemperaturaAlta(float temperatura) {
  encolarAccion(ACCION_LCD_TEMP_ACTUALIZAR, temperatura);
  encolarAccion(ACCION_RELE_COOLER_ON);
}

inline void accionarClimaTemperaturaNormal(float temperatura) {
  encolarAccion(ACCION_LCD_TEMP_ACTUALIZAR, temperatura);
  encolarAccion(ACCION_RELE_COOLER_OFF);
}

inline void accionarClimaErrorSensor() {
  encolarAccion(ACCION_LCD_TEMP_ERROR);
}

// -- Máquina de Estados Finito (FSM) --
inline void maquina_estados(stEvento evento) {
  switch (estadoActual) {
    case ESTADO_EMBEDDED_INIT: {
      switch (evento.tipo) {
        case TIPO_EVENTO_PUERTA_CERRADA:
          Serial.println("\n[FSM]: INIT -> Puerta CERRADA -> Transicion a REPOSO");
          accionarModoReposo();
          estadoActual = ESTADO_EMBEDDED_REPOSO;
          break;
        case TIPO_EVENTO_PUERTA_ABIERTA:
          Serial.println("\n[FSM]: INIT -> Puerta ABIERTA -> Transicion a PUERTA_ABIERTA");
          accionarModoPuertaAbierta();
          estadoActual = ESTADO_EMBEDDED_PUERTA_ABIERTA;
          break;
        case TIPO_EVENTO_TEMP_ALTA:
          accionarClimaTemperaturaAlta(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_NORMAL:
          accionarClimaTemperaturaNormal(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_ERROR:
          accionarClimaErrorSensor();
          break;
      }
    } break;

    case ESTADO_EMBEDDED_REPOSO: {
      switch (evento.tipo) {
        case TIPO_EVENTO_TARJETA_LEYENDO:
          accionarAvisoValidandoTarjeta();
          break;
        case TIPO_EVENTO_TARJETA_VALIDA:
          Serial.println("[FSM]: REPOSO | [EVENTO]: TARJETA VALIDA -> ACCESO_CONCEDIDO");
          accionarVentanaAccesoConcedido();
          estadoActual = ESTADO_EMBEDDED_ACCESO_CONCEDIDO;
          break;
        case TIPO_EVENTO_TARJETA_DENEGADA:
          Serial.println("[FSM]: REPOSO | [EVENTO]: TARJETA DENEGADA");
          accionarAvisoTarjetaDenegada();
          break;
        case TIPO_EVENTO_SIN_SERVIDOR:
          accionarAvisoSinServidor();
          break;
        case TIPO_EVENTO_FIN_MENSAJE_LCD:
          restaurarPantallaEstadoReposo();
          break;
        case TIPO_EVENTO_PUERTA_ABIERTA:
          Serial.println("\n[FSM]: REPOSO | [EVENTO]: PUERTA ABIERTA SIN PERMISO -> ALERTA_INTRUSION");
          accionarAlertaIntrusion();
          estadoActual = ESTADO_EMBEDDED_ALERTA_INTRUSION;
          break;
        case TIPO_EVENTO_TEMP_ALTA:
          accionarClimaTemperaturaAlta(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_NORMAL:
          accionarClimaTemperaturaNormal(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_ERROR:
          accionarClimaErrorSensor();
          break;
      }
    } break;

    case ESTADO_EMBEDDED_ACCESO_CONCEDIDO: {
      switch (evento.tipo) {
        case TIPO_EVENTO_PUERTA_ABIERTA:
          Serial.println("\n[FSM]: ACCESO_CONCEDIDO | [EVENTO]: PUERTA ABIERTA -> PUERTA_ABIERTA");
          accionarModoPuertaAbierta();
          estadoActual = ESTADO_EMBEDDED_PUERTA_ABIERTA;
          break;
        case TIPO_EVENTO_TIMEOUT_ACCESO:
          Serial.println("\n[FSM]: ACCESO_CONCEDIDO | [EVENTO]: TIMEOUT (5s) -> REPOSO");
          accionarModoReposo();
          estadoActual = ESTADO_EMBEDDED_REPOSO;
          break;
        case TIPO_EVENTO_TARJETA_LEYENDO:
          accionarAvisoValidandoTarjeta();
          break;
        case TIPO_EVENTO_TARJETA_VALIDA:
          Serial.println("[FSM]: ACCESO_CONCEDIDO | [EVENTO]: Reiniciando ventana de 5s");
          accionarVentanaAccesoConcedido();
          break;
        case TIPO_EVENTO_TARJETA_DENEGADA:
          accionarAvisoTarjetaDenegada();
          break;
        case TIPO_EVENTO_SIN_SERVIDOR:
          accionarAvisoSinServidor();
          break;
        case TIPO_EVENTO_FIN_MENSAJE_LCD:
          restaurarPantallaEstadoAcceso();
          break;
        case TIPO_EVENTO_TEMP_ALTA:
          accionarClimaTemperaturaAlta(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_NORMAL:
          accionarClimaTemperaturaNormal(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_ERROR:
          accionarClimaErrorSensor();
          break;
      }
    } break;

    case ESTADO_EMBEDDED_PUERTA_ABIERTA: {
      switch (evento.tipo) {
        case TIPO_EVENTO_PUERTA_CERRADA:
          Serial.println("\n[FSM]: PUERTA_ABIERTA | [EVENTO]: PUERTA CERRADA -> REPOSO");
          accionarModoReposo();
          estadoActual = ESTADO_EMBEDDED_REPOSO;
          break;
        case TIPO_EVENTO_TARJETA_LEYENDO:
          accionarAvisoValidandoTarjeta();
          break;
        case TIPO_EVENTO_TARJETA_DENEGADA:
          accionarAvisoTarjetaDenegada();
          break;
        case TIPO_EVENTO_SIN_SERVIDOR:
          accionarAvisoSinServidor();
          break;
        case TIPO_EVENTO_FIN_MENSAJE_LCD:
          restaurarPantallaEstadoPuertaAbierta();
          break;
        case TIPO_EVENTO_TEMP_ALTA:
          accionarClimaTemperaturaAlta(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_NORMAL:
          accionarClimaTemperaturaNormal(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_ERROR:
          accionarClimaErrorSensor();
          break;
      }
    } break;

    case ESTADO_EMBEDDED_ALERTA_INTRUSION: {
      switch (evento.tipo) {
        case TIPO_EVENTO_PUERTA_CERRADA:
          Serial.println("\n[FSM]: ALERTA_INTRUSION | [EVENTO]: PUERTA CERRADA -> REPOSO");
          accionarModoReposo();
          estadoActual = ESTADO_EMBEDDED_REPOSO;
          break;
        case TIPO_EVENTO_TARJETA_VALIDA:
          Serial.println("\n[FSM]: ALERTA_INTRUSION | [EVENTO]: TARJETA VALIDA (DESARME) -> PUERTA_ABIERTA");
          accionarModoPuertaAbierta();
          estadoActual = ESTADO_EMBEDDED_PUERTA_ABIERTA;
          break;
        case TIPO_EVENTO_TICK_SIRENA:
          accionarPulsoSirena();
          break;
        case TIPO_EVENTO_TARJETA_LEYENDO:
          accionarAvisoValidandoTarjeta();
          break;
        case TIPO_EVENTO_TARJETA_DENEGADA:
          accionarAvisoTarjetaDenegada();
          break;
        case TIPO_EVENTO_SIN_SERVIDOR:
          accionarAvisoSinServidor();
          break;
        case TIPO_EVENTO_FIN_MENSAJE_LCD:
          restaurarPantallaEstadoIntrusion();
          break;
        case TIPO_EVENTO_TEMP_ALTA:
          accionarClimaTemperaturaAlta(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_NORMAL:
          accionarClimaTemperaturaNormal(evento.valor);
          break;
        case TIPO_EVENTO_TEMP_ERROR:
          accionarClimaErrorSensor();
          break;
      }
    } break;
  }
}

inline void tareaFSM(void *parameter) {
  stEvento evento;
  while (true) {
    if (xQueueReceive(colaEventos, &evento, portMAX_DELAY) == pdTRUE) {
      maquina_estados(evento);
    }
  }
}

#endif // MAQUINA_ESTADOS_H