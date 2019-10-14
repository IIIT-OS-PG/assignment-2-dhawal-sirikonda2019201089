#ifndef CLIENT_H_
#define CLIENT_H_

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <string.h>

#include <vector>
#include <map>
#include <set>
#include <string>

#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define CHUNK_SIZE 524288

#define TRACKER_1_IP "127.0.0.1"
#define TRACKER_2_IP "127.0.0.1"

#define TRACKER_1_PORT "50505"
#define TRACKER_2_PORT "50506"

using namespace std;

struct command
{
    int type_of_command;
    int user_id;
    char command_data[500];
};

struct user_groups
{
    int groups[100];
};

struct login_reg_user
{
    int user_id;
    int type;//1 for create 2 for login
    char password[4];
};

struct group_request
{
    int type;
};

struct file_list
{
    int number_of_files;
    char filenames[200][50];
    int share_permission[200];
};

struct chunks_meta
{
    int total_num_of_chunks;
    int chunk_num;
    int group_id;
    int is_shared;
    char filename[50];
    char chunk_sha[20];
};

struct ip_port_struct
{
    char ip[20];
    char port[10];
};

struct pair_cmp 
{
    bool operator()(pair<int, chunks_meta*> first, pair<int, chunks_meta*> second) 
    {
        return first.first < second.first;
    }
};

struct download_file_struct
{
    int chunk_number;
    char ip[20];
    char port[10];
    char filename[50];
};

struct peer_data
{
    map<int, set<string>*> group_map;
    map<string, set< pair<int, chunks_meta*>,pair_cmp>*> file_map;
};

int client_work(int*, peer_data*, pthread_mutex_t)

#endif //!CLIENT_H_