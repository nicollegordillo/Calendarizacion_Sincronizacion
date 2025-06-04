# Simulador de Calendarizacion y Sincroizacion
Esta es una aplicacion de escritorio desarrollada en C++ utilizando el framework de Qt para simular y visualizar algoritmos de calendarizacion y sincronizacion de procesos en sistemas operativos. Esta es una herramienta educativa e interactiva que permite la comprensionvisual y dinamica de los algoritmos principales que conforman los sistemas actuales
## Uso 
### Carga de informacion
La carga de informacion se realiza por medio de archivos de texto (con terminacion ```.txt```)que presentan la siguiente estructura en formato de linea, dependiendo de lo que requiera el tipo de simulacion (calendarizacion o sincronizacion).
- Procesos: ```<PID>, <BT>, <AT>, <Priority>```
- Recursos: ```<RECURSO>, <CONTADOR>```
- Acciones: ```<PID>, <ACCION>, <RECURSO>, <CICLO>```
Asimismo, cada campo de entrada, junto al boton de seleccionar, incluye un boton ```ver``` que permite visualizar los contenidos del archivo
### Calendarizacion
La aplicacion desarrollada comprende de dos vistas principales, las cuales pueden ser accedidas presionando el switch en la parte superior de la pagina que cambia entre la vista de Calendarizacion y la vista de Sincronizacion. Cada uno de los modos de simulacion presenta los diferentes apartados
- Configuracion: como entrada un archivo de procesos
- Algoritmo: permite la eleccion de 1 o varios algoritmos de calendarizacion, desplegando el campo de ingreso de quantum para el algoritmo Round Robin
- Simulacion: una vez ya cargados los datos, se permite generar un simulador, el cual permite avanzar, finalizar o reiniciar la visualizacion de los procesos mostrando siempre el ciclo actual. Asimismo, una vez finalizada la calendarizacion, se permite observar las metricas de eficiencia de este.
### Sincronizacion
- Configuracion: como entrada recibe archivos correspondientes a los procesos, acciones y recursos
- Algoritmo: permite la eleccion entre sincronizacion por mutex o por semaforo.
- Simulacion: una vez ya cargados los datos, se permite generar un simulador, el cual permite avanzar y reiniciar la visualizacion de los procesos mostrando siempre el ciclo actual. Asimismo, cada visualizador muestra el proceso, la accion y esta denotado en color gris si esta en espera.
### Simulador
El simulador es una linea de tiempo basada en diagramas de Gantt para mostrar la ejecucion o acciones de cada proceso, cateogrizado por el ciclo actual. Cada proceso es asignado un color unico para diferenciarlos y muestran siempre el nombre o PID del mismo. Para el caso de sincronizacion, se muestra tambien el recurso al que se desea acceder, la accion y lo procesos con color gris son aquellos que estan a espera de la liberacion de su recurso.
## Documentacion
- ```scheduler.h / scheduler.cpp```
mplementa la lógica de planificación. Tiene métodos para cada algoritmo: 

  - ```nextFIFO()```: planificación sin prioridades. 

  - ```nextSJF()```: selecciona el proceso con menor burst. 

  - ```nextSRT()```: versión preemptive de SJF. 

  - ```nextRR()```: Round Robin con quantum configurable. 

  - ```nextPS()```: planificación por prioridad. 

Variables como timeline, t, queue y finished permiten rastrear y controlar la ejecución del algoritmo en tiempo. 

- ```processes.h / processes.cpp ```
 Carga desde archivo los procesos y los convierte en vectores de atributos (names, burstTime, etc.). Genera colores únicos usando HSV para cada proceso con genColor. 

- ```mutexsync.h / mutexsync.cpp ```
Simula acceso a recursos con exclusión mutua. 
  - ```simulateNext()```: avanza un ciclo simulando las acciones de los procesos. 
  - Estados posibles: ACCESSED (logra acceder al recurso), WAITING (debe esperar). 

- ```semaphore.h / semaphore.cpp ```
Simula acceso a recursos basado en la sincronizacion con semaforos. 
  - ```simulateNext()```: avanza un ciclo simulando las acciones de los procesos. 
  - Estados posibles: ACCESSED (logra acceder al recurso), WAITING (debe esperar). 

- ```actions.h / actions.cpp ```
Carga desde archivo las acciones que cada proceso debe intentar realizar sobre un recurso en un ciclo determinado. 

- ```resources.h / resources.cpp ```
Interpreta y guarda la cantidad disponible de cada recurso desde el archivo de recursos. 

- ```mainwindow.h / mainwindow.cpp ```
Controla la interfaz gráfica. Permite seleccionar algoritmos, cargar archivos, mostrar la simulación paso a paso o completa. 
Usa QScrollArea para permitir visualización dinámica. 
Maneja la lógica de simulación de calendarización y sincronización por separado 
