#ifndef BACKGROUND_SERVER_H_
#define BACKGROUND_SERVER_H_
#include "peer.hpp"

struct ip_port
{
    char *ip;
    char *port;
};

void *ip_port_copy(char **);
void *background_server(void *);

#endif //!BACKGROUND_SERVER_H_