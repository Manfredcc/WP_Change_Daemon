#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <time.h>
#include <pthread.h>
#include <mqueue.h>

/*====================================================================
                    EGO_WP_CHANGE_DAEMON
DESCRIPTION:
    This is a wallpaper-changing daemon that starts at the beginning 
    of the system, based on feh. You can replace it with your command 
    on your PC.

    Date            Author          Description
    ------          --------        ------------------
    2023/12/04      Manfred         First release

====================================================================*/

/* Configuration */
#define DEFAULT_DURATION    5 * 60     /* second */
#define MSG_QUEUE           "/ego_msg"
#define DEFAULT_TYPE        SEQUENTIAL
#define CMD_SEQUENCE        "feh --bg-fill --recursive \
                                ~/02_resource/photo/wallpapers/select/"
#define CMD_RANDOM          "feh --bg-fill --randomize --recursive \
                                ~/02_resource/photo/wallpapers/select/"


/* Defination */
#define MAX_WP_NUM      30
#define MAX_LENGTH_WP   50

typedef enum _bool {
    false,
    true,
} bool;

typedef enum _EVT {
    ECT_WP_AUTO_SWITCH,
    EVT_DEFAULT_METHOD,
    EVT_SWITCH_WALLPAPER,
    EVT_RE_PROGRAM,
    EVT_MAX,
} EVT;

typedef struct _wp_name {
    char name[MAX_LENGTH_WP + 1];
} wp_name, *pwp_name;

typedef enum _SWITCH_TYPE {
    SEQUENTIAL,
    RANDOM,
    SWITCH_TYPE_MAX,
} SWITCH_TYPE;

typedef struct _egoist {
    char *name;
    wp_name wp_data[MAX_WP_NUM];
    int num_of_wp;
    int cur_wp;
    int duration;
    bool auto_change_enable;
    SWITCH_TYPE type;
    pthread_attr_t pthread_attr;
    pthread_t auto_change;
    pthread_t receiver;
    struct mq_attr msg_attr;
    char *msg;
    pthread_mutex_t mutex;
    bool is_initialized;
} egoist, *pegoist;
pegoist ego = NULL;

/* Prototype */
int init(int argc, char *argv[]);
void *auto_change(void *arg);
void switch_wp(SWITCH_TYPE type);
void get_wp_info(FILE *fp);

/*==============================MAIN FUNCTION==============================*/
int main(int argc, char *argv[])
{
    int ret = 0;

    do {
        ret = init(argc, argv);
    } while (0);

    if (ret) {
        fprintf(stderr, "Initialize failed\n");
        return 1;
    }

    for (;;) { /* Keep thread runs */
        sleep(300);
    }

    return 0;
}

/*==============================FUNC DEFINATION==============================*/
/*
 * Receiving message
 */
int receiver(void)
{
    int i;
    int md = mq_open(MSG_QUEUE, O_RDONLY | O_CREAT, 0644, NULL);
    if (-1 == md) {
        fprintf(stderr, "Open %s failed\n", MSG_QUEUE);
        return 1;
    }

    if (-1 == (mq_getattr(md, &ego->msg_attr))) {
        fprintf(stderr, "Get message attribute failed\n");
        return 1;
    }

    ego->msg = calloc(ego->msg_attr.mq_msgsize, sizeof(char));
    if (!ego->msg) {
        fprintf(stderr, "Failed to alloc mem for buff\n");
        return 1;
    }

    if (-1 == (mq_receive(md, ego->msg, ego->msg_attr.mq_msgsize, NULL))) {
        fprintf(stderr, "Message receive failed\n");
        return 1;
    }
    
    mq_close(md);

    return 0;
}

/*
 * Enable/Disable WP change automatically
 */
void wp_change_auto_enable(bool enable)
{
    ego->auto_change_enable = enable;
}

/*
 * Change default switch type for auto-change wp
 */
void wp_auto_default_change(int method)
{
    if (method < SWITCH_TYPE_MAX) {
        ego->type = method;
        fprintf(stdout, "%s:%d\n", __func__, method);
    } else {
        fprintf(stderr, "Error input parameter: %d\n", method);
    }
}

