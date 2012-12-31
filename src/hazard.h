/* File:  hazard.h
 * Version: 1.0
 * Purpose: Public datatypes and functions for thread safe memory handling. Based on,
 * "Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects" - Maged M. Michael
 * Author: Matthew Clemens
 * Copyright: Modified BSD, see LICENSE for more details 
 * Disclaimer: The methods described in the above paper are covered under patent, US8250047 B2. 
 * Details can be found at https://docs.google.com/a/google.com/viewer?url=patentimages.storage.googleapis.com/pdfs/US8250047.pdf
 * The legality of this implementation has yet to be dicussed with the patent holders.
*/

#ifndef _hazard_h
#define _hazard_h

#define MAX_HAZ_POINTERS 2
#define MAX_WASTE 40

typedef enum HSTATUS {
    SUCCESS, 
    FAILURE,
} HSTATUS;

int haz_reclaim(void * mem);


#endif // _hazard_h
