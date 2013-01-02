#ifndef _JouleTest
#define _JouleTest

#include "minunit.h"
#include "unistd.h"
#include "common.h"
#include "JouleQueue.h"

#define SETUPVARS \
    srand(time(NULL));\
    int status; \
    queue_t *q; \
    int * data_new;\
    int * data_old;\
    (void)data_new;\
    (void)data_old;

#define SETUPQUEUE \
    do { \
        q = malloc(sizeof(queue_t)); \
        status = jq_init(q,NULL); \
        mu_success(status);  \
    } while(0)

#define ENQUEUE \
    data_new =  malloc(sizeof(int)); \
    *data_new =  (rand() % 1000) + 1;\
    status = enqueue(q, NULL, (void *) data_new);\

#define DEQUEUE \
    status = dequeue(q,NULL, (void*)&data_old);

int test_init(){
    SETUPVARS;
    SETUPQUEUE;
    mu_truthy(q->head);
    mu_truthy(q->tail);
    mu_eq(q->head, q->tail);
    //mu_falsey(q->quit);
    mu_truthy(q->parallelism);
    mu_truthy(q->app_engine);
    mu_truthy(q->thread_list);
    jq_thread_t * link = q->thread_list;
    mu_truthy(link);
    for(int x = 0; x< q->parallelism; x++){
        mu_truthy(link);
        link = link->next;
    }
    mu_falsey(link);
    return SUCCESS;
}

int test_queue(){
    int num_iter = 10;
    SETUPVARS;
    SETUPQUEUE;
    for(int i =0; i< num_iter; i++){
        ENQUEUE;
        mu_success(status);
        mu_truthy(q->head->next->data);
        mu_truthy(q->head->next);
        mu_eq(*(int*)(q->tail->next)->data,*(int*)data_new);
    }
    for(int i = 0; i< num_iter; i++){
        DEQUEUE;
        mu_success(status);
    }
    DEQUEUE;
    mu_eq(status,EMPTY);
    DEQUEUE;
    mu_eq(status,EMPTY);
    //job_traverse(q->head->next);
    return SUCCESS;
}

int test_enqueue(){
    SETUPVARS;
    SETUPQUEUE;
    int * tdata = malloc(sizeof(work_unit_t));
    *tdata = 10;
    enqueue(q,NULL, (void*) tdata);
    mu_success(status);
    mu_truthy(q->tail->next);
    
    mu_eq(*(int*)(q->head->next)->data,*(int*)tdata);
    mu_eq(*(int*)(q->tail->next)->data,*(int*)tdata);

    int * tdata2 = malloc(sizeof(work_unit_t));
    *tdata2 = 5;
    enqueue(q, NULL,(void*) tdata2);

    /*
    log_info("Head Data is %d",*(int*)(q->head->next->data));
    log_info("Tail Data is %d",*(int*)(q->tail->next->data));
    log_info("Head Next Data is %d",*(int*)(q->head->next->next->data));
    */
    mu_eq(*(int*)(q->head->next)->data,*(int*)tdata);
    mu_neq(*(int*)(q->head->next->next)->data,*(int*)tdata);
    mu_eq(*(int*)(q->tail->next->data),5);
    
    int * tdata3 = malloc(sizeof(work_unit_t));
    *tdata3 = 3;
    enqueue(q, NULL,(void*) tdata3);


    mu_eq(*(int*)(q->tail->data),*(int*)tdata2);
    mu_eq(*(int*)(q->tail->next->data),3);
    //job_traverse(q->head->next);
    
    return SUCCESS;
}

int test_dequeue(){
    SETUPVARS;
    SETUPQUEUE;
    int * tdata = malloc(sizeof(work_unit_t));
    void * rdata = malloc(sizeof(work_unit_t));
    *tdata = 10;
    enqueue(q,NULL, (void*) tdata);
    tdata = malloc(sizeof(work_unit_t));
    *tdata = 8;
    enqueue(q,NULL, (void*) tdata);
    tdata = malloc(sizeof(work_unit_t));
    *tdata = 6;
    enqueue(q, NULL,(void*) tdata);
    //log_debug("STARTING DEQUEUE",1);
    status = dequeue(q,NULL, &rdata);
    mu_success(status);
    status = dequeue(q, NULL,&rdata);
    mu_success(status);
    status = dequeue(q, NULL,&rdata);
    mu_success(status);
    status = dequeue(q, NULL,&rdata);
    mu_truthy(status);
    mu_eq(status, EMPTY);
    //job_traverse(q->head->next);

    return SUCCESS;
}

int test_contention(){
    // we need to join on all threads and check the work that they did.
    // We are lookingfor errors, aka duplicated work, or contention on empty
    SETUPVARS;
    SETUPQUEUE;
    return SUCCESS;
}

int test_queue2(){
    int num_iter = 1000;
    srand(time(NULL));
    int status; 
    jq_t q; 
    int * data_new;
    int * data_old;
    q = malloc(sizeof(queue_t)); 
    status = jq_init(q,NULL); 
    mu_falsey(status);
    sleep(1);
    for(int i =0; i < num_iter; i++){

        //log_warn("ADDING %d",i);
        data_new =  malloc(sizeof(int));
        *data_new = i;
        status = enqueue(q, NULL, (void *) data_new);
        mu_success(status);
        mu_truthy(q->head->next->data);
        mu_truthy(q->head->next);
        mu_eq(*(int*)(q->tail->next)->data,*(int*)data_new);
        
        //log_info("SLEEP",1);
        //log_info("ENDSLEEP %d",i);
    }
    //job_traverse(q->head->next);
    jq_thread_traverse(q);
    return SUCCESS;
}

int all_tests() {
    MU_SUITE_BEGIN;
    mu_run_test(test_init);
    mu_run_test(test_enqueue);
    //sleep(2);
    mu_run_test(test_dequeue);
    mu_run_test(test_queue);
    mu_run_test(test_contention);
    mu_run_test(test_queue2);
    MU_SUITE_END;
}

RUN_TESTS(all_tests)
#endif
