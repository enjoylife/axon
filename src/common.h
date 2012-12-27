/* File: common.h
 * Version: 1.0
 * Purpose: Definitions for debugging, error checks, terminal colors, simple logic (min,max,etc), logging  
 * Author: Matthew Clemens
 * Copyright: Modified BSD, see LICENSE for more details 
*/

#ifndef _common_h
#define _common_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

// following common convetion for debugging
#ifndef NDEBUG
    #define DEBUG 1
#else
    #define DEBUG 0
#endif


/* Terminal colors  */
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"
#define MSGDEBUG  "[" BOLDCYAN "DEBUG" RESET "]"
#define MSGWARN  "[" BOLDYELLOW "WARN" RESET "]"
#define MSGSUCCESS  "[" BOLDGREEN "OK" RESET "]"
#define MSGINFO  "[" BOLDMAGENTA "INFO" RESET "]"
#define MSGERR  "[" BOLDRED "ERROR" RESET "]"


#define log_success(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGSUCCESS, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define log_warn(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] [E: %s] "M" \n",\
            MSGWARN ,__FILE__,__LINE__, (errno == 0 ? "n/a": strerror(errno)),##__VA_ARGS__); } while(0)

#define log_info(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGINFO, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define log_err(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGERR, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define debug(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGDEBUG, __FILE__, __LINE__,##__VA_ARGS__); } while(0)


#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define check_alt(A) if(!(A)) {goto error;}
#define check_hard(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; exit(1); }
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

// simple expanding macros
#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

#define EXPECT_TRUE(x)        __builtin_expect(!!(x), 1)
#define EXPECT_FALSE(x)       __builtin_expect(!!(x), 0)
#define COUNT_TRAILING_ZEROS  __builtin_ctz

#define VOLATILE_DEREF(x)  (*((volatile typeof(x))(x)))

// compile time assertions 
/* ASSERT_CONCAT taken from http://www.pixelbeat.org/programming/gcc/static_assert.html */
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#ifdef __COUNTER__ // gcc 
  #define STATIC_ASSERT(e,m) { enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }; }
#else // c89
  #define STATIC_ASSERT(e,m) { enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }; }
#endif

#ifndef _SINGLE_THREADED

#define MAX_NUM_THREADS  32 // make this whatever you want, but make it a power of 2

#define SYNC_SWAP(addr,x)         __sync_lock_test_and_set(addr,x)
#define SYNC_CAS(addr,old,x)      __sync_val_compare_and_swap(addr,old,x)
#define SYNC_ADD(addr,n)          __sync_add_and_fetch(addr,n)
#define SYNC_FETCH_AND_OR(addr,x) __sync_fetch_and_or(addr,x)
#else// _SINGLE_THREADED

#define MAX_NUM_THREADS  1

#define SYNC_SWAP(addr,x)         ({ typeof(*(addr)) _old = *(addr); *(addr)  = (x); _old; })
#define SYNC_CAS(addr,old,x)      ({ typeof(*(addr)) _old = *(addr); *(addr)  = (x); _old; })
//#define SYNC_CAS(addr,old,x)    ({ typeof(*(addr)) _old = *(addr); if ((old) == _old) { *(addr)  = (x); } _old; })
#define SYNC_ADD(addr,n)          ({ typeof(*(addr)) _old = *(addr); *(addr) += (n); _old; })
#define SYNC_FETCH_AND_OR(addr,x) ({ typeof(*(addr)) _old = *(addr); *(addr) |= (x); _old; })

#endif//_SINGLE_THREADED



#endif
