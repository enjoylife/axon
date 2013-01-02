/* File:  JouleQueue.c
 * Version: 1.0
 * Purpose: Implementation of a fast thread  conscious worker queue.
 * Author: Matthew Clemens
 * Copyright: Modified BSD, see LICENSE for more details 
*/
#ifndef _JouleQueue
#define _JouleQueue
#include <stdlib.h>
#include <string.h> //memset
#include <pthread.h> 
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "JouleQueue.h"
#include "common.h"

// passed to threads on initialization
typedef struct start_type {
    jq_t q;
    jq_thread_t * thread; // pass our specfic thread data
} start_type_t;

static void * default_app_engine(void * arg);
static void * thread_initilizer(void * arg);

/* Creates and waits for complete setup of threads, then gets out of their way. */
int jq_init(jq_t q, jq_config_t * config){

    // pthread stuff
    int status;
    status = pthread_attr_init (&q->attr);
    checkp(status);
    status = pthread_attr_setdetachstate (
        &q->attr, PTHREAD_CREATE_DETACHED);
    checkp(status);
    status = pthread_mutex_init (&q->mutex, NULL); 
    checkp(status);
    status = pthread_cond_init (&q->cv, NULL); 
    checkp(status);

    // internal counters
    q->counter = 0;               
    q->running = 0;                    
    q->quit = 0;                    

    // queue
    job_t * new = malloc(sizeof(job_t));       
    new->data = 0;
    new->next = NULL;
    // first node is always a dummy node
    q->head = q->tail = new;

    // config stuff
    q->app_engine = 
        (config == NULL) ? default_app_engine : config->app_engine;       
    q->parallelism = 
        (config == NULL) ? JQ_DEFAULT_THREAD_COUNT : 
            (config->max_threads <= JQ_MAX_THREAD_COUNT) ?
                config->max_threads : JQ_MAX_THREAD_COUNT;       

    // we need to hold on to mutex now or else a speedy thread might signal 
    // before were done seting up
    pthread_mutex_lock(&q->mutex);

    // setup each thread
    q->thread_list = NULL;
    start_type_t  * start = malloc(sizeof(start_type_t));
    for(int i = 0; i< q->parallelism; i++){
        jq_thread_t * node = (jq_thread_t *) malloc(sizeof(jq_thread_t));
        checkm(node);

        node->jqid = i;
        node->tstate = TS_ALIVE;

        node->next = q->thread_list;
        q->thread_list = node;

        memset(node->hazards, 0, MAX_HAZ_POINTERS);
        //node->rtotal = 0;
        //memset(node->rarray, 0, MAX_WASTE);

        start->q = q;
        start->thread = node; //pass in this threads stuff;

        pthread_create(&node->ptid, NULL, thread_initilizer, (void*)start);
        log_info("Pid %p", node->ptid);

    }
    // wait for them to be all setup, and allow them to continue initilizing
    while(pthread_cond_wait(&q->cv, &q->mutex));
    pthread_mutex_unlock(&q->mutex);
    log_info("threads ready %d", q->running);
    // commence work
    return SUCCESS;
}

int dequeue(queue_t * q, jq_thread_t * h, void ** data){
    job_t *head, *tail, *next, *changed;
    //log_warn("Start DEQUEUE",1);
    for(int i = 0;;i++){
        head = q->head;
        //h->hazards[0] = head;
        if(q->head != head) continue;
        tail = q->tail;
        next = head->next;
        //h->hazards[1] = next;
        if(q->head != head) continue;
        if(next == NULL) return EMPTY;
        if(head == tail) {
            while(1){ // help other threads
                changed = SYNC_CAS(&q->tail,tail, next);
                if(changed == tail){ 
                    //log_success("Made change #1: %d",i);
                    break;
                }else {
                    //log_info("Contention #1: %d",i);
                }
            }
            continue;
        }
        *data = next->data;
        //log_info("DATA IS: %d",*(int*)data);
        changed = SYNC_CAS(&q->head,head,next);
        if( changed == head){
            //log_success("Made change #2: %d",i);
            break;
        }
    }
    //remove_box(head);
    //log_warn("END DEQUEUE",1);
    return SUCCESS;
}

