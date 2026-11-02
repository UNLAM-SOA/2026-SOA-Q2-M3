#include "Configuracion.h"
#include "GestorLCD.h"
#include "GestorActuadores.h"
#include "GestorMQTT.h"
#include "GestorSensores.h"
#include "MaquinaEstados.h"

void inicializarRecursosRTOS() {
  mutexMQTT       = xSemaphoreCreateMutex();
  colaEventos     = xQueueCreate(CAPACIDAD_COLA_RTOS, sizeof(stEvento));
  colaAcciones    = xQueueCreate(CAPACIDAD_COLA_RTOS, sizeof(stAccion));

  timerAcceso     = xTimerCreate("TimerAcceso", pdMS_TO_TICKS(TIEMPO_VFENTANA_ACCESO_MS),   pdFALSE, NULL, callbackTimerAcceso);
  timerSirena     = xTimerCreate("TimerSirena", pdMS_TO_TICKS(TIEMPO_SIRENA_PWM_MS),       pdTRUE,  NULL, callbackTimerSirena);
  timerMensajeLCD = xTimerCreate("TimerMsgLCD", pdMS_TO_TICKS(TIEMPO_PANTALLA_MENSAJE_MS), pdFALSE, NULL, callbackTimerMensajeLCD);
}

void lanzarTareasConcurrentes() {
  xTaskCreate(tareaGetEvent, "Task_GetEvent", STACK_TAREA_ESTANDAR, NULL, PRIORIDAD_TAREA_ESTANDAR, NULL);
  xTaskCreate(tareaFSM, "Task_FSM", STACK_TAREA_ESTANDAR, NULL, PRIORIDAD_TAREA_ESTANDAR, NULL);
  xTaskCreate(tareaEjecutarAcciones, "Task_Acciones", STACK_TAREA_ESTANDAR, NULL, PRIORIDAD_TAREA_ESTANDAR, NULL);
  xTaskCreate(tareaMonitoreoClimatico, "Task_Clima",    STACK_TAREA_ESTANDAR, NULL, PRIORIDAD_TAREA_CLIMA, NULL);
  xTaskCreate(tareaMQTT, "Task_MQTT", STACK_TAREA_ESTANDAR, NULL, PRIORIDAD_TAREA_MQTT, NULL);
}

void do_init() {
  Serial.begin(VELOCIDAD_SERIAL_BAUD);
  vTaskDelay(pdMS_TO_TICKS(PAUSA_INIT_SERIAL_MS));
  Serial.println("\n\n--- Iniciando Sistema Operativo InfraGuard ---");

  actuadoresInicializarPines();
  lcdInicializarHardware();
  wifiInicializarConexion();
  mqttInicializarCliente();
  sensoresInicializarHardware();
  actuadoresInicializarNeoPixel();

  inicializarRecursosRTOS();
  lcdLimpiarSeguro();
  lanzarTareasConcurrentes();

  Serial.println("[SYS] Tareas de FreeRTOS lanzadas. Sistema en ejecucion.");
}

void setup() {
  do_init();
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(PAUSA_LOOP_IDLE_MS));
}