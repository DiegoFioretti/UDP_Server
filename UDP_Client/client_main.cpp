#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
    // NetCat:  .\nc.exe -ul -p8900
    // send.exe 127.0.0.1 8900 "mensaje de texto"
    // Programa debe recibir IP / Puerto / Mensaje
    WSADATA data;
    WORD version = MAKEWORD(2, 2);

    // Inicializar winsock
    int isOk = WSAStartup(version, &data);
    if (isOk != 0)
    {
        // Avisa si hubo un error
        std::cout << "Cant start!" << isOk << std::endl;
        return -1;
    }

    // Indicar IP y puerto de destino
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8900); // Puerto de destino
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // IP de destino

    // Crear un socket, UDP
    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    // Enviar data a traves del socket
    std::string msg = std::string("hola");
    int sendOk = sendto(out, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&server, sizeof(server));
    if (sendOk == SOCKET_ERROR)
    {
        std::cout << "Hubo un error al enviar! " << sendOk << std::endl;
        return -1;
    }

    // Cerrar el socket
    closesocket(out);

    // Cerrar winsock
    WSACleanup();
}