#include <fcntl.h>
#include <sys/mman.h>
// #include <linux/mediatek_drm.h>
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <libdrm_macros.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "drm_resource.h"

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

//-----------------   macros for custom drm function -------------

#define memclear(s) memset(&s, 0, sizeof(s))

#define U642VOID(x) ((void *)(unsigned long)(x))
#define VOID2U64(x) ((uint64_t)(unsigned long)(x))

//  -------------------- Macros end here ----------------

void *imagePointer = {0};
int image_pointer_index = 0;
void *drawPtr = {0};
//--------------------

int init_resource(Drm_resource *drm_resource_object)
{
  int res = 0;
  
  (*drm_resource_object).mDrmFd = -1; 
  (*drm_resource_object).mCrtc = NULL;
  (*drm_resource_object).mEncoder =NULL;
  (*drm_resource_object).mConnector = NULL;
  (*drm_resource_object).mDisplayWidth = 0;
  (*drm_resource_object).mDisplayHeight = 0;
  (*drm_resource_object).mPinPon = 0;
  for (size_t i = 0; i < MAX_BO_SIZE; i++)
  {
    memset(&((*drm_resource_object).mBo[i]), 0, sizeof(*((*drm_resource_object).mBo)));
    (*drm_resource_object).mBo[i].fbId = 0;
    (*drm_resource_object).mBo[i].fd = -1;
  }

  (*drm_resource_object).mDrmFd = open(DRM_NODE_PATH, O_RDWR);
  SLOGD("[drm_resource %s %d] drmFd = %d\n", __FUNCTION__, __LINE__, (*drm_resource_object).mDrmFd);

  if ((*drm_resource_object).mDrmFd < 0)
  {
    SLOGD("failed to open drm device node: %s , %s", DRM_NODE_PATH, strerror(errno));
    return -1;
  }
  else
  {
    SLOGD("[drm_resource %s %d] drm_resource_object = %p\n", __FUNCTION__, __LINE__, &drm_resource_object);
        
    res = initDrmResource(drm_resource_object);
    if (res != 0)
    {
      SLOGD("failed to init drm resource");
      return res;
    }

    res = findPrimaryDisplay(drm_resource_object);
    if (res != 0)
    {
      SLOGD("[drm_resource %s %d] failed to find primay display", __FUNCTION__, __LINE__);
      return res;
    }
    else
    {
      SLOGD("[drm_resource %s %d] find primary display success", __FUNCTION__, __LINE__);
      SLOGD("Values => drm_resource_object.mConnector = %p , %p, mDisplayWidth = %d , mDisplayHeight = %d", &((*drm_resource_object).mConnector), &drm_resource_object, drm_resource_object->mDisplayWidth, drm_resource_object->mDisplayHeight);
    }

    res = createFramebuffer(drm_resource_object);
    if (res != 0)
    {
      SLOGD("[drm_resource %s %d] failed to create framebuffer", __FUNCTION__, __LINE__);
      //destroyFramebuffer();
      return res;
    }

    draw(drm_resource_object);
    SLOGD("[drm_resource %s %d] draw and attach frambuffer successfully ", __FUNCTION__, __LINE__);

    res = getDrmFb(drm_resource_object);
    if (res != 0)
    {
      SLOGD("[drm_resource %s %d] failed to get drmfb", __FUNCTION__, __LINE__);
      return res;
    }
    SLOGD("[drm_resource %s %d] getDrmFb success  drm->fb->bpp = %d , drm->fb->width = %d", __FUNCTION__, __LINE__, (drm_resource_object->fb).bpp, (drm_resource_object->fb).width);
  }
  return res;
}

