#include "os_cfg.h"

#if OS_MALLOC == OS_MALLOC_CHUNKS

#include "os_mem.h"
#include "string.h"

/*
 * Private constants
 */

#define OS_HEAP_CHUNK_COUNT (OS_HEAP_SIZE/OS_HEAP_CHUNK_SIZE)

/*
 * Private functions prototypes
 */

static void* my_malloc(unsigned int sz);
static void my_free(void *ptr);
static unsigned int my_get_total();
static unsigned int my_get_free();
static unsigned int my_get_used();

/*
 * Private data
 */

static struct malloc_drv_s drv = {
    .malloc = my_malloc,
    .free = my_free,
    .get_free = my_get_free,
    .get_used = my_get_used,
    .get_total = my_get_total
};

static uint8_t heap[OS_HEAP_CHUNK_COUNT][OS_HEAP_CHUNK_SIZE];

// TODO Should be zero-init in .bss
static struct {
    void * ptr;
} chunks[OS_HEAP_CHUNK_COUNT];

/*
 * Exported data
 */

const malloc_drv_t malloc_drv = &drv;

/*
 * Private functions
 */

static void* my_malloc(unsigned int sz)
{
    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
    {
        // Chunk is not free
        if (chunks[i].ptr != NULL_PTR) continue;

        unsigned int chunks_needed = 0;
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
                for (unsigned int j = 0 ; j < chunks_needed ; j++)
                    chunks[i+j].ptr = ptr;

                return ptr;
            }
        }
    }

    return NULL_PTR;
}

static void my_free(void * ptr)
{
    for (unsigned int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if ( chunks[i].ptr == ptr)
            chunks[i].ptr = NULL_PTR;
}

static unsigned int my_get_used()
{
    unsigned int used_heap = 0;

    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if (chunks[i].ptr != NULL_PTR)
            used_heap += OS_HEAP_CHUNK_SIZE;

    return used_heap;
}

static unsigned int my_get_free()
{
    unsigned int free_heap = 0;

    for (int i = 0 ; i < OS_HEAP_CHUNK_COUNT ; i++)
        if (chunks[i].ptr == NULL_PTR)
            free_heap += OS_HEAP_CHUNK_SIZE;

    return free_heap;
}

static unsigned int my_get_total()
{
    return OS_HEAP_SIZE;
}

#endif // OS_MALLOC == OS_MALLOC_CHUNKS
