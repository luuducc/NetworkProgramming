#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
char msg[1024];
SOCKADDR_IN* senders = NULL; // mảng chứa các cấu trúc địa chỉ của những client đã gửi dữ liệu cho mình(server )
int count = 0;
int main()
{
    // UDP SERVER
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    SOCKADDR_IN myaddr, sender;
    int slen = sizeof(SOCKADDR_IN);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(5000);
    myaddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&myaddr, sizeof(SOCKADDR_IN));

    while (0 == 0)
    {
        memset(msg, 0, sizeof(msg));
        int received = recvfrom(s, msg, sizeof(msg) - 1, 0, (SOCKADDR*)&sender, (socklen_t*)&slen);
        // hàm recvfrom() sẽ trả lại thông tin của client, chứa trong biến sender

        printf("Received %d bytes from %s: %s\n", received, inet_ntoa(sender.sin_addr), msg);

        memcpy(senders + count, &sender, sizeof(SOCKADDR_IN)); 
        // nới rộng danh sánh chứa client, số lượng mới là count +1 , đủ để chứa thêm 1 phần tử(clinent) nữa 
       
        senders = (SOCKADDR_IN*)realloc(senders, (count + 1) * sizeof(SOCKADDR_IN)); // nới rộng danh sánh chứa client, số lượng mới là count +1 
        
        // lưu thông tin của client mới vào mảng senders, lưu tại vị trí mà chứa phần tử tiếp theo => chính là + count
        memcpy(senders + count, &sender, sizeof(SOCKADDR_IN)); 
        count += 1;
        for (int i = 0; i < count;i++)
        {
            sendto(s, msg, strlen(msg), 0, (SOCKADDR*)&(senders[i]), sizeof(SOCKADDR_IN));
        }
    }
    close(s);
    free(senders);
    senders = NULL;
    // nếu dùng tcp sẽ không tạo được chatroom như thế này, do TCP có 2 điểm treo là hàm accept() 
    // và hàm recv() 
    // còn ở vd này, chỉ có 1 điểm treo ở hàm recvfrom(), nhưng điểm treo này không chỉ làm việc
    // với 1 client, mà nó có thể nhận dữ liệu từ bất kỳ 1 client nào 
    // thậm chí 2 client đồng thời kết nối, lần thứ nhất sẽ nhận của client 1
    // sau khi quay lại hàm recvfrom() lần thứ 2 sẽ ngay lập tức nhận của client 2 
    // chứ không bị treo nữa, do cơ chế của tầng giao vận, liên quan đến bộ nhớ buffer
    // Tuy nhiên vẫn có cách giải quyết cho TCP, nhưng sẽ tốn thêm chi phí(CPU, RAM)
    // => là ưu điểm rất nổi bật của UDP
}