uint32_t getDrmBPP(uint32_t format)
{
  switch (format)
  {
  case DRM_FORMAT_XRGB8888:
  case DRM_FORMAT_XBGR8888:
  case DRM_FORMAT_RGBX8888:
  case DRM_FORMAT_BGRX8888:
  case DRM_FORMAT_ARGB8888:
  case DRM_FORMAT_ABGR8888:
  case DRM_FORMAT_RGBA8888:
  case DRM_FORMAT_BGRA8888:
  case DRM_FORMAT_XRGB2101010:
  case DRM_FORMAT_XBGR2101010:
  case DRM_FORMAT_RGBX1010102:
  case DRM_FORMAT_BGRX1010102:
  case DRM_FORMAT_ARGB2101010:
  case DRM_FORMAT_ABGR2101010:
  case DRM_FORMAT_RGBA1010102:
  case DRM_FORMAT_BGRA1010102:
    return 32;

  case DRM_FORMAT_RGB888:
  case DRM_FORMAT_BGR888:
    return 24;

  case DRM_FORMAT_RGB565:
    return 16;
  }
  SLOGE("Unknown format(%08x) to get bPP, use default 8", format);
  return 8;
}

int initDrmResource(Drm_resource *drm_resource_object)
{
  if (MTK_LOG_ENABLE == 1)
  {
    SLOGD("[drm_resource: %s %d] initDrmResource: drm_resource_object = %p %p \n", __FUNCTION__, __LINE__, &drm_resource_object, &(*drm_resource_object));
  }
  // initialise DrmFd
  //drm_resource_object.mDrmFd = open(DRM_NODE_PATH, O_RDWR);
  drmModeResPtr res = drmModeGetResources(drm_resource_object->mDrmFd);
  if (MTK_LOG_ENABLE == 1)
  {
    SLOGD("[drm_resource: %s %d] initDrmResource: drmresptr = %p \n", __FUNCTION__, __LINE__, &res);
  }

  if (!res)
  {
    if (MTK_LOG_ENABLE == 1)
      SLOGE("failed to get drm resource");
    return -1;
  }

  (*drm_resource_object).mCrtcList = (Drm_crtc *)malloc(sizeof(Drm_crtc) * res->count_crtcs);
  (*drm_resource_object).mEncoderList = (Drm_encoder *)malloc(sizeof(Drm_encoder) * res->count_encoders);
  (*drm_resource_object).mConnectorList = (Drm_connector *)malloc(sizeof(Drm_connector) * res->count_connectors);
  
  SLOGD("[drm_resource: %s %d] initDrmResource: Creating crtc: \n", __FUNCTION__, __LINE__);
  int ret = initCrtc(drm_resource_object, res);
  if (ret != 0)
  {
    SLOGE("failed to init crtc");
    return -1;
  }
  SLOGD("[drm_resource: %s %d] initDrmResource: Created crtc successfully. %d\n", __FUNCTION__, __LINE__, ret);
  ret = initEncoder(drm_resource_object, res);
  if (ret != 0)
  {
    SLOGE("failed to init encoder");
    return -1;
  }
  SLOGD("[drm_resource: %s %d] initDrmResource: created encoder successfully %d\n", __FUNCTION__, __LINE__, ret);
  ret = initConnector(drm_resource_object, res);
  if (ret != 0)
  {
    SLOGE("failed to init connector");
    return -1;
  }
  SLOGD("[drm_resource: %s %d] initDrmResource: created connector successfully %d \n", __FUNCTION__, __LINE__, ret);
  return ret;
}

int initCrtc(Drm_resource *drm_resource_object, drmModeResPtr r)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] initCrtc: drm_resource_object = %p , r->count_crcts = %d r->count_fbs = %d \n", __FUNCTION__, __LINE__, drm_resource_object, r->count_crtcs, r->count_fbs);
  int res = 0;
  if (!r->count_crtcs)
  {
    SLOGE("[drm_resource: %s %d] initCrtc No Crtc Found \n", __FUNCTION__, __LINE__);
    return -1;
  }
  countCrtc = r->count_crtcs;

  for (int i = 0; i < r->count_crtcs; i++)
  {
    Drm_crtc drm_crtc_object;
    drmModeCrtcPtr c = drmModeGetCrtc(drm_resource_object->mDrmFd, r->crtcs[i]);
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] drmModeCrtcPtr c = %p , c->bufferid = %d \n", __FUNCTION__, __LINE__, c, c->buffer_id);

    if (c == NULL)
    {
      SLOGE("failed to open drm crtc");
      return -1;
    }
    drm_resource_object->drmCrtc = c;
    init_drmcrtc(&drm_crtc_object, i, c);
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] drm_crtc_object->mId = %d\n", __FUNCTION__, __LINE__, drm_crtc_object.mId);
    errno = 0;
    drm_resource_object->mCrtcList[i] = drm_crtc_object;
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d]drm_resource_object.mCrtcList[%d] = %p drm_crtc_object = %p \n", __FUNCTION__, __LINE__, i, &(drm_resource_object->mCrtcList[i]), &drm_crtc_object);
    drmModeFreeCrtc(c);
  }
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] initCrtc: res = %d \n", __FUNCTION__, __LINE__, res);
  return res;
}

