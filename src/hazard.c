/* File:  hazard.c
 * Version: 1.0
 * Purpose: Implementation of thread safe memory handling. Based on,
 * "Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects" - Maged M. Michael
 * Author: Matthew Clemens
 * Copyright: Modified BSD, see LICENSE for more details 
 * Misc: The methods described in the above paper are covered under patent, US8250047 B2. 
 * Details can be found at https://docs.google.com/a/google.com/viewer?url=patentimages.storage.googleapis.com/pdfs/US8250047.pdf
 * The legality of this implementation has yet to be dicussed with the patent holders.
*/

#ifndef _hazard_c
#define _hazard_c

#include <stdlib.h>
#include <string.h> // memset
#include <assert.h>
#include "Hazard.h" 
#include "tls.h"

// per thread stuff for hazard pointers.
typedef struct haz_node {
    struct haz_node * next;           // linked list 
    int rtotal;                       // current occupancy of rarray 
    void * rarray[MAX_WASTE];         // memory scheduled to be released
    void * hazards[MAX_HAZ_POINTERS]; // stuff being modified by this thread
} haz_node_t;

// holds our list, plus extras
typedef struct haz_thread_pool {
    int size;                         // number of threads  (aka size of haz_list)
    haz_node_t * haz_list; 
} haz_thread_pool_t;


haz_thread_pool_t * haz_init( int num){

    haz_thread_pool_t * haz = malloc(sizeof(haz_thread_pool_t));
    haz->size = num;

    haz_node_t  * link = NULL; // create our links starting from the end
    for(int i=num; i>=0; --i){
        haz_node_t * node = malloc(sizeof(haz_node_t));
        node->rtotal = 0;
        memset(node->rarray, 0, MAX_WASTE);
        memset(node->hazards, 0, MAX_HAZ_POINTERS);
        node->next = link;
        link = node;
    }
    haz->haz_list = link; // all went well, link! 

    return haz; 
}

int haz_reclaim(void * mem){
    assert(mem);
    free(mem);
    return SUCCESS;
} 

int hazard_cleaner(haz_thread_pool_t toxic_pool[]);

#endif // _hazard_c
