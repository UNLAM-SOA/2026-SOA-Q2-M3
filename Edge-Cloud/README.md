# Edge / Cloud

Este directorio contiene los archivos relacionados con los servicios que se ejecutan en las capas Edge o Cloud del sistema.

Puede incluir:

- Archivos de Docker.
- Servidores.
- Flujos de Node-RED.
- Bases de datos.
- Configuración de brokers.


## Requisitos para la puesta en marcha

* [Docker](https://www.docker.com/) y Docker Compose instalados.
* Entorno de terminal compatible (WSL2 en Windows, Linux o macOS).

## Puesta en Marcha

### 1. Levantar la Infraestructura
Clona este repositorio, abrir una terminal en la raíz del proyecto y ejecutar el siguiente comando para construir e iniciar los contenedores en segundo plano:

```bash
docker compose up -d --build
```

### 2. Configurar la Lógica en Node-RED
Node-RED utiliza un volumen de Docker aislado. Debes importar el flujo de trabajo la primera vez:

1. Ingresa a http://localhost:1880 en tu navegador.
2. Click en el menu superior derecho (tres líneas horizontales) > Import.
3. Pega el contenido completo del archivo Node-Red/flows.json (incluido en este repositorio) y haz clic en Import.
4. Presiona el botón rojo Deploy en la esquina superior derecha.
5. El sistema ya está operativo. Node-RED se conectará internamente a la base de datos y al broker MQTT de forma automática.

### 3. Monitoreo
Una vez que la infraestructura está corriendo, utilizar los siguientes comandos en tu terminal para supervisar el estado, ver errores y visualizar el tráfico en tiempo real.

Para comprobar que los contenedores infraguard_db, infraguard_mqtt e infraguard_nodered están activos (en estado Up):
```bash
docker compose ps
```

Para ver qué está sucediendo dentro de cada contenedor (presiona Ctrl + C para salir del modo de visualización):
Ver logs de Node-RED:
```bash
docker compose logs -f nodered
```

Ver logs de Mosquitto MQTT
```bash
docker compose logs -f mosquitto
```

Ver logs de PostgreSQL
```bash
docker compose logs -f postgres
```

Si necesitas verificar directamente si un usuario o tarjeta se guardó correctamente sin depender de Node-RED o pgAdmin
```bash
docker exec -it infraguard_db psql -U postgres -d Rack_Smart
```
Una vez en esta consola, tiramos consultas SQL normales y con \q salimos del proceso.

### 4. Detener servicios
Para apagar el sistema de forma segura conservando absolutamente todos los datos, usuarios, credenciales y flujos guardados:
```bash
docker compose down
```

Para reiniciar el sistema:
```bash
docker compose up -d
```

# Pruebas Postman
### POST
```/api/remoto/autorizar --> Envia apertura remota y desactiva la alarma. Devuelve 200 OK```

```/api/tarjetas --> Da de alta un usuario con UID/HASH RFID. Devuelve 200 OK```

```/api/accesos --> Inserta manualmente un acceso. Devuelve 201 CREADO```

### GET
```/api/clima --> Devuelve la ultima lectura del DHT11. Devuelve 200 OK```

```/api/tarjetas --> Lista todos los usuarios. Devuelve 200 OK```

```/api/usuarios/por-tarjeta --> Valida si un hash especifico existe. Devuelve 200 OK o 400 o 404```

```/api/accesos --> Obtener el historial completo de autorizados y denegados. Devuelve 200 OK```

### PUT
```/api/usuarios/estado --> Habilita o deshabilita el permiso de un usuario. Devuelve 200 OK```



