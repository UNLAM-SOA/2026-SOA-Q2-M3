@file:Suppress("SpellCheckingInspection")

package com.example.infraguard

import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import java.util.Locale
import kotlin.time.Duration.Companion.seconds
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                InfraGuardApp()
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InfraGuardApp() {
    val scope = rememberCoroutineScope()
    val context = LocalContext.current

    var tabIndex by remember { mutableIntStateOf(0) }
    var clima by remember { mutableStateOf(ClimaResponse(0f, 0f, false, null)) }
    var accesos by remember { mutableStateOf<List<AccesoItem>>(emptyList()) }
    var tarjetas by remember { mutableStateOf<List<TarjetaUsuarioItem>>(emptyList()) }
    var errorRed by remember { mutableStateOf<String?>(null) }

    var nuevoNombre by remember { mutableStateOf("") }
    var nuevoHash by remember { mutableStateOf("") }

    // Consultas separadas para que un fallo en una no bloquee las demás
    LaunchedEffect(Unit) {
        while (true) {
            try {
                clima = RetrofitClient.api.obtenerClima()
                errorRed = null
            } catch (e: Exception) {
                errorRed = e.localizedMessage ?: e.javaClass.simpleName
            }

            try {
                accesos = RetrofitClient.api.obtenerAccesos()
            } catch (_: Exception) { }

            try {
                tarjetas = RetrofitClient.api.obtenerTarjetas()
            } catch (_: Exception) { }

            delay(2.seconds)
        }
    }

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("InfraGuard Control Center", fontWeight = FontWeight.Bold) }
            )
        }
    ) { padding ->
        Column(modifier = Modifier.padding(padding)) {
            PrimaryTabRow(selectedTabIndex = tabIndex) {
                Tab(selected = tabIndex == 0, onClick = { tabIndex = 0 }, text = { Text("Monitor") })
                Tab(selected = tabIndex == 1, onClick = { tabIndex = 1 }, text = { Text("Accesos") })
                Tab(selected = tabIndex == 2, onClick = { tabIndex = 2 }, text = { Text("Tarjetas") })
            }

            // Banner de diagnóstico en vivo
            if (errorRed != null) {
                Card(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp, vertical = 8.dp),
                    colors = CardDefaults.cardColors(containerColor = Color(0xFFFFEBEE))
                ) {
                    Text(
                        text = "⚠️ Sin conexión a Node-RED: $errorRed",
                        color = Color(0xFFB71C1C),
                        fontSize = 13.sp,
                        fontWeight = FontWeight.SemiBold,
                        modifier = Modifier.padding(12.dp)
                    )
                }
            }

            when (tabIndex) {
                0 -> Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(16.dp)
                ) {
                    Card(modifier = Modifier.fillMaxWidth()) {
                        Column(modifier = Modifier.padding(20.dp)) {
                            Text(
                                text = "Telemetría del Rack (DHT11)",
                                fontSize = 18.sp,
                                fontWeight = FontWeight.Bold
                            )
                            Spacer(modifier = Modifier.height(12.dp))
                            Text(
                                text = "${String.format(Locale.US, "%.1f", clima.temperatura)} °C",
                                fontSize = 42.sp,
                                fontWeight = FontWeight.ExtraBold,
                                color = if (clima.coolerActivo) Color(0xFFD32F2F) else Color(0xFF388E3C)
                            )
                            Text(
                                text = "Humedad relativa: ${String.format(Locale.US, "%.1f", clima.humedad)} %",
                                fontSize = 16.sp
                            )
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = if (clima.coolerActivo) {
                                    "❄️ COOLER ENCENDIDO (> 30°C)"
                                } else {
                                    "✅ COOLER EN REPOSO (<= 30°C)"
                                },
                                fontWeight = FontWeight.SemiBold
                            )
                            if (clima.timestamp != null) {
                                Spacer(modifier = Modifier.height(4.dp))
                                Text(
                                    text = "Última lectura: ${clima.timestamp!!.take(19).replace("T", " ")}",
                                    fontSize = 12.sp
                                )
                            }
                        }
                    }

                    Button(
                        onClick = {
                            scope.launch {
                                try {
                                    RetrofitClient.api.autorizarRemoto()
                                    Toast.makeText(
                                        context,
                                        "Acceso concedido / Alarma desarmada",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                } catch (_: Exception) {
                                    Toast.makeText(
                                        context,
                                        "Error de conexión con Node-RED",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                }
                            }
                        },
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(56.dp)
                    ) {
                        Text("🔓 HABILITAR APERTURA / SILENCIAR ALARMA", fontWeight = FontWeight.Bold)
                    }
                }

                1 -> LazyColumn(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    items(accesos) { item ->
                        val autorizado = item.estadoAutorizacion == "AUTORIZADO"
                        Card(
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable {
                                    if (!autorizado) {
                                        nuevoHash = item.hashTarjeta
                                        tabIndex = 2
                                        Toast.makeText(
                                            context,
                                            "Hash copiado. Ingresa el nombre para autorizar.",
                                            Toast.LENGTH_SHORT
                                        ).show()
                                    }
                                }
                        ) {
                            Row(
                                modifier = Modifier
                                    .padding(16.dp)
                                    .fillMaxWidth(),
                                horizontalArrangement = Arrangement.SpaceBetween,
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Column {
                                    Text(
                                        text = item.nombreUsuario ?: "Tarjeta no registrada",
                                        fontWeight = FontWeight.Bold,
                                        fontSize = 16.sp
                                    )
                                    Text("UID: ${item.hashTarjeta}", fontSize = 13.sp)
                                    Text(
                                        text = item.timestampAcceso.take(19).replace("T", " "),
                                        fontSize = 12.sp
                                    )
                                }
                                Text(
                                    text = item.estadoAutorizacion,
                                    color = if (autorizado) Color(0xFF388E3C) else Color(0xFFD32F2F),
                                    fontWeight = FontWeight.Bold
                                )
                            }
                        }
                    }
                }

                2 -> Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Text("Registrar o Actualizar Tarjeta", fontWeight = FontWeight.Bold, fontSize = 18.sp)
                    OutlinedTextField(
                        value = nuevoNombre,
                        onValueChange = { nuevoNombre = it },
                        label = { Text("Nombre del Usuario (ej. Junquera Santiago)") },
                        modifier = Modifier.fillMaxWidth()
                    )
                    OutlinedTextField(
                        value = nuevoHash,
                        onValueChange = { nuevoHash = it },
                        label = { Text("UID / Hash RFID (ej. c3e2cba9)") },
                        modifier = Modifier.fillMaxWidth()
                    )
                    Button(
                        onClick = {
                            scope.launch {
                                try {
                                    RetrofitClient.api.registrarTarjeta(
                                        NuevaTarjetaRequest(nuevoNombre, nuevoHash)
                                    )
                                    nuevoNombre = ""
                                    nuevoHash = ""
                                    tarjetas = RetrofitClient.api.obtenerTarjetas()
                                    Toast.makeText(
                                        context,
                                        "Tarjeta registrada con éxito",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                } catch (_: Exception) {
                                    Toast.makeText(
                                        context,
                                        "Error al guardar tarjeta",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                }
                            }
                        },
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Text("Guardar y Autorizar Tarjeta")
                    }

                    HorizontalDivider(modifier = Modifier.padding(vertical = 8.dp))
                    Text("Usuarios Registrados (Switch = Permitir / Denegar)", fontWeight = FontWeight.Bold)

                    LazyColumn(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                        items(tarjetas) { t ->
                            Card(modifier = Modifier.fillMaxWidth()) {
                                Row(
                                    modifier = Modifier
                                        .padding(16.dp)
                                        .fillMaxWidth(),
                                    horizontalArrangement = Arrangement.SpaceBetween,
                                    verticalAlignment = Alignment.CenterVertically
                                ) {
                                    Column {
                                        Text(
                                            text = t.nombreUsuario,
                                            fontWeight = FontWeight.Bold,
                                            fontSize = 16.sp
                                        )
                                        Text("UID: ${t.hashTarjeta}", fontSize = 13.sp)
                                    }
                                    Switch(
                                        checked = t.estado,
                                        onCheckedChange = { nuevoEstado ->
                                            scope.launch {
                                                try {
                                                    RetrofitClient.api.cambiarEstadoUsuario(
                                                        CambiarEstadoRequest(t.idUsuario, nuevoEstado)
                                                    )
                                                    tarjetas = RetrofitClient.api.obtenerTarjetas()
                                                } catch (_: Exception) { }
                                            }
                                        }
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}