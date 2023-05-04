# Grupo 29
## Integrantes:
- Lara Kurtz, lara.kurtz@mi.unc.edu.ar
- Lautaro Rodri­guez, lautaro.rodriguez@mi.unc.edu.ar

# Tarea Análisis

## Caso de estudio 1

| Conexión           | Datarate       | Delay  |
|--------------------|----------------|--------|
| `NodeTx` a `Queue` | **`1.0 Mbps`** | 100 us |
| `Queue` a `NodeRx` | **`1.0 Mbps`** | 100 us |
| `Queue` a `Sink`   | **`0.5 Mbps`** |        |

Vemos que hay un cuello de botella en NodeRx, ya que los paquetes llegan a
NodeRx a un rate mucho más rápido (**`1.0 Mbps`**) de lo que salen
(**`0.5 Mbps`**). Es decir que el buffer de NodeRx no se vacía lo
suficientemente rápido como para dar lugar a los paquetes entrantes.

Esto es un problema de **control de flujo**.

## Caso de estudio 2

| Conexión           | Datarate       | Delay  |
|--------------------|----------------|--------|
| `NodeTx` a `Queue` | **`1.0 Mbps`** | 100 us |
| `Queue` a `NodeRx` | **`0.5 Mbps`** | 100 us |
| `Queue` a `Sink`   | **`1.0 Mbps`** |        |

Vemos que hay un cuello de botella llegando Queue, desde NodeTx, ya que los
paquetes llegan a la Queue a un rate mucho más rápido (**`1.0 Mbps`**) del que
la Queue es capaz de procesarlos y enviarlos (**`0.5 Mbps`**). Es decir que el
la Queue no se vacía lo suficientemente rápido como para dar lugar a
los paquetes entrantes.

Esto es un problema de **control de congestión**.

## Preguntas

1. ¿Qué diferencia observa entre el caso de estudio 1 y 2?

    En el caso de estudio 1 tenemos un problema en los *end hosts*, mientras que en el caso de estudio 2 tenemos un problema en la *red*.

2. ¿Cuál es la fuente limitante en cada uno?
    - Caso 1: la fuente limitante es la velocidad a la que los paquetes son
    enviados desde el receptor (NodeRx) hacia el resumidero (Sink).

    - Caso 2: la fuente limitante es la capacidad de la red de procesar y hacer
    llegar a destino (NodeRx) los paquetes provenientes del emisor (NodeTx).
