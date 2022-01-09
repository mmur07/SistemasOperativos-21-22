#include <pthread.h>
#define N_PARADAS 5		// número de paradas de la ruta
#define EN_RUTA 0		// autobús en ruta
#define EN_PARADA 1		// autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4		// numero de usuarios
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0;   // ocupantes que tiene el autobús
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};
// Otras definiciones globales (comunicación y sincronización)
pthread_t bus;
pthread_t users[USUARIOS];
void *thread_autobus(void *args)
{
	while (1/*condicion*/)
	{
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	printf("El autobus ha llegado a la parada %d\n", parada_actual);
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	printf("El ha salido de la parada %d\n", parada_actual);
}

void *thread_usuario(void *arg)
{
	// obtener el id del usario
	int id_usuario = (int)arg;
	while (1)
	{
		int a = rand() % N_PARADAS;
		int b;
		do
		{
			b = rand() % N_PARADAS;
		} while (a == b);
		Usuario(id_usuario, a, b);
	}
}
void Usuario(int id_usuario, int origen, int destino)
{
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	printf("Usuario %d se ha subido al autobus en la parada %d.\n", id_usuario, origen);
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	printf("Usuario %d se ha bajado del autobus en la parada %d.\n", id_usuario, destino);
}

int main(int argc, char *argv[])
{
	int i;
	// Definición de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas

	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL);

	// Crear thread para el usuario i
	for (i = 0; i < USUARIOS; i++)
	{
		pthread_create(&users[i], NULL, thread_usuario, (void*)i);
	}

	// Esperar terminación de los hilos
	pthread_join(&bus, NULL);

	for (i = 0; i < USUARIOS; i++)
	{
		pthread_join(&users[i], NULL);
	}

	return 0;
}