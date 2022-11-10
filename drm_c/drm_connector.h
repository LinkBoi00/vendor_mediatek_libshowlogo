#ifndef DRM_CONNECTOR_H
#define DRM_CONNECTOR_H

#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <string.h>
#include <stdbool.h>


typedef struct
{
    uint32_t mId;
    uint32_t mConnectorType;
    int mCountEncoders;
    uint32_t *mPossibleEncoderId;
    DrmModeInfo *mModes;
    int count_nModes;
} Drm_connector;

void init_drm_connector(Drm_connector *drm_connector_object, drmModeConnectorPtr c);
uint32_t getConnectorId(Drm_connector drm_connector_object);
uint32_t getConnectorType(Drm_connector drm_connector_object);
bool isValidEncoder(Drm_connector drm_connector_object, uint32_t encoderId);
bool getModeResolution(Drm_connector drm_connector_object, int *width, int *height, size_t index);

#endif
