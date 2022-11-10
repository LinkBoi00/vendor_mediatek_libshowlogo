#ifndef DRM_ENCODER_H
#define DRM_ENCODER_H

#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    uint32_t mId;
    uint32_t mPossibleCrtcs;
} Drm_encoder;

void init_drmencoder(Drm_encoder *drm_encoder_object, drmModeEncoderPtr e);
uint32_t getEncoderId(Drm_encoder drm_encoder_object);
uint32_t getPossibleCrtcs(Drm_encoder drm_encoder_object);

#endif