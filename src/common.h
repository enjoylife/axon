/* File: common.h
 * Version: 1.0
 * Purpose: Definitions for debugging, error checks, terminal colors, simple logic (min,max,etc), logging  
 * Author: Matthew Clemens
 * Copyright: BSD 
*/

#ifndef _common_h
#define _common_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

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

#ifndef NDEBUG
    #define DEBUG 1
#else
    #define DEBUG 0
#endif

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

#define log_success(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] "M" \n",MSGSUCCESS, __FILE__, __LINE__,##__VA_ARGS__); } while(0)

#define log_warn(M, ...) \
    do {if(DEBUG) fprintf(stderr,"%-20s[%s:%d] [E: %s] "M" \n",MSGWARN ,__FILE__,__LINE__, (errno == 0 ? "n/a": strerror(errno)),##__VA_ARGS__); } while(0)

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

/* ASSERT_CONCAT taken from http://www.pixelbeat.org/programming/gcc/static_assert.html */
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#ifdef __COUNTER__
  #define STATIC_ASSERT(e,m) \
    { enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }; }
#else
  #define STATIC_ASSERT(e,m) \
    { enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }; }
#endif


#endif
