#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
int* g_client = NULL;
int g_count = 0;
pthread_mutex_t* mutex = NULL;

// TELNET SERVER USING PTHREAD

void* ClientThread(void* arg) {
    int c = *(int*)arg;
    free(arg);
    arg = NULL;
    char buffer[1024];

    const char* msg = "TELNET CHATROOM USING THREAD\nSend any command to execute...\n";
    // gửi Welcome message cho Client vừa kết nối tới
    send(c, msg, strlen(msg), 0);

    printf("\n%d has connected\n",c); // hiển thị Client nào đã kết nối tới

    while(1) { // vòng lặp liên tục nhận dữ liệu từ Client và gửi trả lại cho Client

        char buffer[1024];
                memset(buffer, 0, sizeof(buffer));\
                
                // nhận dữ liệu từ client
                int received = recv(c, buffer, sizeof(buffer) - 1, 0);
                

                if(received > 0) {

                    printf("Receiving data from %d: %s",c,buffer); // for debugging, không cần \n nữa do khi client enter cũng sẽ được lưu vào buffer

                    if (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
                        buffer[strlen(buffer) - 1] = 0; // loại bỏ các ký tự xuống dòng

                    sprintf(buffer + strlen(buffer), " > command.txt"); // thêm dòng command.txt vào buf, từ vị trí trước đó + thêm strlen
                    printf("New data is : %s\n",buffer);
                    system(buffer); // > command.txt <=> nội dung của buffer sẽ được redirect vào file command.txt
                    // việc cái client truy cập đồng thời vào command.txt có thể gây xung đột
                    // => dùng cặp hàm mutex lock và unlock để bao trùm đoạn code truy nhập file
                    
                    pthread_mutex_lock(mutex);
                    // bắt đầu đoạn code gây xung đột
                    FILE* f = fopen("command.txt","rb"); // tạo file commant.txt, mode read binary
                    fseek(f, 0, SEEK_END);
                    int size = ftell(f);
                    char* data = (char*)calloc(size, 1); // data có kích thước size(bytes)
                    fseek(f, 0, SEEK_SET);
                    fread(data, 1, size, f);
                    fclose(f);
                    // kết thúc đoạn code gây xung đột
                    pthread_mutex_unlock(mutex);

                    // printf("Receiving data...: %s\n", buffer); // for debugging
                    
                    int sent = 0;
                    int tmp = 0;
                    
                    // gửi lại nội dung lệnh đã thực thi cho đúng Client nào mà đã gửi cho Server
                    // không gửi lại cho tất cả Client đã kết nối tới Server
                    // for (int i = 0;i < g_count;i++) {
                    //     if (g_client[i] == c) { 
                    //         do {
                    //             tmp = send(g_client[i], data, size, 0);
                    //             sent += tmp;
                    //         }while (sent < size && tmp >= 0);
                    //     }
                    // }
                    do {
                                tmp = send(c, data, size, 0);
                                sent += tmp;
                            }while (sent < size && tmp >= 0);
                
                    free(data);
                    data = NULL;
                }else break;
    }
    printf("%d disconnected\n", c);// thông báo client nào đã ngắt kết nối
    // 1 Client hay toàn bộ Client ngắt kết nối không làm ngắt đi toàn bộ hệ thống
    return NULL;
}
int main(int argc, char** argv)
{
    if (argc > 1) {

        // khởi tạo biến mutex để giải quyết xung đột
        mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
        pthread_mutex_init(mutex, NULL);

        short port = (short)atoi(argv[1]);

        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);     
        SOCKADDR_IN myaddr, client;
        int clen = sizeof(client);
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(port);
        myaddr.sin_addr.s_addr = 0;

        // gắn socket của server vào 1 địa chỉ cố định
        if (bind(s, (SOCKADDR*)&myaddr, sizeof(myaddr)) == 0) { 
            listen(s, 10);
            while(1) {

                // tiến trình cha luôn nhận accept tới từ các Client
                int c = accept(s, (sockaddr*)&client, (socklen_t*)&clen);
                g_client = (int*)realloc(g_client, (g_count + 1) * sizeof(int));
                g_client[g_count] = c;
                g_count += 1;      
                pthread_t tid;
                int* arg = (int*)calloc(1, sizeof(int));
                *arg = c;
                // tạo ra các luồng để xử lý cho từng Client
                pthread_create(&tid, NULL, ClientThread, (void*)arg);
                
            }
            // Giải phóng biến mutex sau khi dùng xong
            pthread_mutex_destroy(mutex);
            free(mutex);
            mutex = NULL;
        }
        
    }else printf("Parameter missing\n");
}