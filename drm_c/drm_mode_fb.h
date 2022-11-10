#ifndef DRM_MODE_FB_H
#define DRM_MODE_FB_H


#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    drmModeFB drmFb;
} Drm_Mode_Fb;

void init_drmfb(Drm_Mode_Fb drmfb_object, drmModeFBPtr fb);

uint32_t getBpp(Drm_Mode_Fb drmfb_object);

#endif