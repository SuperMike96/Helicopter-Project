#ifndef CIRCBUFT_H_
#define CIRCBUFT_H_

/* *****************************************************************************
 * circBufT.h
 *
 * Support for a circular buffer of uint32_t values on the Tiva processor.
 * P.J. Bones UCECE, Modified by Hangwen Hu and Marc Katzef
 * Last modified:  3.6.2017
 */

#include <stdint.h>

/* *****************************************************************************
 * Buffer structure
 * Stores buffer properties and a pointer to the data array.
 */
typedef struct {
	uint32_t size;		// number of entries in buffer
	uint32_t windex;	// index for writing, mod(size)
	uint32_t rindex;	// index for reading, mod(size)
	uint32_t *data;		// pointer to the data
} circBuf_t;

/* *****************************************************************************
 * initCircBuf: initialise the circBuf instance. Resets both indices to the
 * start of the buffer.  Dynamically allocates and clears the memory and returns
 * a pointer for the data.  Returns NULL if allocation fails.
 */
uint32_t *
initCircBuf (circBuf_t *buffer, uint32_t size);

/* *****************************************************************************
 * writeCircBuf: inserts entry at the current windex location, advances windex,
 * modulo buffer size.
 */
void
writeCircBuf (circBuf_t *buffer, uint32_t entry);

/* *****************************************************************************
 * readCircBuf: returns entry at the current rindex location, advances rindex,
 * modulo buffer size. The function does not check if reading has advanced ahead
 * of writing.
 */
uint32_t
readCircBuf (circBuf_t *buffer);

/* *****************************************************************************
 * freeCircBuf: releases the memory allocated to the buffer data, sets pointer
 * to NULL and other fields to 0. The buffer can be re-initialised by another
 * call to initCircBuf().
 */
void
freeCircBuf (circBuf_t *buffer);

/* *****************************************************************************
 * circBufMean: calculates the mean of the current entries in the buffer's data
 * array. Returns the rounded result.
 */
uint32_t
circBufMean (circBuf_t *buffer);

#endif /*CIRCBUFT_H_*/
