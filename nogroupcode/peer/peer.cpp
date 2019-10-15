#include "peer.h"

int peer_client(map<string, vector<chunk_meta*>*>& peer_data_map,int *login_status, int *port, )
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
            {
                
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
                
                if(peer_data_map.find(string(filename))!=peer_data_map.end())
                {
                    printf("file already uploaded\n");
                    continue;
                }
                else
                {
                    peer_data_map[string(filename)] = new vector<chunk_meta*>();
                }

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
                    peer_data_map[string(filename)]->push_back(current_chunk_meta);
                    while(sizeof(chunk_meta)!=send(sock_fd, current_chunk_meta, sizeof(chunk_meta),0))
                        printf("failed to send chunk sha data\n");
                }

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

            }
            else
            {
                printf("not logged in\n");
            }
        }
}