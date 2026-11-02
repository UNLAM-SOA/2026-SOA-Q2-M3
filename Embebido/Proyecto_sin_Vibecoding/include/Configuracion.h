#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <Arduino.h>

//Configuración de Red y Tópicos MQTT
const char* SSID_WIFI              = "Personal 2.4 GHZ";
const char* PASSWORD_WIFI          = "00442952504";
const char* SERVIDOR_MQTT          = "192.168.0.120";
const int   PUERTO_MQTT            = 1883;
const char* ID_CLIENTE_MQTT        = "ESP32_InfraGuard";
const char* TOPIC_MQTT_CONSULTA    = "acceso/tarjeta/consulta";
const char* TOPIC_MQTT_RESPUESTA   = "acceso/tarjeta/respuesta";
const char* TOPIC_MQTT_AMBIENTE    = "rack/sensores/ambiente";

//Definición de Pines del ESP32
const int PIN_PUERTA               = 14;
const int PIN_BUZZER               = 27;
const int PIN_RELE_MOTOR           = 13;
const int PIN_DHT                  = 15;
const int PIN_TIRA_LED             = 25;
const int PIN_RFID_SS              = 5;
const int PIN_RFID_RST             = 4;

//Constantes de Tiempos y Sistema
const unsigned long VELOCIDAD_SERIAL_BAUD       = 115200;
const uint32_t TIEMPO_VENTANA_ACCESO_MS         = 5000;
const uint32_t TIEMPO_SIRENA_PWM_MS             = 250;
const uint32_t TIEMPO_PANTALLA_MENSAJE_MS       = 3000;
const uint32_t INTERVALO_DHT_MS                 = 2000;
const uint32_t CADENCIA_SENSORES_MS             = 50;
const uint32_t INTERVALO_REINTENTO_MQTT_MS      = 3000;
const uint32_t TIEMPO_ANTIRREBOTE_RFID_MS       = 3000;
const uint32_t PAUSA_INIT_SERIAL_MS             = 100;
const uint32_t PAUSA_WIFI_CONEXION_MS           = 500;
const uint32_t PAUSA_INIT_RFID_MS               = 50;
const uint32_t PAUSA_TAREA_MQTT_MS              = 50;
const uint32_t PAUSA_LOOP_IDLE_MS               = 1000;
const uint32_t PAUSA_BORRADO_LCD_MS             = 5;
const uint32_t PAUSA_ESTABILIZACION_RGB_MS      = 2;
const uint32_t TIMEOUT_MUTEX_MQTT_MS            = 200;
const uint32_t TIMEOUT_POLL_MUTEX_MQTT_MS       = 100;
const uint16_t TIMEOUT_SOCKET_MQTT_SEG          = 2;
const TickType_t TIEMPO_NO_BLOQUEANTE           = 0;

//Parámetros de Hardware, Audio, Clima y Pantalla
const uint16_t CANTIDAD_LEDS_TIRA               = 8;
const float    UMBRAL_TEMP_MAXIMA_C             = 30.0f;
const float    TEMP_MIN_VALIDA_C                = -40.0f;
const float    TEMP_MAX_VALIDA_C                = 85.0f;
const float    VALOR_ACCION_POR_DEFECTO         = 0.0f;
const float    VALOR_EVENTO_POR_DEFECTO         = 0.0f;
const uint32_t FRECUENCIA_SIRENA_AGUDA_HZ       = 1400;
const uint32_t FRECUENCIA_SIRENA_GRAVE_HZ       = 800;

const uint8_t  COLUMNAS_LCD                     = 16;
const uint8_t  FILAS_LCD                        = 2;
const uint8_t  FILA_LCD_SUPERIOR                = 0;
const uint8_t  FILA_LCD_INFERIOR                = 1;
const uint8_t  COLUMNA_INICIAL_LCD              = 0;
const size_t   TAMANO_BUFFER_LINEA_LCD          = 17;
const size_t   INDICE_MAX_CARACTERES_LCD        = 16;
const char     ASCII_MIN_IMPRIMIBLE             = 32;
const char     ASCII_MAX_IMPRIMIBLE             = 126;

const uint8_t  COLOR_NULO                       = 0;
const uint8_t  COLOR_MEDIO                      = 128;
const uint8_t  COLOR_MAXIMO                     = 255;

