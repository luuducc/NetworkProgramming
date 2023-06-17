#include <sys/types.h> // socket functions API
#include <sys/socket.h>
#include <arpa/inet.h> // name and ip API
#include <string.h>
#include <unistd.h> // close socket
#include <iostream>

using namespace std;
const char *welcome = "Hello from TCP server\n";
char buf[1024] = {0};

// BASIC TCP SERVER THAT SERVES ONLY ONE CLIENT!

int main() {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in saddr, caddr; // server_address and client_address
    int clen = sizeof(caddr);

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    // saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_addr.s_addr = 0;

    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    listen(s, 10);
    int c = accept(s, (sockaddr*)&caddr, (socklen_t*)&clen);
    if(c > 0) {
        // send welcome message to client
        send(c, welcome, strlen(welcome), 0);
    } else {
        close(s);
        close(c);
        return 0;
    }

    while(1) { // continuously receive and send message to client

        // received msg from client
        memset(buf, 0, sizeof(buf));
        int r = recv(c, buf, sizeof(buf), 0);
        if(r > 0) {
            cout << "Received " << r << " bytes from " << inet_ntoa(caddr.sin_addr) << ", port "
                << ntohs(caddr.sin_port) << ": " << buf;
        }
        else break;

        // send msg to client
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, stdin);
        send(c, buf, strlen(buf), 0);
        
    }

    close(c);
    close(s);
    return 0;
}