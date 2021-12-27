#include "os.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "string.h"

#if OS_MALLOC == ON

/*
 * Private constants
 */

#define OS_HEAP_CHUNK_COUNT (OS_HEAP_SIZE/OS_HEAP_CHUNK_SIZE)

/*
 * Private data
 */

static unsigned char heap[OS_HEAP_CHUNK_COUNT][OS_HEAP_CHUNK_SIZE];

// TODO Should be zero-init in .bss
static struct {
    void * ptr;
} chunks[OS_HEAP_CHUNK_COUNT];

/*
 * Public functions
 */

void * os_calloc(unsigned int sz)
{
    void *ptr = os_malloc(sz);
    if (ptr != NULL_PTR) memset(ptr, 0, sz);
    return ptr;
}

void * os_malloc(unsigned int sz)
{
    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
    {
        // Chunk is not free
        if (chunks[i].ptr != NULL_PTR) continue;

        int chunks_needed = 0;
        int remaining = sz;

        // Look for contiguous space
        for (int j = i ; j < OS_HEAP_CHUNK_COUNT ; j++)
        {
            if (chunks[j].ptr != NULL_PTR)
            {
                // Skip the next blocks
                i += chunks_needed;
                break;
            }

            remaining -= OS_HEAP_CHUNK_SIZE;
            chunks_needed++;

            if (remaining <= 0)
            {
                void * ptr = &heap[i];

                // Mark chunks
                for (int j = 0 ; j < chunks_needed ; j++)
                    chunks[i+j].ptr = ptr;

                return ptr;
            }
        }
    }

    return 0;
}

unsigned int os_get_used_heap()
{
    unsigned int used_heap = 0;

    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if (chunks[i].ptr != NULL_PTR)
            used_heap += OS_HEAP_CHUNK_SIZE;

    return used_heap;
}

unsigned int os_get_free_heap()
{
    unsigned int free_heap = 0;

    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if (chunks[i].ptr == NULL_PTR)
            free_heap += OS_HEAP_CHUNK_SIZE;

    return free_heap;
}

unsigned int os_get_total_heap()
{
    return OS_HEAP_SIZE;
}

void os_free(void * ptr)
{
    for (unsigned int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if ( chunks[i].ptr == ptr)
            chunks[i].ptr = NULL_PTR;
}

#endif // OS_MALLOC
