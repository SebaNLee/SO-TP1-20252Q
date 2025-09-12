# Trabajo Practico 1 de Sistemas Operativos: ChompChamps
## 2do cuatrimestre 2025

**Aclaracion**: Para poder correr el trabajo es necesario el uso de Docker.

A continuación se detallan dentro de secciones los pasos para poder visualizar el trabajo práctico:

### 1. Docker
Para inicializar contenedor de Docker:
    ```./docker.sh``` 

Luego, en el contenedor:
    ```cd root```

### 2. Makefile
Antes de crear el archivo ejecutable es necesario correr:
    ```make clean```
y luego para crear el ejecutable:
    ```make all```

### 3. Correr ejecutable
Para correr el programa puede elegir entre usar el binario provisto por la cátedra: 

    ./ChompChamps_x86-64 -v ./bin/view -p ./bin/player
    
o el creado por los alumnos:

    ./bin/master -v ./bin/view -p ./bin/player

Además, hay 7 parámetros que acepta el ejecutable, aquellos que se encuentran entre corchetes son opcionales:
#### Altura:
Modifica la altura del tablero donde se produce el juego:```[-h tamaño]```

El tamaño deberá ser un numero entero mayor a 10. Su valor default es de 10.
#### Ancho:
Modifica el ancho del tablero donde se produce el juego:```[-w tamaño]```
 
El tamaño deberá ser un numero entero mayor a 10. Su valor default es de 10.
#### Delay:
Modifica la cantidad de milisegundos que se espera para imprimir el estado del juego:```[-d milisegundos]```
El tiempo deberá ser un numero mayor a cero. Su valor default es de 200.

#### Timeout:
Modifica la cantidad de segundos que se espera para recibir solicitudes de movimientos validos antes de terminar la partida: ```[-t segundos]```

El tiempo deberá ser un numero mayor a cero. Su valor default es de 10.


#### Semilla:
Modifica la semilla utilizada para la generación del tablero:```[-s semilla]```

La semilla deberá ser un numero mayor a cero. Su valor default es el correspondiente a time(NULL).


#### Vista:
Modifica la vista utilizada para la generación del tablero. Para esto se coloca la ruta del archivo binario de la vista:```[-v rutaDelBinario]```

Su default es sin vista.

#### Jugador/es:
Modifica los jugadores utilizada para la generación de la partida. Para esto se coloca la ruta del archivo binario de los jugadores. Es posible agregar desde 1 a 9 jugadores:
```-p rutaDelBinario1 rutaDelBinario2```

## Integrantes
**Nicole Belén Bertucci - Legajo 64415**

**Juan Ignacio Bridoux - Legajo 64092**

**Sebastián Nicolás Lee - Legajo 64675**