int initEncoder(Drm_resource *drm_resource_object, drmModeResPtr r)
{
  if (MTK_LOG_ENABLE)
    SLOGD("[drm_resource: %s %d] initEncoder: drm_resource_object = %p \n", __FUNCTION__, __LINE__, drm_resource_object);
  int res = 0;
  if (!r->count_encoders)
  {
    SLOGE("[drm_resource: %s %d] No Encoder found, return \n", __FUNCTION__, __LINE__);
    return -1;
  }
  countEncoder = r->count_encoders;

  for (int i = 0; i < r->count_encoders; i++)
  {
    drmModeEncoderPtr e = drmModeGetEncoder(drm_resource_object->mDrmFd, r->encoders[i]);
    Drm_encoder drm_encoder_object;
    init_drmencoder(&drm_encoder_object, e);
    drm_resource_object->mEncoderList[i] = drm_encoder_object;
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] drm_resource_object.mEncoderList[%d] = %p drm_encoder_object = %p \n", __FUNCTION__, __LINE__, i, &(drm_resource_object->mEncoderList[i]), &drm_encoder_object);
    //SLOGD("[drm_resource: %s %d] Loop successfully %dth time \n", __FUNCTION__, __LINE__, i);
    drmModeFreeEncoder(e);
  }
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] initEncoder: res = %d \n", __FUNCTION__, __LINE__, res);
  return res;
}

int initConnector(Drm_resource *drm_resource_object, drmModeResPtr r)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] initConnector: drm_resource_object = %p \n", __FUNCTION__, __LINE__, drm_resource_object);
  if (!r->count_connectors)
  {
    SLOGE("[drm_resource: %s %d] No Connector found, return \n", __FUNCTION__, __LINE__);
    return -1;
  }
  int res = 0;
  countConnector = r->count_connectors;
  for (int i = 0; i < r->count_connectors; i++)
  {
    drmModeConnectorPtr c = drmModeGetConnector(drm_resource_object->mDrmFd, r->connectors[i]);
    if ((!c) || (!c->count_modes) || (!c->count_encoders))
    {
      SLOGE("[drm_resource: %s %d] connector resource free %dth time, error in c ptr \n", __FUNCTION__, __LINE__, i);
      drmModeFreeConnector(c);
      continue;
    }
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] initConnector: count_connectors = %d, count_modes = %d \n", __FUNCTION__, __LINE__, r->count_connectors, c->count_modes);
    Drm_connector drm_connector_object;
    init_drm_connector(&drm_connector_object, c);
    drm_resource_object->mConnectorList[i] = drm_connector_object;
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] drm_resource_object.mConnectorList[%d] = %p drm_connector_object = %p \n", __FUNCTION__, __LINE__, i, &(drm_resource_object->mConnectorList[i]), &drm_connector_object);
    //SLOGD("[drm_resource: %s %d] Loop successfully %dth time \n", __FUNCTION__, __LINE__, i);
    drmModeFreeConnector(c);
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] connector resource free %dth time \n", __FUNCTION__, __LINE__, i);
  }
  //SLOGD("[drm_resource: %s %d] initConnector: res = %d \n", __FUNCTION__, __LINE__, res);
  return res;
}

