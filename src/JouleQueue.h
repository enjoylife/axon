/* File:  JouleQueue.h
 * Version: 1.0
 * Purpose: Public datatypes and functions for supporting the JouleQueue api
 * Author: Matthew Clemens
 * Copyright: Modified BSD, see LICENSE for more details 
*/
/* What is JouleQueue?
 * It is a thread conscious worker queue with the ablity to have a
 * work processing flow behave in chain gang esque configuration.
 * In other words multiple threads may be simultaneously consuming and 
 * producing work and this work may be then consumed by other JouleQueues
 * waiting on work to be handed off to them.
*/

#ifndef _JouleQueue_h
#define _JouleQueue_h

#include <pthread.h> 
#include "common.h"

#define MAX_HAZ_POINTERS 2
#define MAX_WASTE 40

typedef void * work_unit_t;
typedef void *(*pthread_func)(void*);

// to hold the data to be worked on
typedef struct box {
    work_unit_t  data;
    struct box * next;
} box_t;

// per thread stuff for hazard pointers.
typedef struct haz_node {
    struct haz_node * next;       // linked list 
    int rtotal;                   //occupancy
    work_unit_t rarray[MAX_WASTE];// to be released
    work_unit_t hazards[MAX_HAZ_POINTERS]; // held memory
} haz_t;

typedef struct queue {
    box_t *          head; 
    box_t *          tail;
    haz_t *          haz_list;   
    pthread_func     thread_initilizer;
    pthread_func     internal_engine;
    pthread_func     app_engine;
    pthread_func     thread_destroyer;
    pthread_mutex_t  mutex;
    pthread_cond_t   cv;             /* wait for work */
    pthread_attr_t   attr;           /* create detached threads */
    int              quit;           /* set when workq should quit */
    int              parallelism;    /* number of threads required */
    int              counter;        /* current number of threads */
    int              idle;           /* number of idle threads */
} queue_t;

int haz_init(haz_t * haz , int num_threads );
int queue_init(queue_t * q, int threads, pthread_func app_engine);
int queue_destroy(queue_t * q);

int enqueue(queue_t * q, haz_t * h, work_unit_t data);
int enqueue_haz(queue_t * q, work_unit_t *data);


#endif // _JouleQueue_h

