#include "MySocket.h"
#include <thread>
#include <vector>
#include <SDL2/SDL.h>
#include "Cola.h"
#include "User.h"
#include "Interprete.h"

const unsigned int MAX_NUM_CLIENTS = 8;
using namespace std;

struct thread_data{
	MySocket*  newClient;
	Cola* colaEventos;
	SDL_mutex *mutex;
	User* user;
	Interprete* interprete;
	bool reconnect;
};

struct thread_notifier_data{
	MySocket*  newClient;
	User* user;
	Interprete* interprete;
};

struct thread_ppal_data{
	Cola* colaEventos;
	vector<User*> users;
	Interprete* interprete;
};

struct cola_data{
	User* senderIP;
	string* evento;
};

void enviarKeepAlive(MySocket* myClient, Interprete* interprete){

	string messageFromServer = "";

	string messageToServer = interprete->getKeepAliveMsg();

	myClient->sendMessage(messageToServer);

	myClient->recieveMessage(messageFromServer);
}

void agregarACola(Cola* colaEventos,string* evento,User* client, SDL_mutex *mutex){

	struct cola_data cola_data;

	cola_data.senderIP =  client;
	cola_data.evento =  evento;

	if (SDL_LockMutex(mutex) == 0) {
		//ver de hacer el copy de evento

		colaEventos->push(&cola_data);
		SDL_UnlockMutex(mutex);
	} else {
	  fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void enviarNotificaciones(void *threadArg){
	struct thread_notifier_data* data;
	string verification,notificacion;

	data = (struct thread_notifier_data*) threadArg;

	MySocket* socket = data->newClient;
	User* user = data ->user;
	Interprete* interprete = data ->interprete;

	while( (user->isColaVacia() == false) && (socket->isConnected())){
		 notificacion = user->popNotifiacion();

		 socket->sendMessage(notificacion);
		 socket->recieveMessage(verification);
	}

}

void enviarConfirmReceived(MySocket* myClient, Interprete* interprete){
	string msgToSrv = interprete->getReceivedMsg();
	myClient->sendMessage(msgToSrv);
}
void establecerLogin(MySocket* socket, User* user, Interprete* interprete){
	string msgFromClient = "";

	socket->recieveMessage(msgFromClient);
	if (socket->isConnected()){
		enviarConfirmReceived(socket,interprete);
		interprete->postLoginMsg(msgFromClient, user);
	}

}

void* acceptedClientThread(void *threadArg ){

	struct thread_data *my_data;
	string messageFromClient, messageToClient;

	/* Recive argumentos */
	my_data = (struct thread_data *) threadArg;
	MySocket* socket = my_data ->newClient;
	Cola* colaEventos = my_data -> colaEventos;
	SDL_mutex *mutex = my_data -> mutex;
	User* user = my_data -> user;
	Interprete* interprete = my_data ->interprete;

	establecerLogin(socket,user,interprete);

	/* Abre thread notificador para este user */

	//pthread_t tNotifierClient;
	struct thread_notifier_data threadNotifierArg;

	threadNotifierArg.user = user;
	threadNotifierArg.newClient = socket;
	threadNotifierArg.interprete = interprete;

//pthread_create(&tNotifierClient, NULL, notifierClientThread,(void *) &threadNotifierArg);

	/* Tareas del thread Este que obtiene mensajes del client */

	if(my_data->reconnect == true){
		//notificar reconexion - desfreeze
	}else{
		//notificar aparicion
	}

	messageFromClient= "";

	enviarNotificaciones((void *)&threadNotifierArg);

   while ( (!interprete->isQuit(messageFromClient)) && (socket->isConnected())){

	  enviarKeepAlive(socket, interprete);

	  messageFromClient= "";
	  socket->recieveMessage(messageFromClient);
	  printf("%s \n", messageFromClient.c_str());
	  if (socket->isConnected()){

		  agregarACola(colaEventos,&messageFromClient,user, mutex);

		  enviarConfirmReceived(socket,interprete);
	  }
    }
   user->setConnectedFlag(false);
   //pthread_exit(&tNotifierClient);
   pthread_exit(NULL);
}



void serverHandleThread(void* threadArgPpal){

	struct thread_ppal_data* ppal_data;
	ppal_data = (struct thread_ppal_data*) threadArgPpal;

	Cola* colaEventos = ppal_data ->colaEventos;
	vector<User*> users = ppal_data -> users;
	Interprete* interprete = ppal_data -> interprete;

	MySocket myServer(PORTNUM);

	myServer.bindSocket();
	myServer.listenToClient();

	pthread_t tAcceptedClient [MAX_NUM_CLIENTS];

	struct thread_data threadArg[MAX_NUM_CLIENTS];

	SDL_mutex *mutex;
	mutex = SDL_CreateMutex();
	if (!mutex) {
		fprintf(stderr, "Couldn't create mutex\n");
	}

	unsigned int counter = 0;
	while ( counter < MAX_NUM_CLIENTS )
	{
	   string clientIP;//me devuelve la IP del cliente que se conecta en "clientIP"
	   MySocket* newClient = myServer.acceptClient(clientIP); //conexion dedicada al nuevo cliente

	   User* user;
	   bool incCounter = false;
	   for (unsigned int i = 0; i <= counter; i++){
		   User* tempUser = users [i];
	   		if (tempUser != NULL) {
	   			if(tempUser->clientIP.compare(clientIP) == 0){
	   				tempUser->setConnectedFlag(true); //ojo con thread notificador que hay que volverlo a encender
	   				user = tempUser;
	   				threadArg[counter].reconnect = true;
	   			}
	   		}else{
	   			//ojo aca si hay que mandar todo el YAML o no
	   			user = new User(clientIP);
	   			users [i] = user;
	   			incCounter = true;
	   			threadArg[counter].reconnect = false;
	   		}
	   	}
	   if (incCounter) counter = counter + 1;

	   threadArg[counter].colaEventos = colaEventos;
	   threadArg[counter].newClient = newClient;
	   threadArg[counter].mutex = mutex;
	   threadArg[counter].user = user;
	   threadArg[counter].interprete = interprete;

	   pthread_create(&tAcceptedClient[counter], NULL, acceptedClientThread,(void *) &threadArg[counter]);

	}
	SDL_DestroyMutex(mutex);
}

void simularEventosEnCola(Cola* colaEventos){
	struct cola_data cola_dato;
	double tiempo_actual,tiempo_viejo=0;
	tiempo_viejo=SDL_GetTicks();

	while (1){
		while(!colaEventos->estaVacia()){
			 cola_dato = *(struct cola_data*)colaEventos->pop(); //tengo en cola_dato el Ip del cliente que envio el evento y el mensaje
		}
		 usleep((40 - (tiempo_actual-tiempo_viejo))*1000);
		 tiempo_actual= SDL_GetTicks();
		 tiempo_viejo=tiempo_actual;
	}

}

int main(int argc, char *argv[]) {

	Cola colaEventos;
	Interprete interprete;

	vector<User*> users(MAX_NUM_CLIENTS) ;

	struct thread_ppal_data  threadArgu;

	threadArgu.colaEventos = &colaEventos;
	threadArgu.users = users;
	threadArgu.interprete = &interprete;

	thread tServer(serverHandleThread, (void*)&threadArgu );

	simularEventosEnCola(&colaEventos);

	tServer.join();

	for (unsigned int i= 0; i < users.size(); i++){
		if (users [i]) delete users [i];
	}

	return 0;
}
