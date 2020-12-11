#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<pthread.h>
#include <sys/types.h>
#include <linux/userfaultfd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <stdio_ext.h>
#include "mpk_apis.h"
#include <mpt/mpt.h>
#include <mpt/pkey.h>
#include <mpk_heap.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                  } while (0)














#define DOMAIN 15
#define M_TIMES 5


int vpkeys[DOMAIN];

int main(void)
       {

/**/

       _init(0.5);
       char* addr[DOMAIN];
    int i =0;
    for(i = 0; i< DOMAIN; i++) {
    vpkeys[i] = rwmmap((void**)&addr[i]);
    }

    printf("Creating 15 domains each with one page memory.........\n");


/*Do that 500 times*/

int m = 0;
int p = 0;
for(m=0; m <M_TIMES; m++) {

/*Change permission to each domain*/
    for(p = 0; p<DOMAIN; p++) {
        BRIDGE_DOMAINRW(vpkeys[p]);
        for(i =0;i< 4096;i++) {
            addr[p][i] = (0x41+p)%128; /*just to keep tthings ascii*/
                }
        for(i =0;i< 4096;i++)
        printf("%c",addr[p][i]);
        printf("\n");
	    EXIT_DOMAIN(vpkeys[p]);

    }

}

   /*Release keys and pages*/
    for(i= 0; i<DOMAIN; i++)
        DESTROY_DOMAIN(vpkeys[i]);
    return 0;
}
