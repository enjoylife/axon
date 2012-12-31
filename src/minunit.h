/* File: minunit.h
 * Version: 1.0
 * Purpose: Definitions for testing 
 * Author: Matthew Clemens
 * Copyright: BSD 
*/


#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>
#include "common.h"


#define mu_suite_start() char *message = NULL

#define mu_truthy(test) if(!test){_log_it(TESTFAIL,"",1); }
#define mu_truthy_m(test, M) if(!test){_log_it(TESTFAIL,M,1); }

#define mu_success(test) if(test !=SUCCESS){_log_it(TESTFAIL,"",1); }
#define mu_success_m(test, M) if(test !=SUCCESS){_log_it(TESTFAIL,M,1); }

#define mu_falsey(test) if(test){_log_it(TESTFAIL,"",1); }
#define mu_falsey_m(test, M) if(test){_log_it(TESTFAIL,M,1); }

#define mu_eq(test, test2) if(test !=test2){_log_it(TESTFAIL,"",1); }
#define mu_neq(test, test2) if(test == test2){_log_it(TESTFAIL,"",1); }
#define mu_eq_m(test, test2, M) if(test != test2){_log_it(TESTFAIL,M,1); }

#define mu_run_test(test) printf("\n-----%s", " " #test "\n"); message = test(); tests_run++; if (message) return message;


#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; \
    fprintf( stderr, "------\n" "[" BOLDCYAN "RUNNING:" RESET "]"" %s \n", argv[0]);\
        char *result = name();\
        if (result != 0) {\
            fprintf( stderr,"\n\n[" BOLDRED "FAILED: %s" RESET "]" "\n", result);\
        }\
        else {\
            fprintf(stderr, "\n\n[" BOLDGREEN "ALL TESTS PASSED" RESET "]" "\n");\
        }\
    fprintf(stderr, "Tests run: %d\n", tests_run);\
        exit(result != 0);\
}

int tests_run;

#endif
