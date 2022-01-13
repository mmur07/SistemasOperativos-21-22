#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define N_PARADAS 5		// número de paradas de la ruta
#define EN_RUTA 0		// autobús en ruta
#define EN_PARADA 1		// autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4		// numero de usuarios
// estado inicial
int estado = EN_PARADA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0;   // ocupantes que tiene el autobús
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};
// Otras definiciones globales (comunicación y sincronización)
pthread_t bus;
pthread_t *users[USUARIOS];
pthread_cond_t busWaiting;
pthread_cond_t busReady;
pthread_mutex_t lock;

void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	pthread_mutex_lock(&lock);
	printf("El autobus ha llegado a la parada %d\n", parada_actual);
	estado = EN_PARADA;
	while(esperando_bajar[parada_actual] > 0 || (esperando_parada[parada_actual] > 0) && n_ocupantes < MAX_USUARIOS){
		pthread_cond_broadcast(&busWaiting);
		pthread_cond_wait(&busReady, &lock);
	}
	printf("Todos los viajeros se han subido al bus en la parada %d\n", parada_actual);
	pthread_mutex_unlock(&lock);
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	sleep(3);
	printf("El bus se dirige a su destino...\n-------------------------\n");
	pthread_mutex_lock(&lock);
	estado = EN_RUTA;

	parada_actual = (parada_actual + 1) % N_PARADAS;
	pthread_mutex_unlock(&lock);
}

void *thread_autobus(void *args)
{
	while (1)
	{
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&lock);
	esperando_parada[origen]++;
	printf("El usuario %d esta esperando a subir en la parada %d.\n", id_usuario, origen);

	while(estado == EN_PARADA && parada_actual != origen && n_ocupantes < MAX_USUARIOS) {
		pthread_cond_wait(&busWaiting, &lock);
	}

	printf("Usuario %d se ha subido al autobus en la parada %d", id_usuario, origen);
	esperando_parada[origen]--;
	n_ocupantes++;

	if(esperando_bajar[origen] == 0 && esperando_parada[origen] == 0) pthread_cond_signal(&busReady);
	pthread_mutex_unlock(&lock);
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&lock);
	printf(" con destino %d.\n", id_usuario, destino);
	esperando_bajar[destino]++;

	while(estado == EN_PARADA && parada_actual != destino) pthread_cond_wait(&busWaiting, &lock);
	printf("Usuario %d se ha bajado del autobus en la parada %d.\n", id_usuario, destino);
	esperando_bajar[destino]--;
	n_ocupantes--;

	if (esperando_bajar[destino] == 0 && esperando_parada[destino] == 0)
	{
		pthread_cond_signal(&busReady);
	}

	pthread_mutex_unlock(&lock);
}

void Usuario(int id_usuario, int origen, int destino)
{
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
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

int main(int argc, char *argv[])
{
	int i;
	// Definición de variables locales a main
	pthread_cond_init(&busWaiting, NULL);
	pthread_cond_init(&busReady, NULL);
	pthread_mutex_init(&lock, NULL);
	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL);
	// Crear thread para el usuario i
	for (i = 0; i < USUARIOS; i++)
	{
		pthread_create(&users[i], NULL, thread_usuario, (void*)i);
	}
	// Esperar terminación de los hilos
	pthread_join(bus, NULL);

	for (i = 0; i < USUARIOS; i++)
	{
		pthread_join(users[i], NULL);
	}

	return 0;
}