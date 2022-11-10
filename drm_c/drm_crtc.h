#ifndef DRM_CRTC_H
#define DRM_CRTC_H

#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    uint32_t mPipe;
    uint32_t mId; 
} Drm_crtc;

void init_drmcrtc(Drm_crtc *drm_crtc_object, uint32_t pipe, drmModeCrtcPtr c);
uint32_t getPipe(Drm_crtc drm_crtc_object);
uint32_t getCrtcId(Drm_crtc drm_crtc_object);

#endif