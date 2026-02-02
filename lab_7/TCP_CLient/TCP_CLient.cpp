#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Can't initialize winsock! Quitting" << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Can't create socket! Quitting" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverHint;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &serverHint.sin_addr);

    cout << "Connecting to server at 127.0.0.1:54000..." << endl;

    if (connect(sock, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
        cerr << "Can't connect to server! Error: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server. Type messages (use '#exit' to close connection, '#shutdown' to stop server):" << endl;

    string userInput;
    while (true) {
        cout << "> ";
        getline(cin, userInput);

        if (userInput.empty()) {
            continue;
        }

        int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendResult == SOCKET_ERROR) {
            cerr << "Error sending message to server. Disconnecting." << endl;
            break;
        }

        // Если пользователь ввёл команду #exit или #shutdown — не ждём ответ
        if (userInput == "#exit" || userInput == "#shutdown") {
            if (userInput == "#exit") {
                cout << "Closing connection..." << endl;
                break;
            }
            if (userInput == "#shutdown") {
                cout << "Server should shut down now. Closing connection." << endl;
                break;
            }
        }

        // Ждём ответ от сервера
        char serverResponse[4096];
        ZeroMemory(serverResponse, 4096);
        int bytesReceived = recv(sock, serverResponse, 4096, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error receiving message from server. Disconnecting." << endl;
            break;
        }
        if (bytesReceived == 0) {
            cout << "Server disconnected." << endl;
            break;
        }

        cout << "Server: " << serverResponse << endl;
    }

    closesocket(sock);
    WSACleanup();
    cout << "Client closed." << endl;
    return 0;
}