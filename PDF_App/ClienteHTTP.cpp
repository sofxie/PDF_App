#include "ClienteHTTP.h"
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "json.hpp"
#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;
using namespace std;
#define addr "127.0.0.1" // Direccion del server con PDF App
#define port 8080 // Puerto con server

void ClienteHTTP::run() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    string request, body;
    json j;

    // Inicializar WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error de inicialización: " << WSAGetLastError() << endl;
        return;
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cerr << "No se pudo crear el socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // Configurar dirección del servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addr);

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cerr << "No se pudo conectar al servidor: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Construir cuerpo JSON
    j["comando"] = "guardar";
    j["nombre"] = "documento.pdf";
    j["contenido"] = "Datos";
    body = j.dump();

    // Construir solicitud HTTP
    request = "POST / HTTP/1.1\r\n";
    request += "Host: " + string(addr) + ":" + to_string(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + to_string(body.length()) + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    request += body;

    // Enviar solicitud
    int totalSent = 0;
    int requestSize = request.length();
    while (totalSent < requestSize) {
        int sent = send(sock, request.c_str() + totalSent, requestSize - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            cerr << "Error al enviar la solicitud: " << WSAGetLastError() << endl;
            closesocket(sock);
            WSACleanup();
            return;
        }
        totalSent += sent;
    }

    // Recibir respuesta del servidor
    string response;
    char buffer[4096];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        response += buffer;
    }
    if (bytesReceived == SOCKET_ERROR) {
        cerr << "Error al recibir la respuesta: " << WSAGetLastError() << endl;
    }
    else {
        cout << "Respuesta del servidor:\n" << response << endl;

        // Parsear la respuesta JSON (opcional)
        size_t pos = response.find("\r\n\r\n");
        if (pos != string::npos) {
            string body = response.substr(pos + 4);
            try {
                json j_response = json::parse(body);
                cout << "JSON recibido: " << j_response.dump(2) << endl;
            }
            catch (json::parse_error& e) {
                cerr << "Error al parsear JSON de respuesta: " << e.what() << endl;
            }
        }
    }

    // Cerrar socket y limpiar
    closesocket(sock);
    WSACleanup();
}