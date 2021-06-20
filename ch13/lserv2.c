#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/errno.h>
#include <unistd.h>

#define MSGLEN 128
#define RECLAIM_INTERVAL 60

int setup();
void narrate(char *, char *, struct sockaddr_in *);
void handle_request(char *, struct sockaddr_in *, socklen_t);

int main(int ac, char *av[]) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buf[MSGLEN];
    int ret, sock;
    
    void ticket_reclaim();
    unsigned time_left;

    sock = setup();
    signal(SIGALRM, ticket_reclaim);
    alarm(RECLAIM_INTERVAL);

    while (1) {
        addrlen = sizeof(client_addr);
        ret = recvfrom(sock, buf, MSGLEN, 0,(struct sockaddr *)&client_addr, &addrlen);
        if (ret != -1) {
            buf[ret] = '\0';
            narrate("GOT:", buf, &client_addr);
            time_left = alarm(0);
            handle_request(buf, &client_addr, addrlen);
            alarm(time_left);
        } else if (errno != EINTR)
            perror("recvfrom");
    }
    return 0;
}
