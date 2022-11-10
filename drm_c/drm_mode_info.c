#include "drm_mode_info.h"
#include "../show_logo_log.h"
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>

#ifdef SLOGD
#undef SLOGD
//#define SLOGD(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGD(...)                          \
  do                                        \
  {                                         \
    KLOG_ERROR("libshowlogo", __VA_ARGS__); \
  } while (0)
#endif

#ifdef SLOGE
#undef SLOGE
//#define SLOGE(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGE(...)                          \
  do                                        \
  {                                         \
    KLOG_ERROR("libshowlogo", __VA_ARGS__); \
  } while (0)
#endif

void init_DrmModeInfo(DrmModeInfo *drmInfo_object, drmModeModeInfoPtr mode)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drmInfo_object = %p \n", __FUNCTION__, __LINE__, &drmInfo_object);
  (*drmInfo_object).mInfo.clock = mode->clock;
  (*drmInfo_object).mInfo.hdisplay = mode->hdisplay;
  (*drmInfo_object).mInfo.hsync_start = mode->hsync_start;
  (*drmInfo_object).mInfo.hsync_end = mode->hsync_end;
  (*drmInfo_object).mInfo.htotal = mode->htotal;
  (*drmInfo_object).mInfo.hskew = mode->hskew;
  (*drmInfo_object).mInfo.vdisplay = mode->vdisplay;
  (*drmInfo_object).mInfo.vsync_start = mode->vsync_start;
  (*drmInfo_object).mInfo.vsync_end = mode->vsync_end;
  (*drmInfo_object).mInfo.vtotal = mode->vtotal;
  (*drmInfo_object).mInfo.vscan = mode->vscan;
  (*drmInfo_object).mInfo.vrefresh = mode->vrefresh;
  (*drmInfo_object).mInfo.flags = mode->flags;
  (*drmInfo_object).mInfo.type = mode->type;
  memcpy((*drmInfo_object).mInfo.name, mode->name, sizeof((*drmInfo_object).mInfo.name));
}

uint32_t getWidth(DrmModeInfo drmInfo_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drmInfo_object = %p \n", __FUNCTION__, __LINE__, &drmInfo_object);
  return drmInfo_object.mInfo.hdisplay;
}

uint32_t getHeight(DrmModeInfo drmInfo_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drmInfo_object = %p \n", __FUNCTION__, __LINE__, &drmInfo_object);
  return drmInfo_object.mInfo.vdisplay;
}

drmModeModeInfo getModeInfo(DrmModeInfo drmInfo_object) { return drmInfo_object.mInfo; }