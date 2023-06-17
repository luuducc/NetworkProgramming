#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h> 
#include <iostream>

using namespace std;
char buf[1024] = {0};

// BASIC UDP SERVER
int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in saddr, sender; 
    int slen = sizeof(sender);

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    

    while(1) {

        memset(buf, 0, sizeof(buf));
        recvfrom(s, buf, sizeof(buf)-1, 0, (sockaddr*)&sender, (socklen_t*)&slen);
        cout << "Received a message from " << inet_ntoa(sender.sin_addr) << ", at port " 
                << ntohs(sender.sin_port) << ": " << buf;

        cout << "Input a message to send: ";
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, stdin);
        sendto(s, buf, strlen(buf), 0, (sockaddr*)&sender, slen);
    
    }
}