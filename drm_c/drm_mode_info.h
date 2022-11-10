#ifndef DRM_MODE_INFO_H
#define DRM_MODE_INFO_H

#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <string.h>
#include <stdbool.h>

typedef struct 
{
    drmModeModeInfo mInfo;
}DrmModeInfo ;

void init_DrmModeInfo(DrmModeInfo *drmInfo_object, drmModeModeInfoPtr mode);
uint32_t getWidth(DrmModeInfo drmInfo_object);
uint32_t getHeight(DrmModeInfo drmInfo_object);
drmModeModeInfo getModeInfo(DrmModeInfo drmInfo_object);

#endif