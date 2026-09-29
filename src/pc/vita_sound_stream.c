#ifdef TARGET_VITA

#include <stddef.h>
#include <stdint.h>

#include <psp2/io/fcntl.h>

#include "platform.h"
#include "vita_sound_stream.h"

#define VITA_SOUND_STREAM_BASE 0x10000000u
#define VITA_SOUND_STREAM_SPAN 0x08000000u
#define VITA_SOUND_STREAM_PATH "app0:/res/sound/sound_data.tbl"

extern unsigned char gSoundDataRaw[];

static SceUID s_sound_data_fd = -1;

static void vita_sound_stream_open(void) {
    if (s_sound_data_fd >= 0) {
        return;
    }

    s_sound_data_fd = sceIoOpen(VITA_SOUND_STREAM_PATH, SCE_O_RDONLY, 0);
    if (s_sound_data_fd < 0) {
        sys_fatal("could not open Vita sound data '%s' (0x%08X)",
                  VITA_SOUND_STREAM_PATH, (unsigned int)s_sound_data_fd);
    }
}

void *vita_sound_stream_resolve(const void *ptr) {
    if (ptr != (const void *)gSoundDataRaw) {
        return (void *)ptr;
    }

    vita_sound_stream_open();
    return (void *)(uintptr_t)VITA_SOUND_STREAM_BASE;
}

int vita_sound_stream_is_address(uintptr_t addr, size_t size) {
    if (addr < VITA_SOUND_STREAM_BASE) {
        return 0;
    }

    uintptr_t offset = addr - VITA_SOUND_STREAM_BASE;
    if (offset >= VITA_SOUND_STREAM_SPAN) {
        return 0;
    }

    if (size > VITA_SOUND_STREAM_SPAN - offset) {
        return 0;
    }

    return 1;
}

int vita_sound_stream_read(uintptr_t addr, void *dst, size_t size) {
    vita_sound_stream_open();

    uintptr_t offset = addr - VITA_SOUND_STREAM_BASE;
    int read = sceIoPread(s_sound_data_fd, dst, (SceSize)size, (SceOff)offset);
    if (read < 0) {
        sys_fatal("sound_data.tbl read failed at 0x%08X size 0x%X (0x%08X)",
                  (unsigned int)offset, (unsigned int)size, (unsigned int)read);
    }

    if ((size_t)read != size) {
        sys_fatal("short sound_data.tbl read at 0x%08X: wanted 0x%X, got 0x%X",
                  (unsigned int)offset, (unsigned int)size, (unsigned int)read);
    }

    return 0;
}

#endif
