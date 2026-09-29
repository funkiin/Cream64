#ifdef AAPI_VITA

#include <pthread.h>
#include <psp2/audioout.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "audio_api.h"

#define VITA_AUDIO_RATE 32000
#define VITA_AUDIO_PACKET_FRAMES 512
#define VITA_AUDIO_RING_FRAMES 8192

static int audio_port = -1;
static pthread_t audio_thread;
static pthread_mutex_t audio_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t audio_cond = PTHREAD_COND_INITIALIZER;
static bool audio_running;
static int16_t audio_ring[VITA_AUDIO_RING_FRAMES * 2];
static unsigned int audio_read_pos;
static unsigned int audio_write_pos;
static unsigned int audio_frames_queued;

static void *audio_vita_thread(void *unused) {
    (void)unused;
    int16_t packet[VITA_AUDIO_PACKET_FRAMES * 2];

    while (true) {
        pthread_mutex_lock(&audio_mutex);
        while (audio_running && audio_frames_queued < VITA_AUDIO_PACKET_FRAMES)
            pthread_cond_wait(&audio_cond, &audio_mutex);

        if (!audio_running) {
            pthread_mutex_unlock(&audio_mutex);
            break;
        }

        for (unsigned int i = 0; i < VITA_AUDIO_PACKET_FRAMES; i++) {
            packet[i * 2 + 0] = audio_ring[audio_read_pos * 2 + 0];
            packet[i * 2 + 1] = audio_ring[audio_read_pos * 2 + 1];
            audio_read_pos = (audio_read_pos + 1) % VITA_AUDIO_RING_FRAMES;
        }
        audio_frames_queued -= VITA_AUDIO_PACKET_FRAMES;
        pthread_mutex_unlock(&audio_mutex);

        sceAudioOutOutput(audio_port, packet);
    }

    return NULL;
}

static bool audio_vita_init(void) {
    audio_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, VITA_AUDIO_PACKET_FRAMES,
                                     VITA_AUDIO_RATE, SCE_AUDIO_OUT_MODE_STEREO);
    if (audio_port < 0) return false;

    audio_read_pos = 0;
    audio_write_pos = 0;
    audio_frames_queued = 0;
    audio_running = true;

    if (pthread_create(&audio_thread, NULL, audio_vita_thread, NULL) != 0) {
        audio_running = false;
        sceAudioOutReleasePort(audio_port);
        audio_port = -1;
        return false;
    }

    return true;
}

static int audio_vita_buffered(void) {
    int result;
    pthread_mutex_lock(&audio_mutex);
    result = (int)audio_frames_queued;
    pthread_mutex_unlock(&audio_mutex);
    return result;
}

static int audio_vita_get_desired_buffered(void) {
    return 2048;
}

static void audio_vita_play(const uint8_t *buf, size_t len) {
    const int16_t *samples = (const int16_t *)buf;
    unsigned int frames = (unsigned int)(len / 4);

    pthread_mutex_lock(&audio_mutex);

    unsigned int free_frames = VITA_AUDIO_RING_FRAMES - audio_frames_queued;
    if (frames > free_frames) frames = free_frames;

    for (unsigned int i = 0; i < frames; i++) {
        audio_ring[audio_write_pos * 2 + 0] = samples[i * 2 + 0];
        audio_ring[audio_write_pos * 2 + 1] = samples[i * 2 + 1];
        audio_write_pos = (audio_write_pos + 1) % VITA_AUDIO_RING_FRAMES;
    }
    audio_frames_queued += frames;

    pthread_cond_signal(&audio_cond);
    pthread_mutex_unlock(&audio_mutex);
}

static void audio_vita_shutdown(void) {
    if (audio_port < 0) return;

    pthread_mutex_lock(&audio_mutex);
    audio_running = false;
    pthread_cond_signal(&audio_cond);
    pthread_mutex_unlock(&audio_mutex);

    pthread_join(audio_thread, NULL);
    sceAudioOutOutput(audio_port, NULL);
    sceAudioOutReleasePort(audio_port);
    audio_port = -1;
}

struct AudioAPI audio_vita = {
    audio_vita_init,
    audio_vita_buffered,
    audio_vita_get_desired_buffered,
    audio_vita_play,
    audio_vita_shutdown
};

#endif