/*
 * Response for different msg
 */
void action(char *msg)
{
    int raw = atoi(msg);
    int evt = raw / 10;
    int code = raw % 10;

    switch (evt) {
    case ECT_WP_AUTO_SWITCH:
        wp_change_auto_enable(code);
        break;
    case EVT_DEFAULT_METHOD:
        wp_auto_default_change(code);
        break;
    case EVT_SWITCH_WALLPAPER:
        switch_wp(code);
        break;
    case EVT_RE_PROGRAM:
        //TODO;
        break;
    default:
        fprintf(stderr, "This message shouldn't be printf\n");
        break;
    }

    fprintf(stdout, "Event happened:%d code:%d\n", evt, code);
}

/*
 * A loop for receiving message
 */
void *receiver_loop(void *arg)
{
    do {
        if (receiver()) {
            fprintf(stderr, "something wrong with receiver()\n");
            break;
        }

        action(ego->msg);

    } while (1);

    memset(ego->msg, '\0', ego->msg_attr.mq_msgsize);
    free(ego->msg);
}


/*
 * Switch wallpaper by random or sequential
 */
void switch_wp(SWITCH_TYPE type)
{
    if (type < SWITCH_TYPE_MAX) {
        switch (type) {
        case RANDOM:
            system(CMD_RANDOM);
            break;
        case SEQUENTIAL:
        default:
            if (ego->cur_wp == ego->num_of_wp - 1) {
                ego->cur_wp = -1;
            }
            ego->cur_wp++;
            char *cmd = (char *)malloc(strlen(CMD_SEQUENCE) + strlen(ego->wp_data[ego->cur_wp].name));
            sprintf(cmd, CMD_SEQUENCE"%s", ego->wp_data[ego->cur_wp].name);
            system(cmd);
            break;
        }
    } else {
        fprintf(stderr, "Error input parameter: %d\n", type);
    }
}

/*
 * Get info from wallpaper list as members for wp_data
 */
void get_wp_info(FILE *fp)
{
    ego->num_of_wp = 0;

    /* get wp info */
    while (fgets(ego->wp_data[ego->num_of_wp].name, MAX_LENGTH_WP, fp) != NULL
                    && ego->num_of_wp < MAX_WP_NUM) {
        ego->num_of_wp++;
    }

    fclose(fp);
}

/*
 * Automatically change the wallpaper at predetermined intervals
 */
void *auto_change(void *arg)
{
    while (1) {
        if (ego->auto_change_enable) {
            switch_wp(ego->type);
        }
        sleep(ego->duration);
    }
}

/*
 * Initialize egoist and create thread for auto_change
 * Return 0 on success or 1 on failure
 */
int init(int argc, char *argv[])
{
    FILE *fp;
    int ret = 0;
    
    if (2 != argc) {
        fprintf(stderr, "Usage:%s <wallpaper list>\n", argv[0]);
        return 1;
    }
    
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Can't open %s\n", argv[1]);
        return 1;
    }

    ego = (pegoist)malloc(sizeof(*ego));
    if (!ego) {
        fprintf(stderr, "Failed to alloc mem for egoist\n");
        return 1;
    }

    ego->name = "egoist_wp_change";
    ego->duration = DEFAULT_DURATION;
    ego->type = DEFAULT_TYPE;
    ego->auto_change_enable = true;
    get_wp_info(fp);
    
    ret = pthread_attr_init(&ego->pthread_attr);
    ret |= pthread_create(&ego->auto_change, &ego->pthread_attr, auto_change, NULL);
    ret |= pthread_detach(ego->auto_change);
    ret |= pthread_create(&ego->receiver,&ego->pthread_attr, receiver_loop, NULL);
    ret |= pthread_detach(ego->receiver);
    if (ret != 0) {
        fprintf(stderr, "Error with pthread: %s\n", strerror(ret));
        return 1;
    }

    ego->is_initialized = true;
    return 0;
}

