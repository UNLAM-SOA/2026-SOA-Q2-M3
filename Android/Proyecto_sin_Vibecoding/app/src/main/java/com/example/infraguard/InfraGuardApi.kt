@file:Suppress("SpellCheckingInspection")

package com.example.infraguard

import com.google.gson.annotations.SerializedName
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.PUT

data class ClimaResponse(
    val temperatura: Float,
    val humedad: Float,
    @field:SerializedName("cooler_activo") val coolerActivo: Boolean,
    val timestamp: String?,
    @field:SerializedName("cooler_manual") val coolerManual: Boolean = false
)

data class AccesoItem(
    @field:SerializedName("id_acceso") val idAcceso: Long,
    @field:SerializedName("hash_tarjeta") val hashTarjeta: String,
    @field:SerializedName("timestamp_acceso") val timestampAcceso: String,
    @field:SerializedName("estado_autorizacion") val estadoAutorizacion: String,
    @field:SerializedName("id_usuario") val idUsuario: Long?,
    @field:SerializedName("nombre_usuario") val nombreUsuario: String?
)

data class TarjetaUsuarioItem(
    @field:SerializedName("id_tarjeta") val idTarjeta: Long,
    @field:SerializedName("hash_tarjeta") val hashTarjeta: String,
    @field:SerializedName("id_usuario") val idUsuario: Long,
    @field:SerializedName("nombre_usuario") val nombreUsuario: String,
    val estado: Boolean
)

data class NuevaTarjetaRequest(
    @field:SerializedName("nombre_usuario") val nombreUsuario: String,
    @field:SerializedName("hash_tarjeta") val hashTarjeta: String
)

data class CambiarEstadoRequest(
    @field:SerializedName("id_usuario") val idUsuario: Long,
    val estado: Boolean
)

data class CoolerRequest(
    val cooler: String
)

interface InfraGuardApi {
    @GET("api/clima")
    suspend fun obtenerClima(): ClimaResponse

    @GET("api/accesos")
    suspend fun obtenerAccesos(): List<AccesoItem>

    @GET("api/tarjetas")
    suspend fun obtenerTarjetas(): List<TarjetaUsuarioItem>

    @POST("api/tarjetas")
    suspend fun registrarTarjeta(@Body req: NuevaTarjetaRequest): Any

    @PUT("api/usuarios/estado")
    suspend fun cambiarEstadoUsuario(@Body req: CambiarEstadoRequest): Any

    @POST("api/remoto/autorizar")
    suspend fun autorizarRemoto(): Any

    @POST("api/remoto/cooler")
    suspend fun cambiarModoCooler(@Body req: CoolerRequest): Any
}

object RetrofitClient {
    var ipServidor: String = "192.168.0.120"
        private set

    var api: InfraGuardApi = crearCliente(ipServidor)
        private set

    fun actualizarIp(nuevaIp: String) {
        val ipLimpia = nuevaIp.trim()
        if (ipLimpia.isNotEmpty()) {
            ipServidor = ipLimpia
            api = crearCliente(ipLimpia)
        }
    }

    private fun crearCliente(ip: String): InfraGuardApi {
        return Retrofit.Builder()
            .baseUrl("http://$ip:1880/")
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(InfraGuardApi::class.java)
    }
}
