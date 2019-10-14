#ifndef TRACKER_H_
#define TRACKER_H_

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <map>
#include <utility>
#include <vector>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

#define GROUP_USER_FILE "group_user.bin"
#define USER_DATA_FILE "user_data.bin"
#define FILE_PERM 0644

struct user_data
{
    int user_id;
    int port;
    int num_of_files;
    int num_of_groups_owned;
    char password[4];
    int group_ids[100];
    int owner[100];
    int is_full[200];
    char ip[20];
    char files[200][50];
};

struct command_type
{
    int command;
    char command_args[200];
};

struct user_files
{
    char filename[50];
    int is_ful;
    int file_size;
    int num_of_chunks;
};

struct chunk_data_meta
{
    int chunk_number;
    char sha[20];
};

struct group_data
{
    int group_id;
    int owner_id;
    int number_of_users;
    int users_id[100];
    //change to dynamic if time permits
};

struct alive_users
{
    int user_id[100];
};

int set_up(map<int, group_data*>&, map<int, user_data*>&);
int exit_phase(map<int, group_data*>&, map<int, user_data*>&);

#endif //!TRACKER_H_