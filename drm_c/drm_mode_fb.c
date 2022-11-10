#include "drm_mode_fb.h"
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>

#ifdef SLOGD
#undef SLOGD
//#define SLOGD(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGD(...) do { KLOG_ERROR("libshowlogo", __VA_ARGS__); } while (0)
#endif

#ifdef SLOGE
#undef SLOGE
//#define SLOGE(x,...) do { KLOG_ERROR("libshowlogo", x); } while (0)
#define SLOGE(...) do { KLOG_ERROR("libshowlogo", __VA_ARGS__); } while (0)
#endif

void init_drmfb(Drm_Mode_Fb drmfb_object, drmModeFBPtr fb){
    SLOGD("[libshowlogo: %s %d] drmfbobject = %p \n",__FUNCTION__,__LINE__, &drmfb_object);
    drmfb_object.drmFb.bpp = fb->bpp;
    drmfb_object.drmFb.fb_id = fb->fb_id;
}

uint32_t getBpp(Drm_Mode_Fb drmfb_object) { return drmfb_object.drmFb.bpp; };