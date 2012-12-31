#include "minunit.h"
#include "common.h"
#include "JouleQueue.h"

#ifndef _JouleTest
#define _JouleTest


void * test_init(){
    int t = 8;
    int status;
    queue_t q;
    status = queue_init(&q,t,NULL);
    mu_success(status);
    mu_truthy(q.head);
    mu_truthy(q.tail);
    mu_eq(q.head, q.tail);
    mu_falsey(q.quit);
    mu_truthy(q.parallelism);
    mu_truthy_m(q.app_engine,"Always needs engine.");
    mu_truthy(q.haz_list);
    haz_t * link = q.haz_list;
    mu_truthy(link);
    for(int x = 0; x< t; x++){
        mu_truthy(link);
        link = link->next;
    }
    mu_falsey(link);
    queue_destroy(&q);
    return SUCCESS;
}

void * test_enqueue(){

    int t = 8;
    int status;
    queue_t q;
    status = queue_init(&q,t,NULL);
    mu_success(status);
    int * tdata = malloc(sizeof(work_unit_t));
    *tdata = 10;
    enqueue_haz(&q, (void*) tdata);
    mu_success(status);
    mu_truthy(q.tail->next);
    
    mu_eq(*(int*)&(q.head->next)->data,*(int*)tdata);
    mu_eq(*(int*)&(q.tail->next)->data,*(int*)tdata);

    int * tdata2 = malloc(sizeof(work_unit_t));
    *tdata2 = 5;
    enqueue_haz(&q, (void*) tdata2);

    /*
    log_info("Head Data is %d",*(int*)&(q.head->next->data));
    log_info("Tail Data is %d",*(int*)&(q.tail->next->data));
    log_info("Head Next Data is %d",*(int*)&(q.head->next->next->data));
    */
    mu_eq(*(int*)&(q.head->next)->data,*(int*)tdata);
    mu_neq(*(int*)&(q.head->next->next)->data,*(int*)tdata);
    mu_eq(*(int*)&(q.tail->next->data),5);
    
    int * tdata3 = malloc(sizeof(work_unit_t));
    *tdata3 = 3;
    enqueue_haz(&q, (void*) tdata3);

    /*
    log_info("Head Data is %d",*(int*)&(q.head->next->data));
    log_info("Tail Data is %d",*(int*)&(q.tail->next->data));
    log_info("Head Next Data is %d",*(int*)&(q.head->next->next->data));
    log_info("Head Next Next Data is %d",*(int*)&(q.head->next->next->next->data));
    */
    mu_eq(*(int*)&(q.tail->data),*(int*)tdata2);
    mu_eq(*(int*)&(q.tail->next->data),3);
    
    return SUCCESS;
}

void * all_tests()
{
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_enqueue);
    return SUCCESS;
}


RUN_TESTS(all_tests)
#endif
