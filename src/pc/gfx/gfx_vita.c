#ifdef WAPI_VITA

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#include <stdint.h>
#include <vitaGL.h>

#include "gfx_window_manager_api.h"

#define VITA_WIDTH 960
#define VITA_HEIGHT 544

static void gfx_vita_init(const char *window_title) {
    (void)window_title;

#ifdef VITA_MAX_CLOCKS
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);
#endif

    vglSetDisplayBufferCount(3);
    vglSetCircularPoolSize(2 * 1024 * 1024);
    vglInitExtended(0, VITA_WIDTH, VITA_HEIGHT, 24 * 1024 * 1024, SCE_GXM_MULTISAMPLE_NONE);
    vglWaitVblankStart(GL_TRUE);
}

static void gfx_vita_main_loop(void (*run_one_game_iter)(void)) {
    run_one_game_iter();
}

static void gfx_vita_get_dimensions(uint32_t *width, uint32_t *height) {
    *width = VITA_WIDTH;
    *height = VITA_HEIGHT;
}

static void gfx_vita_handle_events(void) {
}

static bool gfx_vita_start_frame(void) {
    return true;
}

static void gfx_vita_swap_buffers_begin(void) {
}

static void gfx_vita_swap_buffers_end(void) {
    vglSwapBuffers(GL_FALSE);
}

static double gfx_vita_get_time(void) {
    return (double)sceKernelGetProcessTimeWide() / 1000000.0;
}

static void gfx_vita_shutdown(void) {
#if defined(VITAGL_HAS_VGLEND)
    vglEnd();
#endif
}

struct GfxWindowManagerAPI gfx_vita = {
    gfx_vita_init,
    gfx_vita_main_loop,
    gfx_vita_get_dimensions,
    gfx_vita_handle_events,
    gfx_vita_start_frame,
    gfx_vita_swap_buffers_begin,
    gfx_vita_swap_buffers_end,
    gfx_vita_get_time,
    gfx_vita_shutdown
};

#endif
