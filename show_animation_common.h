/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _SHOW_ANIMATION_COMMON_H
#define _SHOW_ANIMATION_COMMON_H

#include "show_logo_common.h"
#include "decompress_common.h"
#include "cust_display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int logonum;
    unsigned int logolen;
    uint64_t inaddr;
} LOGO_PARA_T;

/********** show_animationm_ver:  charging animation version  ************/
/*                                                                       */
/* version 0: show 4 recatangle growing animation without battery number */
/* version 1: show wave animation with  battery number                   */
/* version 2: show wireless charging animation                           */
/*                                                                       */
/***                                                                   ***/
#define MAX_SUPPORT_ANIM_VER  2

#define VERION_OLD_ANIMATION 0
#define VERION_NEW_ANIMATION 1
#define VERION_WIRELESS_CHARGING_ANIMATION  2

// version_0 animation
#define ANIM_V0_REGIONS  4
#define ANIM_V0_SPACE_AFTER_REGION  4

void init_charging_animation_ui_dimension();
int getValue(char* key, char* defValue);

void fill_animation_logo(int index, void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_prog_bar(RECT_REGION_T rect_bar,
                       unsigned int fgColor,
                       unsigned int start_div, unsigned int occupied_div,
                       void *fill_addr, LCM_SCREEN_T phical_screen);

void fill_animation_dynamic(int index, RECT_REGION_T rect,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);
void fill_animation_number(int index, unsigned int number_position,
                       void *fill_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_line(int index, unsigned int capacity_grids,
                       void *fill_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_battery_old(int capacity,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_battery_new( int capacity,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_battery_wireless_charging(int capacity,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen);

void fill_animation_battery_fast_charging(int capacity,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen,
                       int draw_anim_mode);

/* public interface function     */
void fill_animation_battery_by_ver(int capacity,
                       void *fill_addr,
                       void *dec_logo_addr,
                       void *logo_addr,
                       LCM_SCREEN_T phical_screen,
                       int version);

#ifdef __cplusplus
}
#endif

#endif
