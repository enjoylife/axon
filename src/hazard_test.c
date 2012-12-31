#include "minunit.h"
#include "common.h"
#include "hazard.h"

char * test_init(){
    mu_truthy(0,"Needs to be zero");
    mu_truthy(1,"Needs to be zero");
    return 0;
}
char * test_grow(){
    return 0;
}
char * test_shrink(){
    return 0;
}

char * test_insert(){
    return 0;
}


char * all_tests()
{
    mu_suite_start();
    mu_run_test(test_init);
    mu_run_test(test_grow);
    mu_run_test(test_shrink);
    mu_run_test(test_insert);
    return 0;
}


RUN_TESTS(all_tests)
