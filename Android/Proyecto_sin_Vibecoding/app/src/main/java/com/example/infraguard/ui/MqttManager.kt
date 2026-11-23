@file:Suppress("SpellCheckingInspection")

package com.example.infraguard

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.withContext
import org.eclipse.paho.client.mqttv3.MqttClient
import org.eclipse.paho.client.mqttv3.MqttConnectOptions
import org.eclipse.paho.client.mqttv3.MqttMessage
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

object MqttManager {

    private const val TOPIC = "control/cooler"
    private const val TIMEOUT_SEGUNDOS = 3
    private const val TIMEOUT_ESPERA_MS = 3000L

    private var brokerUrl: String = "tcp://192.168.0.120:1883"
    private var cliente: MqttClient? = null
    private val mutex = Mutex()

    fun actualizarIp(nuevaIp: String) {
        val ipLimpia = nuevaIp.trim()
        if (ipLimpia.isNotEmpty()) {
            brokerUrl = "tcp://$ipLimpia:1883"
            desconectar()
        }
    }

    private fun conectarSiEsNecesario(): MqttClient {
        val mqtt = cliente ?: MqttClient(
            brokerUrl,
            MqttClient.generateClientId(),
            MemoryPersistence()
        ).apply {
            timeToWait = TIMEOUT_ESPERA_MS
        }.also {
            cliente = it
        }

        if (!mqtt.isConnected) {
            val opciones = MqttConnectOptions().apply {
                connectionTimeout = TIMEOUT_SEGUNDOS
                isAutomaticReconnect = false
                isCleanSession = true
            }
            mqtt.connect(opciones)
        }

        return mqtt
    }

    suspend fun publicarModoCooler(modo: String) {
        if (!mutex.tryLock()) return
        try {
            withContext(Dispatchers.IO) {
                val mqtt = conectarSiEsNecesario()
                val mensaje = MqttMessage(
                    """{"cooler":"$modo"}""".toByteArray()
                ).apply {
                    qos = 0
                }
                mqtt.publish(TOPIC, mensaje)
            }
        } finally {
            mutex.unlock()
        }
    }

    fun desconectar() {
        try {
            cliente?.let {
                if (it.isConnected) it.disconnectForcibly(1000L)
                it.close(true)
            }
        } catch (_: Exception) {
        } finally {
            cliente = null
        }
    }
}