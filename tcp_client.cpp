#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;
char buf[1024] = {0};

// BASIC TCP CLIENT
int main() {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in saddr; // server_address

    saddr.sin_family = AF_INET; 
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /*recommend using the ubuntu loopback card
    despite that the eth0 can be used too*/ 

    int n = connect(s, (sockaddr*)&saddr, sizeof(saddr));
    if(n == 0) {

        cout << "Connected successfully to " << inet_ntoa(saddr.sin_addr) << " at port "
                << ntohs(saddr.sin_port) << endl;

        // received a welcome message from server
        recv(s, buf, sizeof(buf), 0);
        cout << buf;
        cout << "Input a message to send\n";

        // continuously send and receive message from server
        while(1) {

            // send msg to server
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf)-1, stdin); // luôn để byte cuối cùng là 0 làm lính canh
            send(s, buf, strlen(buf), 0);
    
            // receive msg from server
            memset(buf, 0, sizeof(buf));
            int r = recv(s, buf, sizeof(buf), 0);
            if(r > 0) {
                cout << "Server replied: " << buf;
            } else 
                break;
            
        }
        
    }
    close(s);
    return 0;
}