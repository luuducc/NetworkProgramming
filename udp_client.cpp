#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h> 
#include <iostream>

using namespace std;
char buf[1024] = {0};
const int broadcast_on = 1;

// BASIC UDP CLIENT
int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // BROADCASTING OPTION
    /* setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast_on, sizeof(broadcast_on)); */

    sockaddr_in caddr, saddr, sender; // client, server and sender address structure
    int slen = sizeof(sender);

    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(6000);
    caddr.sin_addr.s_addr = 0;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    bind(s, (sockaddr*)&caddr, sizeof(caddr));
    

    while(1) {
        cout << "Input a message to send: ";
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, stdin);
        sendto(s, buf, strlen(buf), 0, (sockaddr*)&saddr, slen);
    
        memset(buf, 0, sizeof(buf));
        recvfrom(s, buf, sizeof(buf)-1, 0, (sockaddr*)&sender, (socklen_t*)&slen);
        cout << "Received a message from " << inet_ntoa(sender.sin_addr) << ", at port " 
                << ntohs(sender.sin_port) << ": " << buf;
    }
}