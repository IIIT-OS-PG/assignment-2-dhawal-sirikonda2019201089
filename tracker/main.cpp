#include "tracker.h"

int main(int argc, char **argv)
{
    int tracker_sock =-1;

    if((tracker_sock=socket(AF_INET, SOCK_STREAM,0))<0)
    {
        printf("tracker::failed to create socket\n");
        return -1;
    }

    struct sockaddr_in *server = (sockaddr_in*)new sockaddr[1];
    struct sockaddr_in *client = (sockaddr_in*)new sockaddr[1];
    // *server.sin_familiy = inet_htons

    return 0;
}
    // map<int, group_data*> group_data_map;
    // map<int, user_data*> user_data_map;
    // map<char*, 
    // int setup_status = set_up(group_data_map, user_data_map);
    // if(setup_status==0)
    // {
    //     //launch thread;
    // }
    // else
    // {
    //     //launch thread
    // }

    // //exit_phase
    // int exit_status = exit_phase(group_data_map, user_data_map);
    // if(exit_status!=0)
    // {
    //     printf("error staus %d\n", exit_status);
    // }
    // return 0;