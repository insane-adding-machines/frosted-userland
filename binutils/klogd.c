#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
int main()
{
    int klog = open("/dev/klog", O_RDONLY);
    if (klog >= 0){
        char logbuf[32];
        int ret;
        while(1) { 
            ret = read(klog, logbuf, 31);
            if (ret > 0) {
                logbuf[ret] = '\0';
                fprintf(stderr, "%s", logbuf);
            }
        }
    } else {
        fprintf(stderr, "Cannot open /dev/klog. Is kernel debug enabled?\r\n");
        exit(111);
    }
}
