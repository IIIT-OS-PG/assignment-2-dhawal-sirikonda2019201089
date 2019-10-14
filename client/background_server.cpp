#include "background_server.h"

void * ip_port_copy(char **argv)
{
    ip_port *ip_port_data = new ip_port();
    ip_port_data->ip = argv[0];
    ip_port_data->port = argv[1];
    return (void*)ip_port_data;
}

void* background_server(void * argv)
{

    int *return_status = new int[1];
    ip_port *ip_port_data = (ip_port*)argv;

    int peer1_sock;
    while((peer1_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("background server error creating socket\n");
        *return_status = -1;
    }
    *return_status = 0;

    sockaddr_in *peer1 = new sockaddr_in[1];
    socklen_t size_of_sockaddr = sizeof(sockaddr);

    peer1->sin_family = AF_INET;
    peer1->sin_port = htons(atoi(ip_port_data->port));
    peer1->sin_addr.s_addr = inet_addr(ip_port_data->ip);

    while((bind(peer1_sock, (sockaddr*)peer1, size_of_sockaddr))<0)
    {
        printf("background server bind error\n");
        *return_status = -2;
    }
    *return_status = 0;

    while(listen(peer1_sock, 100) < 0)
    {
        printf("background server listen\n");
        *return_status = -3;
    }
    *return_status = 0;

    while(true)
    {
        size_of_sockaddr = sizeof(sockaddr);
        int request_socket;
        sockaddr_in *request_client = new sockaddr_in[1];
        if((request_socket = accept(peer1_sock, (sockaddr*)request_client, &size_of_sockaddr)) < 0)
        {
            printf("accept call failed\n");
            *return_status = -4;
        }
        *return_status = 0;

        
        

    }
    


}