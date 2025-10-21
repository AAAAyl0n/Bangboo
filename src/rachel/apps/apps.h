/**
 * @file apps.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "launcher/launcher.h"
#include "app_template/app_template.h"
#include "app_settings/app_settings.h"
//#include "app_screencast/app_screencast.h"
#include "app_music/app_music.h"
#include "app_genshin/app_genshin.h"
//#include "app_raylib_games/app_raylib_games.h"
//#include "app_ble_gamepad/app_ble_gamepad.h"
//#include "app_nofrendo/app_nofrendo.h"
#include "app_bangboo/app_bangboo.h"
#include "app_timeview/app_timeview.h"
#include "app_imutest/app_imutest.h"
#include "app_asciiart/app_asciiart.h"
#include "app_poweroff/app_poweroff.h"
/* Header files locator(Don't remove) */

void rachel_app_install_callback(MOONCAKE::Mooncake* mooncake)
{
    mooncake->installApp(new MOONCAKE::APPS::AppSettings_Packer);
    //mooncake->installApp(new MOONCAKE::APPS::AppMusic_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppGenshin_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppBangboo_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppTimeview_Packer);
    //mooncake->installApp(new MOONCAKE::APPS::AppImutest_Packer);
    //mooncake->installApp(new MOONCAKE::APPS::AppAsciiart_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppPoweroff_Packer);
    /* Install app locator(Don't remove) */
}
