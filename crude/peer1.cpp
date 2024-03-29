#include <sys/socket.h>
#include <arpa/inet.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct ip_data
{
    int port;
    int chunk_num;
    char ip[20];
};

#define chunk_sz 10*1024
void *routine(void* args)
{
    int port = ((ip_data*)args)->port;
    char *ip = (char*)calloc(sizeof(char),50);
    memcpy(ip,((ip_data*)args)->ip,20);
    int sock = -1;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr *addr = (sockaddr*)calloc(1,sizeof(sockaddr));
    ((sockaddr_in*)addr)->sin_addr.s_addr=inet_addr(ip);
    ((sockaddr_in*)addr)->sin_family=AF_INET;
    ((sockaddr_in*)addr)->sin_port=htons(port);
    bind(sock, addr, sizeof(sockaddr));
    listen(sock,10);
    while(1)
    {
        sockaddr_in *client = (sockaddr_in*)calloc(1,sizeof(sockaddr_in));
        int sock_addr_size = sizeof(sockaddr_in);
        int cli_sock = accept(sock, (sockaddr*)client,(socklen_t*)&sock_addr_size);
        int chunk_num=-1;
        recv(cli_sock, &chunk_num,sizeof(int),0);
        if(chunk_num == -1)
            break;
        int fd = open("samplefile", O_RDONLY);
        lseek(fd, (chunk_num-1)*chunk_sz,SEEK_SET);
        void *buffer = (void*)calloc(sizeof(char),chunk_sz);
        int read_by = read(fd, buffer, chunk_sz);
        send(cli_sock, buffer, read_by,0);
        close(cli_sock);
    }
}

int main()
{
    pthread_t server1, server2, server3;
    ip_data *ip = (ip_data*)calloc(sizeof(ip_data),1);
    strncpy(ip->ip,"127.0.0.1", strlen("127.0.0.1"));
    ip->port = 50505;
    pthread_create(&server1, NULL, routine, (void*)ip);
    ip = (ip_data*)calloc(sizeof(ip_data),1);
    ip->port = 50506;
    pthread_create(&server2, NULL, routine, (void*)ip);
    ip = (ip_data*)calloc(sizeof(ip_data),1);
    ip->port = 50507;
    pthread_create(&server2, NULL, routine, (void*)ip);
    pthread_detach(server1);
    pthread_detach(server2);
    pthread_detach(server3);
    
    int dest=open("dest",O_WRONLY|O_TRUNC|O_CREAT,0);
    int sock = -1;
    int chunk=1;
    void *buf = calloc(sizeof(char),chunk_sz);
    int r_bytes;
    sock=socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in *client = (sockaddr_in*)calloc(sizeof(sockaddr_in), 1);
    client->sin_family = AF_INET;
    client->sin_port = htons(50505);
    client->sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)client, sizeof(sockaddr));
    send(sock, &chunk, sizeof(int), 0);
    r_bytes=recv(sock, buf, chunk_sz,0);
    lseek(dest,(chunk-1)*chunk_sz,SEEK_SET);
    read(dest,buf, r_bytes);
    close(sock);
    sock=socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in *client = (sockaddr_in*)calloc(sizeof(sockaddr_in), 1);
    client->sin_family = AF_INET;
    client->sin_port = htons(50505);
    client->sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)client, sizeof(sockaddr));
    chunk=3;
    send(sock, &chunk, sizeof(int), 0);
    r_bytes=recv(sock, buf, chunk_sz,0);
    lseek(dest,(chunk-1)*chunk_sz,SEEK_SET);
    read(dest,buf, r_bytes);
    close(sock);

    connect(sock, (sockaddr*)client, sizeof(sockaddr));
    chunk=2;
    send(sock, &chunk, sizeof(int), 0);
    r_bytes=recv(sock, buf, chunk_sz,0);
    lseek(dest,(chunk-1)*chunk_sz,SEEK_SET);
    read(dest,buf, r_bytes);
    close(sock);

    close(dest);
    return 0;
}