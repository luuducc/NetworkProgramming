#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define DEBUGGING
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

int* g_clients = NULL; // lưu thông tin client kết nối tới 
int* g_status = NULL; // lưu thông tin trạng thái kết nối của client tương ứng
int g_count = 0;
const char* msg = "Send any command to execute\n";

// TELNET SERVER USING SELECT()  

int main(int argc, char** argv) {
#ifdef DEBUGGING
    argc = 2;
#endif
    if (argc > 1) {
#ifdef DEBUGGING
        short port = htons(8888);
#else
        short port = (short)atoi(argv[1]);
#endif
        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);        
        SOCKADDR_IN saddr, caddr;
        int clen = sizeof(caddr);
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = 0;
        if (bind(s, (SOCKADDR*)&saddr, sizeof(SOCKADDR)) == 0) { // bind succeeded       
            listen(s, 10);
            fd_set set1;

            while(1) {

                // khôi phục lại tập set1
                FD_ZERO(&set1);
                FD_SET(s, &set1);

                for(int i = 0; i < g_count; i++) {
                    if(g_status[i] == 1)
                        FD_SET(g_clients[i], &set1);
                }

                // kiểm tra xem có kết nối tới hay có client nào gửi dữ liệu tới
                select(FD_SETSIZE, &set1, NULL, NULL, NULL);

                // kiểm tra nếu là có kết nối tới 
                if(FD_ISSET(s, &set1)) {
                    int c = accept(s, (sockaddr*)&caddr, (socklen_t*)&clen);
                            
                    // cấp phát lại bộ nhớ cho mảng g_clients, g_status
                    g_clients = (int*)realloc(g_clients, (g_count + 1) * sizeof(int));
                    g_status = (int*)realloc(g_status, (g_count + 1) * sizeof(int));

                    g_clients[g_count] = c;
                    g_status[g_count] = 1;
                    g_count ++;
                    printf("Socket %d connected\n", c);

                    // gửi welcome message cho client 
                    send(c, msg, strlen(msg), 0);
                }

                // kiểm tra xem có client nào gửi dữ liệu tới 
                for(int i = 0; i < g_count; i ++) 
                    if(g_status[i] == 1 && FD_ISSET(g_clients[i], &set1)) {
                            
                        // liên tục nhận dữ liệu từ client 
                        char buffer[1024] = {0};
                        int r = recv(g_clients[i], buffer, sizeof(buffer) - 1, 0);
                        if(r > 0) {
                                
                            printf("Receiving data from %d: %s",g_clients[i], buffer);
                            if (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
                                buffer[strlen(buffer) - 1] = 0;
                            sprintf(buffer + strlen(buffer), " > command.txt");
                            printf("New data is : %s\n",buffer);
                            system(buffer);

                            FILE* f = fopen("command.txt","rb"); // tạo file commant.txt, mode read binary
                            fseek(f, 0, SEEK_END);
                            int size = ftell(f);
                            char* data = (char*)calloc(size, 1); // data có kích thước size(bytes)
                            fseek(f, 0, SEEK_SET);
                            fread(data, 1, size, f);
                            fclose(f);

                            int sent = 0;
                            int tmp = 0;

                            do {
                                tmp = send(g_clients[i], data, size, 0);
                                sent += tmp;
                            }while (sent < size && tmp >= 0);
                
                            free(data);
                            data = NULL;
                        } else {
                            g_status[i] = 0;
                            printf("socket %d disconnected\n",g_clients[i]);
                        }
                    }
            }
        } else{
            printf("Failed to bind\n");
        }
    }else
        printf("Parameter missing\n");
}