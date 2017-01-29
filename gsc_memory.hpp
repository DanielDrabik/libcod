#ifndef _GSC_MEMORY_HPP_
#define _GSC_MEMORY_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

void gsc_memory_malloc();
void gsc_memory_free();
void gsc_memory_int_get();
void gsc_memory_int_set();
void gsc_memory_memset();

void gsc_binarybuffer_new();
void gsc_binarybuffer_free();
void gsc_binarybuffer_seek();
void gsc_binarybuffer_write();
void gsc_binarybuffer_read();

#ifdef __cplusplus
}
#endif

#endif