int findPrimaryDisplay(Drm_resource *drm_resource_object)
{
  if (MTK_LOG_ENABLE == 1)
  {
    SLOGD("[drm_resource: %s %d] FindPrimary Display: drm_resource_object = %p \n", __FUNCTION__, __LINE__, &drm_resource_object);
    SLOGD("[drm_resource: %s %d] FindPrimary Display: countCrtc = %d, countEncoder = %d, countConnector = %d \n", __FUNCTION__, __LINE__, countCrtc, countEncoder, countConnector);
  }
  for (size_t i = 0; i < countCrtc; i++)
  { //drm_resource_object.mCrtcList.size(); i++) {
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] FindPrimary Display: inside loop 1 =>  [%d] drm_resource_object.mCrtcList[%d] = %p \n", __FUNCTION__, __LINE__, i, i, &(drm_resource_object->mCrtcList[i]));
    Drm_crtc *drm_crtc_object = &(drm_resource_object->mCrtcList[i]);
    uint32_t pipe = getPipe(*drm_crtc_object);
    if (MTK_LOG_ENABLE == 1)
      SLOGD("Pipe: %u", pipe);
    for (size_t j = 0; j < countEncoder; j++)
    {
      if (MTK_LOG_ENABLE == 1)
        SLOGD("[drm_resource: %s %d] FindPrimary Display: inside loop 2 => [%d] drm_resource_object.mEncoderList[%d] %p \n", __FUNCTION__, __LINE__, j, j, &(drm_resource_object->mEncoderList[j]));
      //drm_resource_object.mEncoderList.size(); j++) {
      Drm_encoder *drm_encoder_object = &(drm_resource_object->mEncoderList[j]);
      uint32_t mask = 1 << pipe;
      if (getPossibleCrtcs(*drm_encoder_object) & mask)
      {
        for (size_t k = 0; k < countConnector; k++)
        { //drm_resource_object.mConnectorList.size(); k++) {
          if (MTK_LOG_ENABLE == 1)
            SLOGD("[drm_resource: %s %d] FindPrimary Display: inside loop 3 => [%d] drm_resource_object.mConnectorList[%d] = %p \n", __FUNCTION__, __LINE__, k, k, &(drm_resource_object->mConnectorList[k]));
          Drm_connector *drm_connector_object = &(drm_resource_object->mConnectorList[k]);
          if (isValidEncoder(*drm_connector_object, getEncoderId(*drm_encoder_object)) &&
              getConnectorType(*drm_connector_object) == DRM_MODE_CONNECTOR_DSI)
          {
            drm_resource_object->mCrtc = &(drm_resource_object->mCrtcList[i]);
            drm_resource_object->mEncoder = &(drm_resource_object->mEncoderList[j]);
            drm_resource_object->mConnector = &(drm_resource_object->mConnectorList[k]);
            if (MTK_LOG_ENABLE == 1)
            {
              SLOGD("find the primary display success: crtc[pipe:%u id:%u address:%p] encoder[id:%u address:%p] connector[id:%u address:%p getAdd:%p]",
                    pipe, getCrtcId(*drm_crtc_object), &(drm_resource_object->mCrtc), getEncoderId(*drm_encoder_object), &(drm_resource_object->mEncoder), getConnectorId(*drm_connector_object), &(drm_resource_object->mConnector), drm_resource_object->mConnector);
            }
            int getResolution = getModeResolution(*(drm_resource_object->mConnector), &(drm_resource_object->mDisplayWidth), &(drm_resource_object->mDisplayHeight), 0);
            if (MTK_LOG_ENABLE == 1)
            {
              SLOGD(" \n drm_resource_object.mConnector = %p %p %p\n \
                       drm_resource_object.mDisplayWidth= &=>[%p , %d], %p \n \
                       drm_resource_object.mDisplayHeight = &=>[%p %d], %p ",
                    &drm_resource_object->mConnector, &(drm_resource_object->mConnector->mId), &drm_resource_object,
                    &(drm_resource_object->mDisplayWidth), drm_resource_object->mDisplayWidth, &drm_resource_object->mDisplayWidth,
                    &(drm_resource_object->mDisplayHeight), drm_resource_object->mDisplayHeight, &drm_resource_object->mDisplayHeight);
            }
            return 0;
          }
        }
      }
    }
  }
  return -1;
}

