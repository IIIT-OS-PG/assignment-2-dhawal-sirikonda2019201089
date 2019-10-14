#include "background_server.h"
int logged_in=0;
pthread_mutex_t peer_data_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{

    void *ip_port_data = NULL;

    if(argc < 3)
    {
        printf("TOO LESS ARGS\n");
        return -1;
    }
    else
    {
        ip_port_data = ip_port_copy(argv);
    }

    pthread_t backgroud_server_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&backgroud_server_thread, &attr, background_server, ip_port_data);
    pthread_detach(backgroud_server_thread);

    peer_data *current_peer_data = new peer_data[1];
    int status = client_work(&logged_in, current_peer_data, peer_data_lock);

    if(status==-1)
    {
        printf("Error Occured in client work\n");
        return -2;
    }
    
    return 0;
}