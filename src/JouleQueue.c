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
#include <assert.h> //assert
#include "JouleQueue.h"
#include "common.h"

static void * default_app_engine(void * arg){
    volatile int x;
    x = 0;
    for(int i = 0; i< 1000; i++){
        x = x ^ i;
        x = x * i;
        x = x & i;
        x = x * i;
        x = x | i;
    }
    log_success("Default busy Wait x =%d",x);
}

int queue_init(queue_t * q, int num_threads, pthread_func app_engine){
    int status;
    status = pthread_attr_init (&q->attr);
    checkp(status);
    status = pthread_mutex_init (&q->mutex, NULL); 
    checkp(status);
    status = pthread_cond_init (&q->cv, NULL); 
    checkp(status);

    q->quit = 0;                    
    box_t * new = malloc(sizeof(box_t));       
    new->data = 0;
    new->next = NULL;

    //q->head = q->tail = new;
    q->head = q->tail = new;
    q->haz_list = NULL;

    q->parallelism = num_threads;       
    q->counter = 0;               
    q->idle = 0;                    
    q->app_engine = (app_engine == NULL) ? default_app_engine : app_engine;

    for(int i = 0; i< num_threads; i++){
        haz_t * node = (haz_t *) malloc(sizeof(haz_t));
        checkm(node);
        node->rtotal = 0;
        memset(node->rarray, 0, MAX_WASTE);
        memset(node->hazards, 0, MAX_HAZ_POINTERS);
        node->next = q->haz_list;
        q->haz_list = node;
    }

    return SUCCESS;
}

int queue_destroy(queue_t * q){
    pthread_attr_destroy(&q->attr);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cv);
    haz_t *link = q->haz_list;
    haz_t * prev;
    while(link != NULL){
        prev = link;
        link = link->next;
        free(prev);
    }
    return SUCCESS;
}

void * thread_start(void * var){
    return SUCCESS;
}

int queue_add(queue_t q, work_unit_t * data){

    return FAILURE;
}

int queue_change_work(queue_t q){
    return FAILURE;
}

//int enqueue_haz(queue_t * q, haz_t * h, work_unit_t data){
int enqueue_haz(queue_t * q, work_unit_t *data){

    assert(MAX_HAZ_POINTERS >=2);
    box_t * new = malloc(sizeof(box_t));
    checkm(new);
    new->data = *data;
    log_info("Enqueue internal data is %d", (*(int*)data));
    new->next = NULL;
    box_t  * tail, *  next, * changed;
    for(int i = 0;;i++){
        tail = q->tail;
       // h->hazards[0] = tail->data;
        if(tail != q->tail) continue;
        next = tail->next;
        //if(q->tail != tail) continue;
        xxx(0);
        if(next != NULL){
            xxx(1);

            while(1){ // help other threads
                changed = SYNC_CAS(&q->tail,tail, next);
                if(changed == tail){ 
                    xxx(2);
                    log_success("Made change #1: %d",i);
                    break;
                }else {
                    xxx(66);
                    log_info("Contention #1: %d",i);
                }
            }
            xxx(444);
            continue;
        }
        xxx(3);
        changed = SYNC_CAS(&tail->next,NULL,new);
        assert(tail->next == new);
        assert(tail->next->data == new->data);
        if( changed == NULL){
            xxx(4);
            tail = q->tail;
            assert(&(tail->next->data) == &new->data);
            break;
        }
        xxx(5);
        while(1){
            changed = SYNC_CAS(&q->tail,tail,new);
                if(changed == tail){ 
                    log_success("Made change #2: %d",i);
                    break;
                }else {
                    log_info("Contention #2: %d",i);
                }
        }
        
    }
    return SUCCESS;
}
/*int enqueue(queue_t * q, haz_t * h, work_unit_t data){

    assert(MAX_HAZ_POINTERS >=2);
    box_t * new = malloc(sizeof(box_t));
    checkm(new);
    new->data = data;
    new->next = NULL;
    box_t  * tail, *  next, * changed;
    for(int i = 0;;i++){
        tail = q->tail;
        h->hazards[0] = tail->data;
        next = tail->next;
        if(tail != q->tail){
            if(next == NULL){
                changed = SYNC_CAS(&tail->next,next, new);
                if(changed == next){ log_success("Made change #%d",1); break;}
                log_info("Contention #:%d",i);
            } else {
                changed = SYNC_CAS(&q->tail, tail, next->next);
            }
        }
        log_info("Loop running %d",i);
    }
    changed = SYNC_CAS(&q->tail, tail,new);
    if(changed == tail){
        log_success("Made change #%d",2);
        return SUCCESS;
    }
    return FAILURE;
}*/

/*int dequeue(queue_t * q, haz_t * h, work_unit_t data){
    for(;;){
        box_t *changed, *head, *tail, *next;
        head = q->head;
        h->hazards[0] = head->data;
        tail = q->tail;
        next = head->next;
        if(head == q->head){
            if(head == tail){
                if(next == NULL){
                    return FAILURE;
                }
                changed = SYNC_CAS(&q->tail, tail,next);
            }
        }
    }
    return SUCCESS;
}*/

int haz_traverse(haz_t *haz){
    haz_t *link = haz;
    int i = 0;
    while(link != NULL){

        link = link->next;
        log_info("Traverseing %d", ++i);
    }
    return SUCCESS;
}
#endif // _JouleQueue