const byte     BYTE_HEX_LIMITE_CERO             = 0x10;
const byte     REGISTRO_RFID_VACIO_00           = 0x00;
const byte     REGISTRO_RFID_VACIO_FF           = 0xFF;
const int      ESTADO_PUERTA_INDEFINIDO         = -1;

//Parámetros de Recursos de FreeRTOS
const UBaseType_t CAPACIDAD_COLA_RTOS           = 20;
const uint32_t    STACK_TAREA_ESTANDAR          = 4096;
const UBaseType_t PRIORIDAD_TAREA_CLIMA         = 1;
const UBaseType_t PRIORIDAD_TAREA_ESTANDAR      = 2;
const UBaseType_t PRIORIDAD_TAREA_MQTT          = 3;
const size_t      CAPACIDAD_JSON_MQTT           = 256;

//Estados del Sistema FSM
enum eEstadoSistema {
  ESTADO_EMBEDDED_INIT             = 100,
  ESTADO_EMBEDDED_REPOSO           = 200,
  ESTADO_EMBEDDED_ACCESO_CONCEDIDO = 300,
  ESTADO_EMBEDDED_PUERTA_ABIERTA   = 400,
  ESTADO_EMBEDDED_ALERTA_INTRUSION = 500
};

//Tipos de Eventos (FSM)
enum eTipoEvento {
  TIPO_EVENTO_TIMEOUT_ACCESO       = 2000,
  TIPO_EVENTO_TARJETA_VALIDA       = 3000,
  TIPO_EVENTO_TARJETA_DENEGADA     = 3100,
  TIPO_EVENTO_TARJETA_LEYENDO      = 3200,
  TIPO_EVENTO_SIN_SERVIDOR         = 3300,
  TIPO_EVENTO_FIN_MENSAJE_LCD      = 3400,
  TIPO_EVENTO_PUERTA_ABIERTA       = 4000,
  TIPO_EVENTO_PUERTA_CERRADA       = 5000,
  TIPO_EVENTO_TEMP_ALTA            = 6000,
  TIPO_EVENTO_TEMP_NORMAL          = 7000,
  TIPO_EVENTO_TEMP_ERROR           = 7100,
  TIPO_EVENTO_TICK_SIRENA          = 8000
};

//Tipos de Acciones sobre Actuadores
enum eTipoAccion {
  ACCION_RELE_COOLER_ON            = 100,
  ACCION_RELE_COOLER_OFF           = 200,
  ACCION_LED_APAGAR                = 300,
  ACCION_LED_BLANCO                = 400,
  ACCION_LED_ROJO                  = 500,
  ACCION_BUZZER_SILENCIAR          = 600,
  ACCION_SIRENA_PWM_TOGGLE         = 700,
  ACCION_LCD_ESTADO_REPOSO         = 800,
  ACCION_LCD_ESTADO_ACCESO         = 900,
  ACCION_LCD_ESTADO_ABIERTA        = 1000,
  ACCION_LCD_ESTADO_INTRUSION      = 1100,
  ACCION_LCD_TEMP_ACTUALIZAR       = 1200,
  ACCION_LCD_DENEGADO              = 1300,
  ACCION_LCD_VALIDANDO             = 1400,
  ACCION_LCD_TEMP_ERROR            = 1500,
  ACCION_LCD_SIN_SERVIDOR          = 1600
};

//Estructuras de Mensajería RTOS
struct stEvento {
  int   tipo;
  float valor;
};

struct stAccion {
  int   tipo;
  float valor;
};

//Handlers Globales de FreeRTOS
QueueHandle_t     colaEventos;
QueueHandle_t     colaAcciones;
TimerHandle_t     timerAcceso;
TimerHandle_t     timerSirena;
TimerHandle_t     timerMensajeLCD;
SemaphoreHandle_t mutexMQTT;

//Funciones de encolado seguro entre tareas
inline void enviarEvento(int tipoEvento, float valor = VALOR_EVENTO_POR_DEFECTO) {
  stEvento evento;
  evento.tipo  = tipoEvento;
  evento.valor = valor;
  xQueueSend(colaEventos, &evento, TIEMPO_NO_BLOQUEANTE);
}

inline void encolarAccion(int tipoAccion, float valor = VALOR_ACCION_POR_DEFECTO) {
  stAccion accion;
  accion.tipo  = tipoAccion;
  accion.valor = valor;
  xQueueSend(colaAcciones, &accion, TIEMPO_NO_BLOQUEANTE);
}

#endif // CONFIGURACION_H