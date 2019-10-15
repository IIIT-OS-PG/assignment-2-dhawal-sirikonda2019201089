#include "tracker.h"

int tracker(map<int, login_struct*>& usr_map, map<int, login_struct*>& live_user_map, map<int, set<string>>& filemap, map<string, chunk_meta*>)
{
    int sock_fd = -1;
    while((sock_fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
        printf("tracker socket creation error\n");

    sockaddr_in *server = (sockaddr_in*)calloc(sizeof(sockaddr_in),1);
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(TRACKER_IP);
    server->sin_port = htons(TRACKER_PORT);

    while((bind(sock_fd, (sockaddr*)server, sizeof(sockaddr_in)))<0)
        printf("background server bind error\n");
    
    while(listen(sock_fd, 1000)<-1)
        printf("listen failed\n");
    
    while(1)
    {
        int client_socket = -1;
        sockaddr_in *client = (sockaddr_in*)calloc(1,sizeof(client));
        int sock_len = sizeof(sockaddr_in);
        while((client_socket =accept(sock_fd, (sockaddr*)client,(socklen_t*)&sock_len))<0)
            printf("socket accept failed\n");
        int command_type = -1;
        recv(client_socket, &command_type, sizeof(int), 0);

        if(command_type == 1)
        {
            //create user
            login_struct *login_data = (login_struct*)calloc(1,sizeof(login_struct));
            recv(client_socket, login_data, sizeof(login_struct),0);
            int status = -10;
            if(usr_map.find(login_data->uid)==usr_map.end())
            {
                usr_map[login_data->uid] = login_data;
                status = 0;
            }
            send(client_socket,&status, sizeof(int), 0);

        }
        if(command_type == 2)
        {
            login_struct *login_data = (login_struct*)calloc(1,sizeof(login_struct));
            recv(client_socket, login_data, sizeof(login_struct),0);
            int status = -10;
            if(usr_map.find(login_data->uid)!=usr_map.end())
            {
                if(live_user_map.find(login_data->uid)!=live_user_map.end())
                {
                    status = -20;
                }
                else
                {
                    live_user_map[login_data->uid] = login_data;
                    usr_map[login_data->uid] = login_data;
                    status = 0;
                }
            }
            send(client_socket,&status, sizeof(int), 0);
            //fetch his sharable data
        }
        close(client_socket);
    }
    
}