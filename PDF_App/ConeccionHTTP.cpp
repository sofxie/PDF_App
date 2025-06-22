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
    struct sockaddr_in server {};
    string request, body;
    json j;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error de inicialización de Winsock.\n";
        return;
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cerr << "No se pudo crear el socket.\n";
        WSACleanup();
        return;
    }

    // Configurar dirección del servidor
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addr);

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cerr << "No se pudo conectar al servidor.\n";
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Construir cuerpo JSON
    j["comando"] = command;
    j["nombre"] = name;
    j["contenido"] = path;

    body = j.dump(); // Convertir JSON a string

    // Construir solicitud HTTP
    request = "POST / HTTP/1.1\r\n";
    request += string("Host: ") + addr + ":" + to_string(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + to_string(body.length()) + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    request += body;

    // Enviar solicitud
    if (send(sock, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        cerr << "Error al enviar la solicitud.\n";
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Recibir respuesta
    string response;
    char buffer[4096];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';  
        cout << buffer;
    }

    if (bytesReceived < 0) {
        cerr << "Error al recibir la respuesta.\n";
    }
    else {
        // Imprimir respuesta completa
        cout << "Respuesta completa del servidor:\n" << response << "\n";

        // Extraer y parsear body JSON
        size_t pos = response.find("\r\n\r\n");
        if (pos != string::npos) {
            string bodyPart = response.substr(pos + 4);
            try {
                json respuesta_json = json::parse(bodyPart);
                cout << "Respuesta JSON:\n" << respuesta_json.dump(4) << "\n";
            }
            catch (json::parse_error& e) {
                cerr << "No se pudo parsear el cuerpo como JSON: " << e.what() << "\n";
            }
        }
        else {
            cerr << "Encabezado HTTP mal formado.\n";
        }
    }

    // Cerrar conexión
    closesocket(sock);
    WSACleanup();
}