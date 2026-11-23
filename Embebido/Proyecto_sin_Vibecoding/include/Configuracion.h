#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <Arduino.h>

#define SSID_WIFI                       "Personal 2.4 GHZ"
#define PASSWORD_WIFI                   "00442952504"
#define SERVIDOR_MQTT                   "192.168.0.120"
#define PUERTO_MQTT                     1883
#define ID_CLIENTE_MQTT                 "ESP32_InfraGuard"
#define TOPIC_MQTT_CONSULTA             "acceso/tarjeta/consulta"
#define TOPIC_MQTT_RESPUESTA            "acceso/tarjeta/respuesta"
#define TOPIC_MQTT_COOLER               "control/cooler"
#define TOPIC_MQTT_AMBIENTE             "rack/sensores/ambiente"
volatile bool COOLER_MANUAL = false;

#define PIN_PUERTA                      14
#define PIN_BUZZER                      27
#define PIN_RELE_MOTOR                  13
#define PIN_DHT                         15
#define PIN_TIRA_LED                    25
#define PIN_RFID_SS                     5
#define PIN_RFID_RST                    4

#define VELOCIDAD_SERIAL_BAUD           115200UL
#define TIEMPO_VENTANA_ACCESO_MS        5000UL
#define TIEMPO_SIRENA_PWM_MS            250UL
#define TIEMPO_PANTALLA_MENSAJE_MS      3000UL
#define INTERVALO_DHT_MS                2000UL
#define CADENCIA_SENSORES_MS            50UL
#define INTERVALO_REINTENTO_MQTT_MS     3000UL
#define TIEMPO_ANTIRREBOTE_RFID_MS      3000UL
#define PAUSA_INIT_SERIAL_MS            100UL
#define PAUSA_WIFI_CONEXION_MS          500UL
#define PAUSA_INIT_RFID_MS              50UL
#define PAUSA_RESET_FISICO_RFID_MS      10UL
#define PAUSA_TAREA_MQTT_MS             50UL
#define PAUSA_LOOP_IDLE_MS              1000UL
#define PAUSA_BORRADO_LCD_MS            5UL
#define PAUSA_ESTABILIZACION_RGB_MS     2UL
#define PAUSA_ESTABILIZACION_LED_MS     15UL
#define TIMEOUT_MUTEX_MQTT_MS           200UL
#define TIMEOUT_POLL_MUTEX_MQTT_MS      100UL
#define TIMEOUT_SOCKET_MQTT_SEG         2U
#define TIEMPO_NO_BLOQUEANTE            ((TickType_t)0)

#define CANTIDAD_LEDS_TIRA              16
#define BRILLO_TIRA_LED                 30
#define UMBRAL_TEMP_MAXIMA_C            30.0f
#define TEMP_MIN_VALIDA_C               -40.0f
#define TEMP_MAX_VALIDA_C               85.0f
#define VALOR_ACCION_POR_DEFECTO        0.0f
#define VALOR_EVENTO_POR_DEFECTO        0.0f
#define FRECUENCIA_SIRENA_AGUDA_HZ      1400UL
#define FRECUENCIA_SIRENA_GRAVE_HZ      800UL

#define COLUMNAS_LCD                    16
#define FILAS_LCD                       2
#define FILA_LCD_SUPERIOR               0
#define FILA_LCD_INFERIOR               1
#define COLUMNA_INICIAL_LCD             0
#define TAMANO_BUFFER_LINEA_LCD         17
#define INDICE_MAX_CARACTERES_LCD       16
#define ASCII_MIN_IMPRIMIBLE            32
#define ASCII_MAX_IMPRIMIBLE            126

#define COLOR_NULO                      0
#define COLOR_MEDIO                     128
#define COLOR_MAXIMO                    255

#define BYTE_HEX_LIMITE_CERO            0x10
#define REGISTRO_RFID_VACIO_00          0x00
#define REGISTRO_RFID_VACIO_FF          0xFF
#define MASCARA_ANTENA_RFID             0x03
#define FRECUENCIA_SPI_RFID_HZ          1000000UL
#define GANANCIA_MAXIMA_RFID            MFRC522::RxGain_max
#define ESTADO_PUERTA_INDEFINIDO        -1
#define MODELO_SENSOR_DHT               DHTesp::DHT11
#define INDICE_INICIAL_CERO             0
#define TIEMPO_INICIAL_CERO             0UL
#define PREFIJO_HEX_CERO                "0"

#define CAPACIDAD_COLA_RTOS             20
#define STACK_TAREA_ESTANDAR            4096
#define PRIORIDAD_TAREA_CLIMA           1
#define PRIORIDAD_TAREA_ESTANDAR        2
#define PRIORIDAD_TAREA_MQTT            3
#define CAPACIDAD_JSON_MQTT             256

enum eEstadoSistema {
  ESTADO_EMBEDDED_INIT             = 100,
  ESTADO_EMBEDDED_REPOSO           = 200,
  ESTADO_EMBEDDED_ACCESO_CONCEDIDO = 300,
  ESTADO_EMBEDDED_PUERTA_ABIERTA   = 400,
  ESTADO_EMBEDDED_ALERTA_INTRUSION = 500
};

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
  TIPO_EVENTO_COOLER_MANUAL        = 6500,
  TIPO_EVENTO_TEMP_NORMAL          = 7000,
  TIPO_EVENTO_TEMP_ERROR           = 7100,
  TIPO_EVENTO_TICK_SIRENA          = 8000
};

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

struct stEvento {
  int   tipo;
  float valor;
};

struct stAccion {
  int   tipo;
  float valor;
};

QueueHandle_t     colaEventos;
QueueHandle_t     colaAcciones;
TimerHandle_t     timerAcceso;
TimerHandle_t     timerSirena;
TimerHandle_t     timerMensajeLCD;
SemaphoreHandle_t mutexMQTT;

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

#endif
