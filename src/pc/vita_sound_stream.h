#ifndef CREAM64_VITA_SOUND_STREAM_H
#define CREAM64_VITA_SOUND_STREAM_H

#include <stddef.h>
#include <stdint.h>

#ifdef TARGET_VITA
void *vita_sound_stream_resolve(const void *ptr);
int vita_sound_stream_is_address(uintptr_t addr, size_t size);
int vita_sound_stream_read(uintptr_t addr, void *dst, size_t size);
#endif

#endif
