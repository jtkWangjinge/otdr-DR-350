#ifndef APP_MEMORY_DETECTER
#define APP_MEMORY_DETECTER

#include "guiconfig.h"
//#define DEBUG_MEMORY_DETECTER

#define  FILE_NAME_LENGTH            48
#define  OUTPUT_FILE                 MntSDcardDataDirectory"/memory_detecter.txt"

#ifdef DEBUG_MEMORY_DETECTER
#define  malloc(size)                xmalloc (size, __FILE__, __LINE__)
#define  calloc(elements, size)      xcalloc (elements, size, __FILE__, __LINE__)
#define  free(mem_ref)               xfree(mem_ref)
#endif

struct _MEM_INFO
{
    void            *address;
    unsigned int    size;
    unsigned int    line;
    char            file[FILE_NAME_LENGTH];
};
typedef struct _MEM_INFO MEM_INFO;

struct _MEM_LEAK {
    MEM_INFO mem_info;
    struct _MEM_LEAK * next;
};
typedef struct _MEM_LEAK MEM_LEAK;

void memory_detecter_init(void);
void add(MEM_INFO alloc_info);
void erase(unsigned pos, MEM_LEAK *last);
void clear(void);

void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
void xfree(void * mem_ref);

void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line);
void remove_mem_info (void * mem_ref);
void report_mem_leak(void);

#endif
