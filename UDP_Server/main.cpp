#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

#define BUFLEN 1024  //Max length of buffer
#define PORT 8900   //The port on which to listen for incoming data

using namespace std;

int main()
{
//-----Iniciar Winsock--------------------------------------------------------------
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &data);
	if (wsOk != 0)
	{
		// cerr es mensaje de salida de error 
		cerr << "No se pudo iniciar Winsock" << endl;
		return -1;
	}

//-----Crear un listening socket----------------------------------------------------
	SOCKET listener = socket(AF_INET, SOCK_DGRAM, 0);
	if (listener == INVALID_SOCKET)
	{
		cerr << "Socket es invalido" << endl;
		return -1;
	}

//-----Bindear el socket (Atar el socket a una dupla ip:puerto / direccion de socket)
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT); // Puerto en el que vamos a escuchar
	hint.sin_addr.S_un.S_addr = ADDR_ANY;
	//inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // IP de loopback

	int bindresult = bind(listener, (sockaddr*)&hint, sizeof(hint));
	if (bindresult == INVALID_SOCKET)
	{
		cerr << "No se pudo hacer el bind" << endl;
		return -1;
	}

//-----Recibir data del socket y procesarla (proceso bloqueante)---------------------
	char buffer[BUFLEN];
	char serverAnswer[BUFLEN];
	// Estructura con la data del cliente que nos esta enviado mensajes
	sockaddr_in client;
	int clientSize = sizeof(client);

	// LOOP
	while (1)
	{
		fflush(stdout);

		//ZeroMemory(buffer, sizeof(buffer));
		memset(buffer, 0, sizeof(buffer));
		memset(serverAnswer, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		FD_SET fds;
		struct timeval tv;

		FD_ZERO(&fds);
		FD_SET(listener, &fds);

		tv.tv_sec = 0;
		tv.tv_usec = 30000;

		
		int n = select(listener, &fds, NULL, NULL, &tv);
		if (n > 0) {
			//Funcion bloqueante (el programa se queda en esta linea hasta que se ejecuta)
			int bytesIn = recvfrom(listener, buffer, BUFLEN, 0, (sockaddr*)&client, &clientSize);
			if (bytesIn == SOCKET_ERROR)
			{
				cerr << "Error al recibir data" << endl;
				return -1;
			}

			//print details of the client/peer and the data received
			//printf("Received packet from %s:%d\n", inet_ntop(client.sin_addr), ntohs(client.sin_port));
			cout << "Recibiendo " << bytesIn << " bytes." << endl;

			//cout << inet_ntoa(client.sin_addr) << ": " << buffer << endl;
			cout << "Mensaje: " << buffer << endl;
			for (size_t i = 0; i < sizeof(buffer); i++)
			{
				serverAnswer[i] = buffer[i];
			}
			//now reply the client with the same data
			if (sendto(listener, serverAnswer, BUFLEN, 0, (sockaddr*)&client, clientSize) == SOCKET_ERROR) {
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			cout << "Respuesta del servidor : " << serverAnswer << endl;
			//printf("\n %s", serverAnswer);
		}
		else if (n == 0) {
			printf("Waiting for data...\r");
		}
		else if (n < 0) {
			printf("error");
		}

		fflush(stdin);
	}

//-----Destruir el socket------------------------------------------------------------
	closesocket(listener);

//-----Cleanup Winsock---------------------------------------------------------------
	WSACleanup();

	return 0;
}