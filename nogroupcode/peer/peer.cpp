#include "peer.h"

int peer_client(map<string, vector<chunk_meta*>*>& peer_data_map,int *login_status, int *port, pthread_mutex_t lock)
{
    int usr_id = -1;
    int option = -1;
    int sock_fd = -1;
    sockaddr_in* server = (sockaddr_in*)calloc(1, sizeof(sockaddr));
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr(TRACKER_IP);
    server->sin_port = htons(TRACKER_PORT);
    while(1)
    {
        printf("enter command\n 1 create user\n 2 login\n 3 upload file\n 4 download file\n 5 logout\n option ?\t");
        scanf("%d", &option);
        if(option == 1)
        {
            if(!*login_status)
            {
                int user_id = -1;
                char *password = (char*)calloc(4, sizeof(char));
                printf("enter userid\t");
                scanf("%d", &user_id);
                printf("password\t");
                scanf("%d", password);
                int sock_fd = 1;
                while((sock_fd = socket(AF_INET, SOCK_STREAM,0))==-1)
                    printf("socket creation error\n");

                int connect_status = -1;
                while((connect_status= connect(sock_fd, (sockaddr*)server, sizeof(sockaddr)))==-1)
                    printf("connect error with tracker\n");

                while(sizeof(int)!=send(sock_fd, &option, sizeof(int),0))
                    printf("error sending command type\n");

                login_struct *login_data = (login_struct*)calloc(1, sizeof(login_struct));
                login_data->uid = user_id;
                login_data->ip_port_info.port = *port;
                memset(login_data->ip_port_info.ip, '\0', 20);
                strcpy(login_data->ip_port_info.ip, CURRENT_IP);
                strcpy(login_data->password, password);

                while(sizeof(login_struct)!=send(sock_fd, login_data, sizeof(login_struct), 1))
                    printf("error sending create user request\n");
                
                int status = -1;
                while(sizeof(int)!=recv(sock_fd, &status, sizeof(int),0))
                    printf("receive failed\n");

                if(status == -10)
                    printf("user already registered use different details\n");
                
                if(status == 0)
                    printf("user created\n");

                free(login_data);
                free(password);
                close(sock_fd);
            }
            else
            {
                printf("already logged in\n");
            }
        }
        else if(option == 2)
        {
            if(!*login_status)
            {int user_id = -1;
                char *password = (char*)calloc(4, sizeof(char));
                printf("enter userid\t");
                scanf("%d", &user_id);
                printf("password\t");
                scanf("%d", password);
                int sock_fd = 1;
                while((sock_fd = socket(AF_INET, SOCK_STREAM,0))==-1)
                    printf("socket creation error\n");

                int connect_status = -1;
                while((connect_status= connect(sock_fd, (sockaddr*)server, sizeof(sockaddr)))==-1)
                    printf("connect error with tracker\n");

                while(sizeof(int)!=send(sock_fd, &option, sizeof(int),0))
                    printf("error sending command type\n");

                login_struct *login_data = (login_struct*)calloc(1, sizeof(login_struct));
                login_data->uid = user_id;
                login_data->ip_port_info.port = *port;
                memset(login_data->ip_port_info.ip, '\0', 20);
                strcpy(login_data->ip_port_info.ip, CURRENT_IP);
                strcpy(login_data->password, password);

                while(sizeof(login_struct)!=send(sock_fd, login_data, sizeof(login_struct), 1))
                    printf("error sending create user request\n");
                
                int status = -1;
                while(sizeof(int)!=recv(sock_fd, &status, sizeof(int),0))
                    printf("receive failed\n");
                
                if(status == 0)
                {
                    *login_status = 1;
                }
                else if (status == -10)
                {
                    printf("user does not exist\n");
                }
                else if(status == -20)
                {
                    printf("already logged in\n");
                }
                usr_id=user_id;

                int fd = open(to_string(usr_id).c_str(),O_RDONLY);
                int num_of_chunks = -1;
                if(fd<0)
                {
                    printf("failed to open file\n");
                    send(sock_fd, &num_of_chunks, sizeof(int),0);
                    continue;
                }
                struct stat *buff = (struct stat*)calloc(sizeof(struct stat),1);

                fstat(fd, buff);
                num_of_chunks = buff->st_size/sizeof(chunk_meta);

                while(sizeof(int) != send(sock_fd,&num_of_chunks,sizeof(int),0))
                    printf("send chunk count failed\n");

                int read_bytes = 0;
                char *buffer = (char*)calloc(sizeof(chunk_meta), 1);
                while((read_bytes = read(fd, buffer, sizeof(chunk_meta))))
                {
                    if(read_bytes!=sizeof(chunk_meta))
                    {
                        printf("file error\n");
                    }
                    else
                    {
                        chunk_meta * current_chunk_data = (chunk_meta*)calloc(sizeof(chunk_meta), 1);
                        memcpy(current_chunk_data, buffer, read_bytes);
                        char *file_name = (char*)calloc(sizeof(char),50);
                        strncpy(file_name, current_chunk_data->filename, 50);
                        pthread_mutex_lock(&lock);
                        if(peer_data_map.find(string(file_name))==peer_data_map.end())
                        {
                            peer_data_map[string(file_name)] = new vector<chunk_meta*>();
                        }
                        peer_data_map[string(file_name)]->push_back(current_chunk_data);
                        pthread_mutex_unlock(&lock);
                        while(sizeof(chunk_meta)!=send(sock_fd, current_chunk_data, sizeof(chunk_meta),0))
                            printf("sending chnk data failed\n");
                        free(file_name);
                    }
                }
                close(fd);
                free(buffer);
            }
            else
            {
                printf("already logged in\n");
            }
        }
        else if(option == 3)
        {
            if(*login_status)
            {
                printf("enter filename\t");
                char *filename = (char*)calloc(sizeof(char), 50);
                scanf("%s",filename);
                pthread_mutex_lock(&lock);
                if(peer_data_map.find(string(filename))!=peer_data_map.end())
                {
                    printf("file already uploaded\n");
                    continue;
                }
                else
                {
                    peer_data_map[string(filename)] = new vector<chunk_meta*>();
                }
                pthread_mutex_unlock(&lock);

                int fd = open(filename, O_RDONLY);
                if(fd<0)
                {
                    printf("file not opened for upload\n");
                    free(filename);
                    continue;
                }
                while((sock_fd = socket(AF_INET, SOCK_STREAM,0))==-1)
                    printf("socket creation error\n");

                while(connect(sock_fd, (sockaddr*)server, sizeof(sockaddr))==-1)
                    printf("connect failed\n");

                
                while(sizeof(int)!=send(sock_fd, &option, sizeof(int),0))
                    printf("send error for command\n");

                
                
                struct stat *stat_buff = (struct stat*)calloc(sizeof(struct stat),1);
                fstat(fd,stat_buff);
                char *buffer = (char*)calloc(CHUNK_SIZE, sizeof(char));
                int read_bytes =0;
                int chunk_num=1;
                
                while((read_bytes = read(fd, buffer, CHUNK_SIZE))>0)
                {
                    chunk_meta *current_chunk_meta = (chunk_meta*)calloc(sizeof(chunk_meta),1);
                    char *digest = (char*)calloc(sizeof(char), SHA_DIGEST_LENGTH);
                    SHA1((const unsigned char *)buffer, read_bytes,(unsigned char*) digest);
                    memcpy(current_chunk_meta->chunk_sha, digest, 20);
                    memcpy(current_chunk_meta->filename, filename, 50);
                    current_chunk_meta->chunk_num =chunk_num++;
                    current_chunk_meta->total_chunks = ceil(stat_buff->st_size/CHUNK_SIZE);
                    current_chunk_meta->uid = usr_id;
                    pthread_mutex_lock(&lock);
                    peer_data_map[string(filename)]->push_back(current_chunk_meta);
                    pthread_mutex_unlock(&lock);
                    while(sizeof(chunk_meta)!=send(sock_fd, current_chunk_meta, sizeof(chunk_meta),0))
                        printf("failed to send chunk sha data\n");
                    memset(buffer,0,CHUNK_SIZE);
                }
                free(buffer);
                close(sock_fd);
            }
            else
            {
                printf("not logged in\n");
            }
            
        }
        else if(option == 4)
        {
            if(*login_status)
            {

            }
            else
            {
                printf("not logged in\n");
            }
        }
        else if(option == 5)
        {
            if(*login_status)
            {
                int fd = open(to_string(usr_id).c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0644);
                if(fd<0)
                {
                    printf("logout file creattion failed\n");
                    continue;
                }
                pthread_mutex_lock(&lock);
                map<string, vector<chunk_meta*>*>::iterator i = peer_data_map.begin();
                for(;i!=peer_data_map.end();i++)
                {
                    for(vector<chunk_meta*>::iterator j = i->second->begin(); j != i->second->end();j++)
                    {
                        write(fd, &j, sizeof(chunk_meta));
                    }
                }
                pthread_mutex_unlock(&lock);
                close(fd);
            }
            else
            {
                printf("not logged in\n");
            }
        }
}