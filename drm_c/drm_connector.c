
#include <cutils/klog.h>
#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <assert.h>
#include<stdlib.h>
#include "drm_mode_info.h"
#include "drm_connector.h"
#include "../show_logo_log.h"
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

void init_drm_connector(Drm_connector *drm_connector_object, drmModeConnectorPtr c)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drm_connector_object = %p \n", __FUNCTION__, __LINE__, &drm_connector_object);
  (*drm_connector_object).mId = c->connector_id;
  (*drm_connector_object).mConnectorType = c->connector_type;
  (*drm_connector_object).mCountEncoders = c->count_encoders;
  if ((*drm_connector_object).mCountEncoders)
  {
    (*drm_connector_object).mPossibleEncoderId = (uint32_t *)malloc(sizeof(uint32_t *) * (c->count_encoders));
    for (int i = 0; i < (*drm_connector_object).mCountEncoders; i++)
    {
      (*drm_connector_object).mPossibleEncoderId[i] = c->encoders[i];
    }
  }
  if (c->count_modes)
  {
    (*drm_connector_object).count_nModes = c->count_modes;
    (*drm_connector_object).mModes = (DrmModeInfo *)malloc(sizeof(DrmModeInfo) * c->count_modes);
    for (int i = 0; i < c->count_modes; i++)
    {
      DrmModeInfo drmInfo_object;
      init_DrmModeInfo(&drmInfo_object, &(c->modes[i]));
      (*drm_connector_object).mModes[i] = drmInfo_object;
    }
  }
  SLOGD("[libshowlogo: %s %d] init_drm_connector return success connector_type = %d\n", __FUNCTION__, __LINE__, (*drm_connector_object).mConnectorType);
}

uint32_t getConnectorId(Drm_connector drm_connector_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] getConnectoreId = %p \n", __FUNCTION__, __LINE__, &drm_connector_object);
  return drm_connector_object.mId;
}

uint32_t getConnectorType(Drm_connector drm_connector_object)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drm_connector_object = %p \n", __FUNCTION__, __LINE__, &drm_connector_object);
  return drm_connector_object.mConnectorType;
}

bool isValidEncoder(Drm_connector drm_connector_object, uint32_t encoderId)
{
  if (MTK_LOG_ENABLE == 1)
    SLOGD("[libshowlogo: %s %d] drm_connector_object = %p encoderId = %d\n", __FUNCTION__, __LINE__, &drm_connector_object, encoderId);
  for (size_t i = 0; i < drm_connector_object.mCountEncoders; i++)
  {
    if (encoderId == drm_connector_object.mPossibleEncoderId[i])
    {
      return true;
    }
  }
  return false;
}

bool getModeResolution(Drm_connector drm_connector_object, int *width, int *height, size_t index)
{
  assert(0);
  if(MTK_LOG_ENABLE == 1)
  SLOGD("[libshowlogo: %s %d] drm_connector_object = %p \n", __FUNCTION__, __LINE__, &drm_connector_object);
  if (index < drm_connector_object.count_nModes)
  {
    *width = getWidth(drm_connector_object.mModes[index]);
    *height = getHeight(drm_connector_object.mModes[index]);
    SLOGD("[libshowlogo: %s %d] success, resolution: width = %d height = %d\n", __FUNCTION__, __LINE__, *width, *height);
    return true;
  }
  SLOGE("[libshowlogo: %s %d] Failed to get resolution \n", __FUNCTION__, __LINE__);
  return false;
}
