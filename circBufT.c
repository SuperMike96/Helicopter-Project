/* *****************************************************************************
 * circBufT.c
 *
 * Support for a circular buffer of uint32_t values on the Tiva processor.
 * P.J. Bones UCECE, Modified by Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"


/* *****************************************************************************
 * initCircBuf: initialise the circBuf instance. Resets both indices to the
 * start of the buffer.  Dynamically allocates and clears the memory and returns
 * a pointer for the data.  Returns NULL if allocation fails.
 */
uint32_t *
initCircBuf (circBuf_t *buffer, uint32_t size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data = 
        (uint32_t *) calloc (size, sizeof(uint32_t));
	return buffer->data;
}


/* *****************************************************************************
 * writeCircBuf: inserts entry at the current windex location, advances windex,
 * modulo buffer size.
 */
void
writeCircBuf (circBuf_t *buffer, uint32_t entry)
{
	buffer->data[buffer->windex] = entry;
	buffer->windex++;
	if (buffer->windex >= buffer->size)
	   buffer->windex = 0;
}


/* *****************************************************************************
 * readCircBuf: returns entry at the current rindex location, advances rindex,
 * modulo buffer size. The function does not check if reading has advanced ahead
 * of writing.
 */
uint32_t
readCircBuf (circBuf_t *buffer)
{
	uint32_t entry;
	
	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	   buffer->rindex = 0;
    return entry;
}


/* *****************************************************************************
 * freeCircBuf: releases the memory allocated to the buffer data, sets pointer
 * to NULL and other fields to 0. The buffer can be re-initialised by another
 * call to initCircBuf().
 */
void
freeCircBuf (circBuf_t * buffer)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = 0;
	free (buffer->data);
	buffer->data = NULL;
}


/* *****************************************************************************
 * circBufMean: calculates the mean of the current entries in the buffer's data
 * array. Returns the rounded result.
 */
uint32_t
circBufMean (circBuf_t *buffer)
{
    uint32_t bufferSize = buffer->size;

    int64_t circBufSum = 0;
    uint32_t i;
    for (i = 0; i < bufferSize; i++) {
        circBufSum += readCircBuf(buffer);
    }

    uint32_t denom = 2 * bufferSize;
    int64_t num = 2*circBufSum + bufferSize;

    int32_t average = num / denom;
    return average;
}
