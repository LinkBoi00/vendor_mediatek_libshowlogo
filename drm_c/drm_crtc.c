#include "drm_crtc.h"
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include "../show_logo_common.h"
#ifdef SLOGD
#undef SLOGD
//#define SLOGD(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGD(...)                              \
    do                                          \
    {                                           \
        KLOG_ERROR("libshowlogo", __VA_ARGS__); \
    } while (0)
#endif

#ifdef SLOGE
#undef SLOGE
//#define SLOGE(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGE(...)                              \
    do                                          \
    {                                           \
        KLOG_ERROR("libshowlogo", __VA_ARGS__); \
    } while (0)
#endif

void init_drmcrtc(Drm_crtc *drm_crtc_object, uint32_t pipe, drmModeCrtcPtr c)
{
    if (MTK_LOG_ENABLE == 1)
        SLOGD("[libshowlogo: %s %d] drm_crtc_object = %p , pipe = %d , c= %p \n", __FUNCTION__, __LINE__, &drm_crtc_object, pipe, c);
    (*drm_crtc_object).mPipe = pipe;
    (*drm_crtc_object).mId = c->crtc_id;
}

uint32_t getPipe(Drm_crtc drm_crtc_object)
{
    return drm_crtc_object.mPipe;
}

uint32_t getCrtcId(Drm_crtc drm_crtc_object)
{
    return drm_crtc_object.mId;
}