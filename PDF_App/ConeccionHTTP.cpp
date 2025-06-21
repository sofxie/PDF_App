#include "ConeccionHTTP.h"
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "json.hpp"
#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;
using namespace std;
#define addr "127.0.0.1" // Direccion del server con PDF App
#define port 8080 // Puerto con server

void ConeccionHTTP::run(std::string command, std::string name, std::string path){
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    string request, body;
    json j;

    // Inicializar
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error de inicialización";
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cerr << "No se pudo crear el socket.";
        WSACleanup();

    }

    // Configurar dirección del servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addr);

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cerr << "No se pudo conectar al servidor.";
        closesocket(sock);
        WSACleanup();
    }

    // Construir cuerpo JSON
    j["comando"] = command;
    j["nombre"] = name;
    j["contenido"] = path;

    body = j.dump();  // Convertir JSON a string

    // Construir encabezado HTTP
    request = "POST / HTTP/1.1\r\n";
    request += "Host: " addr ":" + to_string(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + to_string(body.length()) + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    request += body;

    // Enviar solicitud
    if (send(sock, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        cerr << "Error al enviar la solicitud." << endl;
        closesocket(sock);
        WSACleanup();
    }

    // Recibir respuesta del servidor
    char buffer[4096];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';  
        cout << buffer;
    }

    if (bytesReceived < 0) {
        cerr << "Error al recibir la respuesta." << endl;
    }

    // Cerrar socket y limpiar Winsock
    closesocket(sock);
    WSACleanup();
}