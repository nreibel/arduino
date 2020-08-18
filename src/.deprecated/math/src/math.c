#include "math.h"
#include "types.h"

/*
 * Iterative calculation of a buffer of uint16 values
 */
uint16_t Math_Average_u16(uint16_t *values, uint16_t size)
{
    uint32_t buffer = 0;
    uint16_t cpt = 0;

    for(cpt = 0 ; cpt < size ; cpt++)
    {
        buffer = ((buffer*cpt)+values[cpt])/(cpt+1);
    }

    return (uint16_t) buffer;
}
