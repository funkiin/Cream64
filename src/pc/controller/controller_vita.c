#ifdef CAPI_VITA

#include <psp2/ctrl.h>
#include <ultra64.h>

#include "controller_api.h"

static int stick_axis(unsigned char v) {
    int n = (int)v - 128;
    if (n > -12 && n < 12) return 0;
    n = (n * 80) / 127;
    if (n < -80) n = -80;
    if (n > 80) n = 80;
    return n;
}

static void controller_vita_init(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
}

static void controller_vita_read(OSContPad *pad) {
    SceCtrlData data;
    if (sceCtrlPeekBufferPositive(0, &data, 1) <= 0) return;

    const unsigned int buttons = data.buttons;

    if (buttons & SCE_CTRL_START) pad->button |= START_BUTTON;
    if (buttons & SCE_CTRL_SELECT) pad->button |= L_TRIG;

    if (buttons & SCE_CTRL_CROSS) pad->button |= A_BUTTON;
    if (buttons & SCE_CTRL_CIRCLE) pad->button |= A_BUTTON;
    if (buttons & SCE_CTRL_SQUARE) pad->button |= B_BUTTON;
    if (buttons & SCE_CTRL_TRIANGLE) pad->button |= B_BUTTON;

    if (buttons & SCE_CTRL_LTRIGGER) pad->button |= Z_TRIG;
    if (buttons & SCE_CTRL_RTRIGGER) pad->button |= R_TRIG;

    if (buttons & SCE_CTRL_UP) pad->button |= U_JPAD;
    if (buttons & SCE_CTRL_LEFT) pad->button |= L_JPAD;
    if (buttons & SCE_CTRL_DOWN) pad->button |= D_JPAD;
    if (buttons & SCE_CTRL_RIGHT) pad->button |= R_JPAD;

    const int rx = stick_axis(data.rx);
    const int ry = -stick_axis(data.ry);

    if (rx > 28) pad->button |= R_CBUTTONS;
    if (rx < -28) pad->button |= L_CBUTTONS;
    if (ry > 28) pad->button |= U_CBUTTONS;
    if (ry < -28) pad->button |= D_CBUTTONS;

    pad->stick_x = stick_axis(data.lx);
    pad->stick_y = -stick_axis(data.ly);
    pad->ext_stick_x = rx;
    pad->ext_stick_y = ry;
}

static u32 controller_vita_rawkey(void) {
    return VK_INVALID;
}

static void controller_vita_shutdown(void) {
}

struct ControllerAPI controller_vita = {
    VK_INVALID,
    controller_vita_init,
    controller_vita_read,
    controller_vita_rawkey,
    NULL,
    NULL,
    NULL,
    controller_vita_shutdown
};

#endif
