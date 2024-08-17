
# Manual de Usuario
## Compilar el programa
Para compilar el programa se ha facilitado un Makefile ubicado en la carpeta raíz del proyecto. Se debe abrir una terminal en esta carpeta y escribir el comando `make` para que este realice su trabajo. El ejectuable quedará en la ubicación `bin/`  del directorio raíz del proyecto.

> NOTA: También se facilitó un `make clean` para limpiar la carpeta generada para el ejecutable.

Esta solución está implementada con la API de MPI y por lo tanto  no compilará si no se encuentra instalada en su sistema. Para instalar esta biblioteca debe ejecutar en la terminal el siguiente comando:
`sudo apt install mpich`
>NOTA: Esto ejemplo es utilizando el manejador de paquetes de Ubuntu, si su sistema tiene otro manejador de paquetes entonces debe reemplazar el `apt` por el comando de su manejador correspondiente.

## Utilizar el programa
Para ejectuar el programa nada más es de correrlo desde la terminal. Si se encuentra en el directorio raíz del proyecto, basta con escribir en la terminal `mpirun bin/super_mario_mpi <proceso a enfocar> <estrategia de ataque>` y esto comenzará la ejecución del programa.

>NOTA: `<proceso a enfocar>` se refiere al jugador con el que se quiere comenzar y `<estrategia de ataque>` es una de las siguientes estrategias: R, L, M, A (RANDOM, LESS_COINS, MORE_COINS, ATTACKER). 

Durante la simulación en todo momento se estará mostrando en pantalla lo que el jugador enfocado está realizando. Si mata a un objetivo entonces se mostrará el evento en forma de string, al igual que si el personaje (de moment Mario solamente) muere. Una vez que el jugador muere se le indicará al usuario que debe ingresar otro proceso para enfocar. La simulación continuará de esta forma hasta que se decida un ganador.

# Detalles de Implementación
Esta implementación es completamente orientada a objetos y se hace aprovechamiento total de sus features, la herencia y polimorfismo. Todos los objetos y clases están bajo el namespace de SM (Super Mario). Esto para facilitar la comprensión del código.
## Lógica de Gameplay
### SM::Object
Todos los objetos del juego con los que el SM::Character interactúa son de tipo SM::Object, una clase abstracta con métodos puros. Los huecos, monedas, little goomba y koopa troopa son clases que heredan de SM::Object. Estos simplemente cambian su nombre y el evento que retornan a la hora de que el SMCharacter interactúa con el objeto.
### SM::World
Internamente contiene SM::WorldCells, que representan cada celda del nivel. Los niveles son leídos de un archivo .txt con un formato específico en donde los objetos van a estar indicados con un char y el final de la casilla con un `;`. SM::World contiene un `std::deque` de SM::WorldCells y va iterando por ellos hasta llegar al último. Cuando esto sucede, se devuelve al inicio. Cada SM::WorldCell tiene un `std::deque` y `std::queue` para el manejo de los SM::Objects que pueden contener. El deque está reservado solamente para los objetos default del nivel, es decir, los que se leen del archivo. La cola está reservada para los enemigos que vienen de ataques de otros jugadores, y eston se van colocando 10 casillas adelante de la casilla actual.
### SM::Character
El SM::Character es el jugador el personaje que el jugador desea utilizar, en este caso es una clase abstracta con la que se construyen los personajes como tal. La clase de Mario deriva de esta y se hizo de esta manera pensando en que en un futuro se podrían agregar más personajes con facilidad. Este personaje tiene un conteo de monedas y aunque va a recorrer el SM::World, este no tiene contacto directo con él, pues el personaje solo interactúa con SM::Objects.
## Actores de comunicación
Esta implementación utiliza un pequeño sistema de Signals y Requests. De esta forma los jugadores se mandan información entre ellos y el controlador. Este sistema se construyó con `MPI_Requests, MPI_Test, MPI_Send, MPI_Irecv`. De esta forma el proceso y los jugadores pueden saber si se les ha enviado una señal o un request y de esta forma actuar a ellos. Las siguientes clases contienen métodos `handlers` y `senders` para reaccionar y enviar los mensajes necesarios.
### SM::Player
El SM::Player ( procesos 1...n ) es lo que representa a un jugador del juego. Cada jugador posee su propio SM::World y SM::Player.  Internamente, este coordina la interacción entre ellos. Además de esto, tiene métodos para enviar y recibir enemigos de otros SM::Players, así como realizar requests al SM::Controller para recibir información de las estrategias y enviar señales para comunicar qué está sucediendo en dicho proceso. Una vez que el jugador del proceso queda fuera de la partida, el proceso muere y paran todas las comunicaciones que salen de él y lógicamente se impide la comunicación hacia él. En otras palabras, el proceso termina su ejecución y se llama el destructor de su SM::World y SM::Player internos para liberar la memoria.
### SM::Controller
El SM::Controller es el núcleo de la comunicación, pues este recibe y manda toda la información necesaria a los jugadores para coordinarlos. El controller sabe qué jugadores siguen vivos, cuántas monedas tienen y a quién están atacando. Cuando se tienen menos de dos jugadores activos, el controlador manda señal de GameOver y se acaba la ejecución de la simulación anunciando al ganador.
> NOTA: Lá única interacción que el controlador no maneja es la de mandar enemigos de un proceso a otro. Los jugadores se lo comunican entre ellos.
# Métodos de Comunicación de MPI
Los métodos de sincronización utilizados son los siguientes:

- **MPI_Send**: Se utiliza para mandar los mensajes entre jugadores y el controlador: los requests y signals.
- **MPI_Irecv**: El principal método para recibir los MPI_Sends, pues permite no bloquear la ejecución del proceso y revisar si ya se recibió un mensaje y debe mandar una respuesta.
- **MPI_Recv**: En algunas ocaciones es necesario asegurarse de esperar hasta recibir ciertos requests antes de  proceder con la ejecución. Por ejemplo, el proceso no puede mandar sus enemigos de ataque a menos de que se actualice el jugador al que debe atacar, así como el número de procesos activos para saber si el proceso debe continuar con la ejecución.
- **MPI_Test**: De esta manera se revisa si se debe atender algún MPI_Request que se generó para escuchar los requests y signals entre los procesos.
