#include "drm_encoder.h"
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include "../show_logo_log.h"
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

void init_drmencoder(Drm_encoder *drm_encoder_object, drmModeEncoderPtr e)
{
    if (MTK_LOG_ENABLE == 1)
        SLOGD("[libshowlogo: %s %d] drm_encoder_object = %p \n", __FUNCTION__, __LINE__, &drm_encoder_object);
    (*drm_encoder_object).mId = e->encoder_id;
    (*drm_encoder_object).mPossibleCrtcs = e->possible_crtcs;
}
uint32_t getEncoderId(Drm_encoder drm_encoder_object)
{
    return drm_encoder_object.mId;
}
uint32_t getPossibleCrtcs(Drm_encoder drm_encoder_object)
{
    return drm_encoder_object.mPossibleCrtcs;
}