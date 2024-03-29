#include "tracker.h"

int tracker_method(map<int, login_struct*>& usr_map, map<int, login_struct*>& live_user_map, map<int, set<string>*>& userfilemap, map<string, vector<chunk_meta*>*>& file_chunkmap)
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
                    userfilemap[login_data->uid] = new set<string>();
                    status = 0;
                }
            }
            send(client_socket,&status, sizeof(int), 0);
            //fetch his sharable data
            int is_there_data = 0;
            recv(client_socket,&is_there_data, sizeof(int),0);
            if(is_there_data>0)
            {
                int num_of_chunks = is_there_data;
                while(num_of_chunks--)
                {
                    chunk_meta *current_chunk_meta = (chunk_meta*)calloc(sizeof(chunk_meta),1);
                    int revd_data_size = recv(client_socket, current_chunk_meta, sizeof(chunk_meta),0);
                    if(revd_data_size != sizeof(chunk_meta))
                        printf("error in recv\n");
                    int num_of_chunks = current_chunk_meta->total_chunks;
                    char *file_name = (char*)calloc(sizeof(char),50);
                    strncpy(file_name, current_chunk_meta->filename, 50);
                    if(login_data->uid != current_chunk_meta->uid)
                        printf("user discripency\n");
                    userfilemap[current_chunk_meta->uid]->insert(string(file_name));
                    if(file_chunkmap.find(string(file_name))==file_chunkmap.end())
                    {
                        file_chunkmap[string(file_name)] = new vector<chunk_meta*>();
                    }
                    file_chunkmap[string(file_name)]->push_back(current_chunk_meta);
                }
            }
        }

        if(command_type == 3)
        {
            int numofrecv = -1;
            recv(client_socket, &numofrecv, sizeof(int), 0);
            if(numofrecv>0)
            {
                while(numofrecv--)
                {
                    chunk_meta *current_chunk = (chunk_meta*)calloc(sizeof(chunk_meta),1);
                    recv(client_socket, current_chunk, sizeof(chunk_meta), 0);
                    char *filename = (char*)calloc(sizeof(char), 50);
                    strncpy(filename, current_chunk->filename, 50);
                    if(userfilemap[current_chunk->uid]->find(string(filename))!=userfilemap[current_chunk->uid]->end())
                    {
                        userfilemap[current_chunk->uid]->insert(string(filename));
                        file_chunkmap[string(filename)] = new vector<chunk_meta*>();
                    }
                    file_chunkmap[string(filename)]->push_back(current_chunk);
                }
            }
        }
        if(command_type == 4)
        {
            
        }
        if(command_type == 5)
        {
            int user_id = -1;
            recv(client_socket, &user_id, sizeof(int),0);
            if(live_user_map.find(user_id)!=live_user_map.end())
            {
                live_user_map.erase(user_id);
                userfilemap.erase(user_id);
            }
        }
        close(client_socket);
        printf("want to list files yes/no (1|0)\t");
        int list =-1;
        scanf("%d", &list);
        if(list == 1)
        {
            for(auto i : file_chunkmap)
                printf("%s\n", i.first.c_str());
        }
        // printf("want to continue/exit?1:0\t");
        // int option = -1;
        // scanf("%d",&option);
        // if(option==0)
        // {
        //     //exit routine write all user data to file and exit
        //     break;
        // }
    }
    
}