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
// #define DEFAULT_DURATION    5 * 60     /* second */
#define DEFAULT_DURATION    5
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
    SWITCH_TYPE type;
    // struct sigaction action;
    pthread_attr_t pthread_attr;
    pthread_t auto_change;
    pthread_mutex_t mutex;
    bool is_initialized;
} egoist, *pegoist;
pegoist ego = NULL;

/* Prototype */
int init(int argc, char *argv[]);
void *auto_change(void *arg);
void switch_wp(SWITCH_TYPE type);
void get_wp_info(FILE *fp);


int main(int argc, char *argv[])
{
    int ret = 0;

    do {
        ret = init(argc, argv);
    } while (0);

    for (;;) { /* Keep thread runs */
        sleep(300);
    }

    return 0;
}

void switch_wp(SWITCH_TYPE type)
{
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
        printf("cmd:%s\n", cmd);
        break;
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
    // printf("ego->wp_nums:%d\n", ego->wp_nums);
    printf("ego->num_of_wp:%d\n", ego->num_of_wp);

    fclose(fp);
}

/*
 * Automatically change the wallpaper at predetermined intervals
 */
void *auto_change(void *arg)
{
    while (1) {
        switch_wp(ego->type);
        printf("Hey auto_change\n");
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
    get_wp_info(fp);
    
    ret = pthread_attr_init(&ego->pthread_attr);
    ret |= pthread_create(&ego->auto_change, &ego->pthread_attr, auto_change, NULL);
    ret |= pthread_detach(ego->auto_change);
    if (ret != 0) {
        fprintf(stderr, "Error with pthread: %s\n", strerror(ret));
        return 1;
    }

    ego->is_initialized = true;
    printf("Initialize well\n");
    return 0;
}

