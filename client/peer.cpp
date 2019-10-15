#include "peer.hpp"

void *download_file(void *args)
{
    download_file_struct *current_args = (download_file_struct*)args;
    char *filename = new char [50]();
    strncpy(filename, current_args->filename1, 50);
    int *download_file = new int[1];
    *download_file = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    close(*download_file);
    while(current_args)
    {
        int *clinet_sock_fd = new int [1];
        while((*clinet_sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
            printf("error creating socket in thread for file %s and chunk num %d\n",
            filename, current_args->chunk_number);
        }

        sockaddr_in *server_peer = new sockaddr_in[1]();
        char *ip = new char[20]();
        strncpy(ip,current_args->ip_port_data.ip, 20);
        server_peer->sin_addr.s_addr = inet_addr(ip);
        server_peer->sin_port = htons(atoi(current_args->ip_port_data.port));
        server_peer->sin_family = AF_INET;
        int *connection_status = new int[1];
        int *num_of_attempts = new int[1];
        while((*connection_status = connect(*clinet_sock_fd, 
        (sockaddr*)server_peer, sizeof(sockaddr_in)))==-1 && *num_of_attempts < 10)
        {
            printf("Connection failed to %s %s\n",ip, current_args->ip_port_data.port);
            *num_of_attempts++;
        }

        if(*connection_status == -1)
        {
            printf("Failed to fetch current %d chunks of file %s\n",
            current_args->chunk_number, filename);
            continue;
        }
        chunk *current_chunk = new chunk[1];

        
        if(recv(*clinet_sock_fd, current_chunk, sizeof(chunk), 0)!=sizeof(chunk))
        {
            printf("recieved improprt chunksfor %s %d\n", 
                filename, current_chunk->chunks_num);
        }

        
        current_args++;
    }
}
int client_work(int *logged_in, peer_data * current_peer_data, pthread_mutex_t peer_data_lock)
{
    while(true)
    {
        printf("Whats to be done?\n");
        printf("1 create user\n2 login\n3 create group\n4 join group\n5 leave group\n6 list request\n7 accept request\n8 list groups\n9 list file\n10 upload file\n11 download file\n12 logout\n13 show downloads\n14 stop share\n");
        // fflush
        int *option = new int[1];
        scanf("%d",option);
        if(*option == 1)
        {
            if(!(*logged_in))
            {
                login_reg_user *register_struct = new login_reg_user[1];
                register_struct->type = 1;
                printf("enter user number\t");
                scanf("%d\n", register_struct->user_id);
                printf("enter password\t");
                scanf("%s", register_struct->password);
                
                int *client_sock_fd = new int[1];
                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
                {
                    printf("create user socket creation failed\n");
                }
                struct sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("create user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("create user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }
                int *sent_bytes = new int[1];
                command *current_command = new command[1];
                current_command->type_of_command = 1;
                
                while((*sent_bytes = send(*client_sock_fd, (void *)current_command,sizeof(login_reg_user), 0)) != sizeof(login_reg_user))
                {
                    printf("create user Error sending the struct\n");
                }

                *sent_bytes = 0;
                while((*sent_bytes = send(*client_sock_fd, (void *)register_struct,sizeof(login_reg_user), 0)) != sizeof(login_reg_user))
                {
                    printf("create user Error sending the struct\n");
                }

                int *return_mesg = new int [1];

                recv(*client_sock_fd, return_mesg, sizeof(int), 0);

                if(*return_mesg != sizeof(int))
                {
                    printf("user create send_failed\n");
                }
                close(*client_sock_fd);
            }
            else
            {
                printf("Already created\n");
            }
        }
        else if(*option == 2)
        {
            if(!(*logged_in))
            {
                login_reg_user *login_struct = new login_reg_user[1];
                login_struct->type = 2;
                printf("enter user number\t");
                scanf("%d\n", login_struct->user_id);
                printf("enter password\t");
                scanf("%s", login_struct->password);
                
                int *client_sock_fd = new int[1];
                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
                {
                    printf("login user socket creation failed\n");
                }
                sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("login user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("login user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }
                
                int *sent_bytes = new int[1];
                command *current_command = new command[1];
                current_command->type_of_command = 2;
                current_command->user_id = login_struct->user_id;
                while((*sent_bytes = send(*client_sock_fd, (void *)current_command,sizeof(login_reg_user), 0)) != sizeof(login_reg_user))
                {
                    printf("create user Error sending the struct\n");
                }

                *sent_bytes = 0;
                while((*sent_bytes = send(*client_sock_fd, (void *)login_struct,sizeof(login_reg_user), 0)) != sizeof(login_reg_user))
                {
                    printf("login user Error sending the struct\n");
                }

                int *return_mesg = new int [1];
                int *recv_size = new int[1];

                *recv_size = recv(*client_sock_fd, return_mesg, sizeof(int), 0);

                if(*recv_size != sizeof(int))
                {
                    printf("login user send_failed\n");
                }

                else
                {
                    *recv_size = recv(*client_sock_fd, return_mesg, sizeof(int),0);   
                    if(*return_mesg == 1)
                    {
                        printf("Logged in successfuly\n");
                        *logged_in = login_struct->user_id;
                        int *user_file_fd = new int[1];
                        char *file_data = new char[10];
                        sprintf(file_data, ".%d", login_struct->user_id);
                        if((*user_file_fd = open(file_data, O_RDONLY))>0)
                        {
                            int *read_bytes = new int[1];
                            void *buffer = new char[sizeof(user_groups)];
                            *read_bytes = read(*user_file_fd, buffer, sizeof(user_groups));
                            if(*read_bytes == sizeof(user_groups))
                            {
                                for(int i=0; i < 100; i++)
                                {
                                    if(((user_groups*)buffer)->groups[i]!=0)
                                    {
                                        vector<string>* new_file_set = new vector<string>();
                                        pthread_mutex_lock(&peer_data_lock);
                                        current_peer_data->group_map[((user_groups*)buffer)->groups[i]] = new_file_set;
                                        pthread_mutex_unlock(&peer_data_lock);
                                    }
                                }
                                delete []buffer;
                                buffer = new char[sizeof(chunks_meta)];
                                *read_bytes = 0;
                                while((*read_bytes = read(*user_file_fd, buffer, sizeof(chunks_meta)))!=0)
                                {
                                    if(*read_bytes == sizeof(chunks_meta))
                                    {
                                        chunks_meta * buff_chunk_meta = (chunks_meta*)buffer;
                                        chunks_meta *new_chunk_meta = new chunks_meta[1];
                                        memcpy(new_chunk_meta, buff_chunk_meta, sizeof(chunks_meta));
                                        pthread_mutex_lock(&peer_data_lock);
                                        //check
                                        current_peer_data->group_map[buff_chunk_meta->group_id]->insert(string(new_chunk_meta->filename));

                                        if((current_peer_data->file_map).find(string(new_chunk_meta->filename))!= current_peer_data->file_map.end())
                                        {
                                            current_peer_data->file_map[string(new_chunk_meta->filename)]->insert(make_pair(new_chunk_meta->chunk_num, new_chunk_meta));
                                        }
                                        else
                                        {
                                            set<pair<int, chunks_meta*>> *current_chunk_meta_set = new  set<pair<int, chunks_meta*>>[1];
                                            current_chunk_meta_set->insert(make_pair(new_chunk_meta->chunk_num, new_chunk_meta));
                                            current_peer_data->file_map[string(new_chunk_meta->filename)]=current_chunk_meta_set;
                                        }
                                        //send a copy to tracker too
                                        pthread_mutex_unlock(&peer_data_lock);
                                        bzero(buffer, sizeof(chunks_meta));
                                    }
                                    else
                                    {
                                        printf("login id read uneven bytes in user file fd\n");
                                        break;
                                    }
                                    
                                }
                                delete []buffer;
                            }
                            else
                            {
                                printf("error reading meta data of user\n");
                            }
                            close(*user_file_fd);
                        }
                    }
                    else
                    {
                        printf("Loggin failed\n");
                    }
                }
                close(*client_sock_fd);
            }
            else
            {
                printf("Already logged in\n");
            }
        }
        else if(*option == 3)
        {
            if(*logged_in)
            {
                int *client_sock_fd = new int[1];

                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
                {
                    printf("create group socket creation failed\n");
                }
                sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("create group failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("create group failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }
                command *command_request = new command[1];
                command_request->type_of_command = 3;
                command_request->user_id = *logged_in;
                while(send(*client_sock_fd, command_request,sizeof(command),0) != sizeof(command))
                {
                    printf("create group send command error\n");
                }

                group_request *group_request_struct = new group_request[1];
                group_request_struct->type = 0;
                while(send(*client_sock_fd,group_request_struct,sizeof(group_request),0)==sizeof(group_request))
                {
                    printf("create group error sending\n");
                }
                int *new_group_id = new int[1];
                int *recv_size = new int[1];

                *recv_size = recv(*client_sock_fd, new_group_id, sizeof(int),0);
                
                if(*recv_size != sizeof(int))
                {
                    printf("create group recv size error\n");
                }
                else
                {
                    pthread_mutex_lock(&peer_data_lock);
                    if(current_peer_data->group_map.find(*new_group_id)==current_peer_data->group_map.end())
                    {
                        current_peer_data->group_map[*new_group_id]=new set<string>[1]();
                    }
                    pthread_mutex_unlock(&peer_data_lock);
                }
                close(*client_sock_fd);
            }
            else
            {
                printf("Not logged in\n");
            }
            
        }
        else if(*option == 4)
        {
            if(*logged_in)
            {
                int *client_sock_fd = new int[1];

                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1)
                {
                    printf("join group socket creation failed\n");
                }
                sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("join group failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("join group failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }
                command *command_request = new command[1];
                command_request->type_of_command = 4;
                command_request->user_id=*logged_in;
                while(send(*client_sock_fd, command_request,sizeof(command),0) != sizeof(command))
                {
                    printf("join group send command error\n");
                }

                int *recev_status = new int[1];
                *recev_status = -1;
                int *group_number = new int[1];
                scanf("%d\n", group_number);
                while(send(*client_sock_fd, group_number, sizeof(int),0)!=sizeof(int))
                {
                    printf("error sending join grpup name\n");
                }
                int sizerecived = recv(*client_sock_fd, recev_status, sizeof(int),0);
                if(sizerecived == sizeof(int) && *recev_status==1)
                {
                    pthread_mutex_lock(&peer_data_lock);
                    if(current_peer_data->group_map.find(*group_number)==current_peer_data->group_map.end())
                        current_peer_data->group_map[*group_number] = new set<string>();
                    pthread_mutex_unlock(&peer_data_lock);

                }
                close(*client_sock_fd);
            }
            else
            {
                printf("Not logged in\n");
            }
        }
        else if(*option == 5)
        {
            //leave group
            //send to server
            ;
        }
        else if(*option == 6)
        {
            //list request
            //send to server
            ;
        }
        else if(*option == 7)
        {
            //accept request
            //send to server
            ;
        }
        else if(*option == 8)
        {
            //list group
            if(*logged_in)
            {
                int *client_sock_fd = new int[1];
                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))!=-1)
                {
                    printf("list group error creating socket\n");
                }
                sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("login user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("login user failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }

                user_groups *group_list = new user_groups[1];

                command *list_command = new command[1];
                list_command->user_id=*logged_in;
                list_command->type_of_command = 8;

                while(send(*client_sock_fd, list_command, sizeof(command),0) == sizeof(command))
                {
                    printf("list groups send failed\n");
                }

                int *recieve_data = new int[1];

                *recieve_data = recv(*client_sock_fd, group_list, sizeof(user_groups),0);

                if(*recieve_data ==sizeof(user_groups))
                {
                    for(int i=0;i<100;i++)
                    {
                        if(group_list->groups[i])
                        {
                            printf("%d\n",i);
                        }
                    }
                }

                close(*client_sock_fd);
                delete []client_sock_fd;
                delete []server;
                delete []connection_status;
                delete []connection_tries;
                delete []group_list;
                delete []recieve_data;
                delete []group_list;                
            }
        }
        else if(*option == 9)
        {
            //list file
            if(*logged_in)
            {
                int *group_number = new int[1];
                printf("enter group number\t");
                scanf("%d\n",group_number);
                pthread_mutex_lock(&peer_data_lock);
                if(current_peer_data->group_map.find(*group_number)!=current_peer_data->group_map.end())
                {
                    for(auto i = (current_peer_data->group_map[*group_number])->begin(); i != current_peer_data->group_map[*group_number]->end(); i++)
                    {
                        printf("%s\n", *i);
                    }
                }
                pthread_mutex_unlock(&peer_data_lock);
                delete[] group_number;
            }
            else
            {
                printf("not logged in\n");
            }
        }
        else if(*option == 10)
        {
            //upload file
            if(*logged_in)
            {
                int *group_number = new int [1];
                printf("enter group name\t");
                scanf("%d\n",group_number );
                char *filename = new char[50];
                printf("enter file name\t");
                scanf("%s", filename);
                pthread_mutex_lock(&peer_data_lock);
                if(current_peer_data->group_map[*group_number]->find(string(filename))==current_peer_data->group_map[*group_number]->end())
                {
                    if(current_peer_data->file_map.find(string(filename))==current_peer_data->file_map.end())
                    {

                        vector<int, chunks_meta*> *vector_chunk = new vector<int, chunks_meta*>[1];
                        current_peer_data->file_map[string(filename)] = vector_chunk;
                        int fd = open(filename, O_RDONLY);
                        int total_chunks = ceil(lseek(fd, 0, SEEK_END) / CHUNK_SIZE);
                        lseek(fd, 0, SEEK_SET);
                        if(fd>0)
                        {
                            int chunk_number = 0;
                            int read_bytes = 0;
                            char *buffer = new char [CHUNK_SIZE]();
                            while((read_bytes=read(fd, buffer, CHUNK_SIZE))>0)   
                            {
                                chunk_number++;
                                char* hash = new char[SHA_DIGEST_LENGTH];
                                SHA1(buffer, read_bytes, hash);
                                chunks_meta * current_chunk = new chunks_meta[1]();

                                memcpy(current_chunk->chunk_sha,hash, SHA_DIGEST_LENGTH);
                                current_chunk->chunk_num = chunk_number;
                                memcpy(current_chunk->filename, filename, 50);
                                current_chunk->group_id=*group_number;
                                current_chunk->is_shared = 1;
                                current_chunk->total_num_of_chunks = total_chunks;
                                
                                current_peer_data->file_map[string(filename)]->insert(make_pair(chunk_number, current_chunk));

                                memset(buffer,'\0',CHUNK_SIZE);
                            }
                            current_peer_data->group_map[*group_number]->insert(string(filename));
                        }
                        else
                        {
                            printf("file not opened\n");
                            continue;
                        }
                    }
                }
                if(current_peer_data->file_map.find(string(filename))!=current_peer_data->file_map.end())
                {
                    int *client_sock_fd = new int[1];
                    while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))!=-1)
                    {
                        printf("upload file error creating socket\n");
                    }
                    sockaddr_in *server = new sockaddr_in[1]();
                    server->sin_family = AF_INET;
                    server->sin_port = htons(atoi(TRACKER_1_PORT));
                    (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                    int *connection_tries = new int[1];
                    int *connection_status = new int[1];

                    while(((*connection_status = connect(*client_sock_fd, 
                            (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("upload file failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries>10)
                    {
                        (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                        server->sin_port = htons(atoi(TRACKER_2_PORT));
                        while(((*connection_status = connect(*client_sock_fd, 
                            (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                        {
                            printf("upload file failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                            *connection_tries++;
                        }
                        if(*connection_tries >10)
                        {
                            printf("TRACKER 1 and 2 both not getting connected\n");
                            return -1;
                        }
                    }
                    command *upload_command = new command[1];
                    upload_command->type_of_command=10;
                    upload_command->user_id=*logged_in;
                    while(sizeof(command)!=send(*client_sock_fd, upload_command, sizeof(command),0))
                    {
                        printf("upload file send command failed\n");
                    }

                    for(set< pair<int, chunks_meta*>,pair_cmp>::iterator i = current_peer_data->file_map[string(filename)]->begin(); i != current_peer_data->file_map[string(filename)]->end();i++)
                    {
                        while(sizeof(chunks_meta)==send(*client_sock_fd, i->second, sizeof(chunks_meta),0))
                        {
                            printf("failed to sedn chunk meta upload file\n");
                        }
                    }
                    close(*client_sock_fd);
                }
                pthread_mutex_unlock(&peer_data_lock);
            }
            else
            {
                printf("not logged in\n");
            }
        }
        else if(*option == 11)
        {
            //download file
            if(*logged_in)
            {
                int *client_sock_fd = new int[1];
                while((*client_sock_fd = socket(AF_INET, SOCK_STREAM, 0))!=-1)
                {
                    printf("download file error creating socket\n");
                }
                sockaddr_in *server = new sockaddr_in[1]();
                server->sin_family = AF_INET;
                server->sin_port = htons(atoi(TRACKER_1_PORT));
                (server->sin_addr).s_addr = inet_addr(TRACKER_1_IP);

                int *connection_tries = new int[1];
                int *connection_status = new int[1];

                while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                {
                    printf("download file failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                    *connection_tries++;
                }
                if(*connection_tries>10)
                {
                    (server->sin_addr).s_addr = inet_addr(TRACKER_2_IP);
                    server->sin_port = htons(atoi(TRACKER_2_PORT));
                    while(((*connection_status = connect(*client_sock_fd, 
                        (sockaddr*)server, sizeof(sockaddr))) == -1) && (*connection_tries < 10))
                    {
                        printf("download file failed to connect to ip %s port %s\n", TRACKER_1_IP, TRACKER_1_PORT);
                        *connection_tries++;
                    }
                    if(*connection_tries >10)
                    {
                        printf("TRACKER 1 and 2 both not getting connected\n");
                        return -1;
                    }
                }
                command *download_command = new command[1];
                download_command->user_id =*logged_in;
                download_command->type_of_command = 11;
                while(sizeof(command)!=send(*client_sock_fd, download_command, sizeof(command),0))
                {
                    printf("download command send failed\n");
                }
                int *size_of_file = new int[1];
                if(recv(*client_sock_fd,size_of_file, sizeof(int),0 ) != sizeof(int))
                {
                    printf("download file ip recieve error\n");
                }
                else
                {
                    int *number_of_ip = new int[1];
                    *number_of_ip = (*size_of_file/CHUNK_SIZE);
                    download_file_struct *ip_port_data = new download_file_struct[*number_of_ip];
                    if(sizeof(download_file_struct)*(*number_of_ip)!=recv(*client_sock_fd, ip_port_data, sizeof(download_file_struct)*(*number_of_ip), 0))
                    {
                        printf("Total file not present only partly available\n");
                    }
                    pthread_t *download_thread = new pthread_t[1];
                    pthread_attr_t *attr = new pthread_attr_t[1];
                    pthread_attr_init(attr);
                    pthread_create(download_thread, attr, download_file, ip_port_data);
                    pthread_detach(*download_thread);
                    //donot delete ip_port_data
                }
                close(*client_sock_fd);
            }
        }
        else if(*option == 12)
        {
            //logout
            
        }
        else if(*option == 13)
        {
            //show download
        }
        else if(*option == 14)
        {
            //stop shar
        }
        else
        {
            printf("INVALID OPTION");
        }
    }
}