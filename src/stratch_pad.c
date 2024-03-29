
/* Hello World program */

#include <stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<inttypes.h>

#define STR(x)   #x
#define SHOW_DEFINE(x) printf("%s=%s\n", #x, STR(x))

static inline uint64_t rdtsc (void) {
    unsigned l, u;
    __asm__ __volatile__("rdtsc" : "=a" (l), "=d" (u));
    return ((uint64_t)u << 32) | l;
}

#ifndef GET_CACHE_LINE_SIZE_H_INCLUDED
#define GET_CACHE_LINE_SIZE_H_INCLUDED

// Author: Nick Strupat
// Date: October 29, 2010
// Returns the cache line size (in bytes) of the processor, or 0 on failure

#include <stddef.h>
size_t cache_line_size();

#if defined(__APPLE__)

#include <sys/sysctl.h>
size_t cache_line_size() {
    size_t line_size = 0;
    size_t sizeof_line_size = sizeof(line_size);
    sysctlbyname("hw.cachelinesize", &line_size, &sizeof_line_size, 0, 0);
    return line_size;
}

#elif defined(_WIN32)

#include <stdlib.h>
#include <windows.h>
size_t cache_line_size() {
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
    return line_size;
}

#elif defined(linux)

#include <stdio.h>
size_t cache_line_size() {
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    unsigned int i = 0;
    if (p) {
        fscanf(p, "%d", &i);
        fclose(p);
    }
    return i;
}

#else
#error Unrecognized platform
#endif

#endif
typedef struct foo {
    int data;
}foo;
int changeit(void * d, void * r){


    foo * temp = d;
    temp->data = 15;
    return 1;
}

int main()
{
    uint64_t val;
    size_t cache_size;
    printf("Hello World\n");
    SHOW_DEFINE(__ELF__);
    SHOW_DEFINE(__APPLE__);
    SHOW_DEFINE(__MACH__);
    val = rdtsc();
    printf(  "CPU Clock %" PRIu64 "\n", val);
    cache_size = cache_line_size();
    printf("Cache line size: %" PRIuPTR "\n", (uintptr_t) cache_size);
    foo * f = malloc(sizeof(foo));
    foo * ret = malloc(sizeof(foo));
    f->data = 5;
    changeit((void*)f, (void*)ret);
    fprintf(stdout, "%d\n", f->data);


}

