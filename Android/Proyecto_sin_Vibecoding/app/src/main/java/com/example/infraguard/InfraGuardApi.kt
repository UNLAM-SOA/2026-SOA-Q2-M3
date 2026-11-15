@file:Suppress("SpellCheckingInspection")

package com.example.infraguard// <-- PON AQUÍ EL MISMO PACKAGE DE TU PROYECTO

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
    val timestamp: String?
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
}

object RetrofitClient {
    // Apunta al puente portproxy de tu Windows en la red local
    private const val BASE_URL = "http://192.168.0.121:1880/"

    val api: InfraGuardApi by lazy {
        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(InfraGuardApi::class.java)
    }
}