#ifndef PEER_H_
#define PEER_H_
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <map>
#include <string>
#include <vector>


#include <sys/socket.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define CHUNK_SIZE 512*1024
#define TRACKER_IP "127.0.0.1"
#define TRACKER_PORT 50505

#define CURRENT_IP "127.0.0.1"

using namespace std;
struct chunk_sha
{
    char sha[20];
};

struct chunk_meta
{
    int chunk_num;
    int total_chunks;
    struct chunk_sha;
    char filename[50];
};

struct ip_port_struct
{
    int port;
    char ip[20];
};

struct chunk_data
{
    int user_id;
    int total_chunks;
    int chunk_number;
    char data[CHUNK_SIZE];
};

struct login_struct
{
    int uid;
    ip_port_struct ip_port_info;
    char password[4];
};

int peer_clinet(map<string, vector<chunk_meta*>*>& filedata, int *login_status);

void *background_server(void *args);

#endif