int createFramebuffer(Drm_resource *drm_resource_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] createFrambuffer: drm_resource_object = %p\n", __FUNCTION__, __LINE__, &drm_resource_object);
  int res = 0;
  for (size_t i = 0; i < MAX_BO_SIZE; i++)
  {
    drm_resource_object->mBo[i].width = drm_resource_object->mDisplayWidth;
    drm_resource_object->mBo[i].height = drm_resource_object->mDisplayHeight;
    drm_resource_object->mBo[i].format = DRM_FORMAT_RGBA8888;
    res = allocateBuffer(drm_resource_object, &(drm_resource_object->mBo[i]));
    if (res != 0)
    {
      SLOGE("failed to allocateBuffer_%zu: %d", i, res);
      return res;
    }
  }
  return res;
}

int allocateBuffer(Drm_resource *drm_resource_object, struct DrmBo *bo)
{
  if (MTK_LOG_ENABLE == 1)
  {
    SLOGD("[drm_resource: %s %d] allocateBuffer: drm_resource_object = %p \n", __FUNCTION__, __LINE__, &drm_resource_object);
    SLOGD("allocateBuffer: %ux%u  format:0x%08x", bo->width, bo->height, bo->format);
  }
  int res = 0;
  res = createDumb(drm_resource_object, bo);
  if (res)
  {
    SLOGE("failed to create dumb buffer: %d (wxh=%ux%u)",
          res, bo->width, bo->height);
    return res;
  }

  res = addFb(drm_resource_object, bo);
  if (res)
  {
    SLOGE("faile to add fb: %d", res);
    struct drm_mode_destroy_dumb destroyArg;
    memset(&destroyArg, 0, sizeof(destroyArg));
    destroyArg.handle = bo->gemHandles[0];
    if (drmIoctl(drm_resource_object->mDrmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroyArg))
      SLOGE("[drm_resource: %s %d]: destroy dumb %d", __FUNCTION__, __LINE__, res);
    return res;
  }
  else
  {
    SLOGD("success to add fb: fb_id=%u", bo->fbId);
  }

#if 1
  int fd = -1;
  res = drmPrimeHandleToFD(drm_resource_object->mDrmFd, bo->gemHandles[0], DRM_RDWR, &fd);
  if (res)
  {
    SLOGE("[drm_resource: %s %d]: failed to exchange the fd: %d", __FUNCTION__, __LINE__, res);
    freeBuffer(drm_resource_object, bo);
    return res;
  }
  else
  {
    SLOGD("[drm_resource: %s %d]: success to exchange fd: %d", __FUNCTION__, __LINE__, fd);
  }
  bo->fd = fd;
#endif

  return res;
}

int createDumb(Drm_resource *drm_resource_object, struct DrmBo *bo)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] createDumb: drm_resource_object = %p  bpp = %d\n", __FUNCTION__, __LINE__, &drm_resource_object, getDrmBPP(bo->format));
  int res = 0;
  struct drm_mode_create_dumb createArg;
  memset(&createArg, 0, sizeof(createArg));
  createArg.bpp = getDrmBPP(bo->format);
  createArg.width = bo->width;
  createArg.height = bo->height;

  res = drmIoctl(drm_resource_object->mDrmFd, DRM_IOCTL_MODE_CREATE_DUMB, &createArg);
  if (res == 0)
  {
    bo->pitches[0] = createArg.pitch;
    bo->gemHandles[0] = createArg.handle;
    bo->offsets[0] = 0;
    bo->size = createArg.size;
    if (MTK_LOG_ENABLE == 1)
      SLOGD("[drm_resource: %s %d] bo->size = %d \n", __FUNCTION__, __LINE__, bo->size);
  }
  else
    SLOGE("[drm_resource: %s %d] create dumb IOCTL failed", __FUNCTION__, __LINE__);

  return res;
}

