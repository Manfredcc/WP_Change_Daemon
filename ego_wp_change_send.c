#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define MAX_MSG_SIZE 2048
#define MSG_QUEUE           "/ego_msg"

int main(int argc, char *argv[])
{
    int md;
    struct mq_attr msg_attr;
    msg_attr.mq_maxmsg = 10;
    msg_attr.mq_msgsize = MAX_MSG_SIZE;

    if (2 != argc) {
        fprintf(stderr, "Usage: %s 'message'\n", argv[0]);
        return 1;
    }

    md = mq_open(MSG_QUEUE, O_CREAT | O_RDWR, 0644, &msg_attr);
    if (-1 == md) {
        fprintf(stderr, "Creating message queue failed\n");
        return 1;
    }

    if (-1 == (mq_send(md, argv[1], strlen(argv[1]), 1))) {
        fprintf(stderr, "Message queue send failed\n");
        return 1;
    }

    mq_close(md);
    return 0;
}