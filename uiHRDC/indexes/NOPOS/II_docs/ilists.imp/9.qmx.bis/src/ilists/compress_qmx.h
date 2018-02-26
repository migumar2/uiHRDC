/*
	COMPRESS_QMX.H
	--------------
*/
#ifndef COMPRESS_QMX_H_
#define COMPRESS_QMX_H_

#include <stdint.h>
#include <stdio.h>

/*
	class COMPRESS_QMX
	------------------
*/
class compress_qmx
{
private:
	uint8_t *length_buffer;
	uint64_t length_buffer_length;

public:
	compress_qmx();
	virtual ~compress_qmx();

	virtual void encodeArray(const uint32_t *in, uint64_t len, uint32_t *out, uint64_t *nvalue);
	virtual void decodeArray(const uint32_t *in, uint64_t len, uint32_t *out, uint64_t nvalue);
} ;

#endif