int addFb(Drm_resource *drm_resource_object, struct DrmBo *bo)
{
  if (MTK_LOG_ENABLE == 1)
  {
    SLOGD("[drm_resource: %s %d] addFb: drm_resource_object = %p \n", __FUNCTION__, __LINE__, &drm_resource_object);
    SLOGD("add fb w:%u h:%u f:%x hnd:%u p:%u o:%u",
          bo->width, bo->height, bo->format, bo->gemHandles[0],
          bo->pitches[0], bo->offsets[0]);
  }
  int res = 0;
  res = drmModeAddFB2WithModifiers(drm_resource_object->mDrmFd, bo->width, bo->height, bo->format, bo->gemHandles,
                                   bo->pitches, bo->offsets, bo->modifier, &(bo->fbId), DRM_MODE_FB_MODIFIERS);
  if (res)
  {
    SLOGE("failed to add fb ret=%d, w:%u h:%u f:%x hnd:%u p:%u o:%u",
          res, bo->width, bo->height, bo->format, bo->gemHandles[0],
          bo->pitches[0], bo->offsets[0]);
  }
  return res;
}

int freeBuffer(Drm_resource *drm_resource_object, struct DrmBo *bo)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] initConnector: drm_resource_object = %p \n", __FUNCTION__, __LINE__, &drm_resource_object);
  int res = 0;
  res = removeFb(drm_resource_object, bo->fbId);
  if (res)
  {
    SLOGE("[drm_resource: %s %d] failed to remove fb: %d", __FUNCTION__, __LINE__, res);
  }

  res = destroyDumb(drm_resource_object, bo);
  if (res)
  {
    SLOGE("[drm_resource: %s %d] failed to destroy dumb: %d", __FUNCTION__, __LINE__, res);
  }
  return res;
}

int removeFb(Drm_resource *drm_resource_object, uint32_t fbId)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] removeFb: drm_resource_object = %p FbID = %u \n", __FUNCTION__, __LINE__, &drm_resource_object, fbId);
  int res = 0;
  res = drmModeRmFB(drm_resource_object->mDrmFd, fbId);
  if (res)
  {
    SLOGE("[drm_resource: %s %d] failed to remove fb ret=%d, id:%u", __FUNCTION__, __LINE__, res, fbId);
  }
  return res;
}

int destroyDumb(Drm_resource *drm_resource_object, struct DrmBo *bo)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource: %s %d] destroyDumb: drm_resource_object = %p \n", __FUNCTION__, __LINE__, &drm_resource_object);
  int res = 0;
  struct drm_mode_destroy_dumb destroyArg;
  memset(&destroyArg, 0, sizeof(destroyArg));
  destroyArg.handle = bo->gemHandles[0];
  res = drmIoctl(drm_resource_object->mDrmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroyArg);
  return res;
}

