#ifndef TRACKER_H_
#define TRACKER_H_

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <map>
#include <set>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define TRACKER_IP "127.0.0.1"
#define TRACKER_PORT 50505

#define CHUNK_SIZE 512*1024
using namespace std;


struct chunk_meta
{
    int chunk_num;
    int uid;
    int total_chunks;
    char chunk_sha[20];
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


int tracker_method(map<int, login_struct*>& live_user_map, map<int, login_struct*>& user_map, map<int, set<string>*>& filemap, map<string, vector<chunk_meta*>*>& file_chunkmap);

#endif