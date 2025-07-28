/**
 * @file app_timeview.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "app_timeview.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"
#include <string>
#include <ctime>

using namespace MOONCAKE::APPS;

void AppTimeview::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
}

// Like setup()...
void AppTimeview::onResume() { 
    spdlog::info("{} onResume", getAppName()); 
    //HAL::LoadTextFont24();//为了防止阻塞
    HAL::GetCanvas()->setFont(&fonts::Font0);
}

// Like loop()...
void AppTimeview::onRunning()
{
    //static bool font_loaded = false;
    static unsigned long last_update_time = 0;
    const unsigned long update_interval = 500; // 0.5秒更新间隔
    
    //

    
    // 检查按键SELECT退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        destroyApp();
        return;
    }
    
    unsigned long current_time = HAL::Millis();
    
    // 检查是否需要更新显示
    if (current_time - last_update_time >= update_interval) {
        // Get RTC time
        auto time_now = HAL::GetLocalTime();
        char date_buffer[32];
        char time_buffer[16];
        strftime(date_buffer, sizeof(date_buffer), "%a %d", time_now);
        strftime(time_buffer, sizeof(time_buffer), "%H:%M", time_now);

        HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
        HAL::GetCanvas()->setTextColor(THEME_COLOR_LawnGreen);
        HAL::GetCanvas()->setTextSize(2);
        HAL::GetCanvas()->drawCenterString(date_buffer, 120, 42);
        HAL::GetCanvas()->setTextSize(4);
        HAL::GetCanvas()->drawCenterString(time_buffer, 120, 68);
        HAL::CanvasUpdate();
        
        last_update_time = current_time;
        _data.count++;
    }
}

void AppTimeview::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