void draw(Drm_resource *drm_resource_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource %s %d] drm_resource_object = %p ", __FUNCTION__, __LINE__, drm_resource_object);
  uint32_t connectorId = getConnectorId(*(drm_resource_object->mConnector));
  drmModeModeInfo modeInfo = getModeInfo(drm_resource_object->mConnector->mModes[0]);
  //SLOGD("[drm_resource %s %d] drmmodeinfo got success", __FUNCTION__, __LINE__);
  uint32_t crtcId = getCrtcId(*(drm_resource_object->mCrtc));

  for (int i = 0; i < MAX_BO_SIZE; ++i)
  {
    struct DrmBo *bo = &(drm_resource_object->mBo[drm_resource_object->mPinPon]);
    SLOGD("[drm_resource %s %d] data for crtc => mdrmfd = %d , crtcId = %d, fbId = %d,x= %d,Y = %d, connectorId = %p , 1, modeInfo = %p %p",
          __FUNCTION__, __LINE__, drm_resource_object->mDrmFd, crtcId, bo->fbId, 0, 0, &connectorId, &modeInfo, modeInfo);
    SLOGD("[drm_resource %s %d] drmmodeinfo before setCrtc = %d,\n hdisplay = %d, hsync_start = %d, hsync_end = %d, htotal = %d, hskew = %d, \n vdisplay = %d, vsync_start = %d, vsync_end = %d, vtotal = %d, vscan = %d, \n vrefresh = %d, flags = %d, type = %d",
            __FUNCTION__, __LINE__, modeInfo.clock, modeInfo.hdisplay, modeInfo.hsync_start, modeInfo.hsync_end, modeInfo.htotal, modeInfo.hskew, modeInfo.vdisplay, modeInfo.vsync_start, modeInfo.vsync_end, modeInfo.vtotal, modeInfo.vtotal, modeInfo.vscan, modeInfo.vrefresh, modeInfo.flags, modeInfo.type);
    int res = drmModeSetCrtc(drm_resource_object->mDrmFd, crtcId, bo->fbId, 0, 0, &connectorId, 1, &modeInfo);
    if (res != 0)
    {
      SLOGE("[drm_resource %s %d] failed to set crtc: %d", __FUNCTION__, __LINE__, res);
    }
    else
    {
      SLOGD("[drm_resource %s %d] draw a frame [%zu]fb_id=%u crtc=%u conn=%u", __FUNCTION__, __LINE__, drm_resource_object->mPinPon, drm_resource_object->mBo[drm_resource_object->mPinPon].fbId, crtcId, connectorId);
    }

    drm_resource_object->mPinPon = drm_resource_object->mPinPon + 1;
    drm_resource_object->mPinPon = drm_resource_object->mPinPon % MAX_BO_SIZE;
  }
}

int getDrmFb(Drm_resource *drm_resource_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource %s %d] getDrmFb called ", __FUNCTION__, __LINE__);
  int ret = 0;
  int setPinpon = -1;
  drmModeResPtr res = drmModeGetResources(drm_resource_object->mDrmFd);
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[drm_resource %s %d] res->count_fbs=%d ", __FUNCTION__, __LINE__, res->count_fbs);

  for (int i = 0; i < res->count_crtcs; i++)
  {
    drmModeCrtcPtr c = drmModeGetCrtc(drm_resource_object->mDrmFd, res->crtcs[i]);
    drmModeFBPtr fb = drmModeGetFB(drm_resource_object->mDrmFd, c->buffer_id);
    if (fb == NULL)
    {
      SLOGE("[drm_resource %s %d] getDrmFb failed ", __FUNCTION__, __LINE__);
      drmModeFreeFB(fb);
      drmModeFreeCrtc(c);
    }
    else
    {
      SLOGD("[drm_resource %s %d] getDrmFb success fb->id = %d fb->bpp = %d, fb->width = %d ", __FUNCTION__, __LINE__, fb->fb_id, fb->bpp, fb->width);
      (*drm_resource_object).fb = *fb;
      drmModeFreeFB(fb);
      drmModeFreeCrtc(c);
    }
  }
  drmModeFreeResources(res);
  for (int j = 0; j < MAX_BO_SIZE; ++j)
  {
    if (drm_resource_object->mBo[j].fbId == drm_resource_object->fb.fb_id)
    {
      setPinpon = j;
      continue;
    }
    // else
    //   freeBuffer(drm_resource_object, &(drm_resource_object->mBo[j]));
  }
  if (setPinpon != -1)
    drm_resource_object->mPinPon = (setPinpon + 1) % MAX_BO_SIZE;
  return ret;
}

void freeResource(Drm_resource *drm_resource_object) {
  for(int i  = 0; i < MAX_BO_SIZE; ++i){
    if(drm_resource_object->mBo[i].fd >= 0)
      close(drm_resource_object->mBo[i].fd);
    if(drm_resource_object->mBo[i].fbId != 0)
      removeFb(drm_resource_object, drm_resource_object->mBo[i].fbId);
    if(drm_resource_object->mBo[i].gemHandles[0])
      destroyDumb(drm_resource_object, &(drm_resource_object->mBo[i]));
  }
  free(drm_resource_object->mCrtc);
  free(drm_resource_object->mCrtcList);
  free(drm_resource_object->mEncoder);
  free(drm_resource_object->mEncoderList);
  free(drm_resource_object->mConnector);
  free(drm_resource_object->mConnectorList);
}