void * thread_initilizer(void * arg){

    struct timeval t;
    struct timespec timeout;
    //jq_t  q = (jq_t ) arg;
    start_type_t * start = (start_type_t *) arg;
    jq_t q = start->q;
    jq_thread_t * thread_specific = start->thread;

    pthread_mutex_lock(&q->mutex); // can't finish before main
    int new = SYNC_ADD(&q->running,1);
    pthread_mutex_unlock(&q->mutex);

    //log_info("PID %p", (void*)pthread_self());
    if(new == q->parallelism){ // were all ready tell main
        pthread_cond_signal(&q->cv);
    }

    // testing
    int max_work = 1000;
    int * saved_work = malloc(sizeof(void *) * max_work);
    int work_counter = 0;
    int empty_counter = 0;
    int * data = malloc(sizeof(void*));
    int status;
//    log_success("Got Data %d", *(int*)data);
    while(!q->quit){
    TRY:
        status = dequeue(q,NULL,(void*) &data);
        switch(status){
            case SUCCESS:
                empty_counter = 0;
                //log_success("SUCCESS %p",thread_specific->ptid);
                log_success("Data %d, Thread %p", *(int*)data, thread_specific->ptid);
                if(work_counter > max_work-1){
                    log_warn("ATE TOO MUCH",1);
                   // return saved_work;
                }
                else {
                    saved_work[work_counter] = *(int*)data;
                }
                work_counter++;
                goto TRY;
            case EMPTY:
              //  log_warn("EMPTY %d",empty_counter);
                gettimeofday(&t, NULL);
                TIMEVAL_TO_TIMESPEC(&t, &timeout);
                timeout.tv_sec += 2;
                //pthread_mutex_lock(&q->mutex);
                //status  = pthread_cond_timedwait(&q->cv, &q->mutex, &timeout);
                empty_counter++;
                //pthread_mutex_unlock(&q->mutex);
                //if (status == ETIMEDOUT) {
                    //log_warn("Try again %d",empty_counter);
                    sleep(2);
                    if( empty_counter >= 3){
                        log_warn("Worker wait timed out %d",empty_counter);
                        return saved_work;
                 //   }
                    //return SUCCESS;
                } else if (status == EINVAL){
                    log_warn("PTHREAD ERROR",1);
                } else {
                    log_warn("WEIRD WAKE",1);
                }
                break;
            case FAILURE:
                log_warn("FAILURE",1);
                break;
            default:
                log_warn("UNKONN",1);
                break;

        }
    }

    log_warn("I GUESS WERE LEAVING",1);
    //return saved_work; 
    return SUCCESS;
}

int jq_add(jq_t q, void * data){
    enqueue(q,NULL, data);
    return SUCCESS;
}

int queue_destroy(queue_t * q){
    pthread_attr_destroy(&q->attr);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cv);
    jq_thread_t *link = q->thread_list;
    jq_thread_t * prev;
    while(link != NULL){

        prev = link;
        link = link->next;
        free(prev);
    }
    return SUCCESS;
}

int enqueue(queue_t * q, jq_thread_t * h, void * data){
//int enqueue_haz(queue_t * q, work_unit_t *data){

    job_t * new = malloc(sizeof(job_t));
    checkm(new);
    new->data = data;
    new->flags = 0;
    new->next = NULL;
    job_t  * tail, *  next, * changed;
    for(int i = 0;;i++){
        tail = q->tail;
        //h->hazards[0] = h == NULL ? tail : NULL;
        if(tail != q->tail) continue;
        next = tail->next;
        //if(q->tail != tail) continue;
        if(next != NULL){

            while(1){ // help other threads
                changed = SYNC_CAS(&q->tail,tail, next);
                if(changed == tail){ 
                    //log_success("Made change #1: %d",i);
                    break;
                }else {
                    //log_info("Contention #1: %d",i);
                }
            }
            continue;
        }
        changed = SYNC_CAS(&tail->next,NULL,new);
        if( changed == NULL){
            assert(tail->next == new);
            assert(tail->next->data == new->data);
            assert(&(tail->next->data) == &new->data);
            tail = q->tail;
            //log_success("Made change #2: %d",i);
            break;
        }
        while(1){
            changed = SYNC_CAS(&q->tail,tail,new);
                if(changed == tail){ 
                    //log_success("Made change #3: %d",i);
                    break;
                }else {
                    //log_info("Contention #3: %d",i);
                }
        }
        
    }
    return SUCCESS;
}

int remove_box(job_t * box){
    free(box);
    return SUCCESS;
}


int job_traverse(job_t *box){
    job_t * link = box;
    log_info("Traversing",1);
    while(link != NULL){

        log_info("Data %d", *(int*)(link->data));
        link = link->next;
    }
    return SUCCESS;
}

int jq_thread_traverse(jq_t q){
    jq_thread_t *link = q->thread_list;
    int i = 0;
    while(link != NULL){
        log_info("Traverseing id:%d, pthread_t: %p", link->jqid, link->ptid);
        link = link->next;
    }
    return SUCCESS;
}

static void * default_app_engine(void * arg){
    sleep(1);
    return SUCCESS;
}
#endif // _JouleQueue
