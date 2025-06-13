/**
 * @file app_bangboo.cpp
 * @author bowen
 * @brief
 * @version 0.1
 * @date 2025-06-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_bangboo.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"

using namespace MOONCAKE::APPS;

static void drawEyes()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillCircle(60, 91, 32, THEME_COLOR_LawnGreen);
    HAL::GetCanvas()->fillCircle(60, 91, 21, THEME_COLOR_BLACK);

    HAL::GetCanvas()->fillCircle(180, 91, 32, THEME_COLOR_LawnGreen);
    HAL::GetCanvas()->fillCircle(180, 91, 21, THEME_COLOR_BLACK);
    HAL::CanvasUpdate();
}

static void drawBlink()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillRect(29, 86, 64, 11, THEME_COLOR_LawnGreen);
    HAL::GetCanvas()->fillRect(149, 86, 64, 11, THEME_COLOR_LawnGreen);
    
    HAL::CanvasUpdate();
}

void AppBangboo::onCreate() { spdlog::info("{} onCreate", getAppName()); }

// Like setup()...
void AppBangboo::onResume() { spdlog::info("{} onResume", getAppName()); }

// Like loop()...
void AppBangboo::onRunning()
{
    //HAL::GetCanvas()->drawPng(page_about_png, page_about_png_size);
    //HAL::CanvasUpdate();
    HAL::Delay(5000); 
    drawBlink();
    HAL::Delay(1000); 
    // 调用封装好的函数
    for (int i = 0; i < 100; i++){
        drawEyes();
        HAL::Delay(3000); 
        drawBlink();
        HAL::Delay(100);
        drawEyes();
        HAL::Delay(100); 
        drawBlink();
        HAL::Delay(100);
        drawEyes();
        HAL::Delay(4000); 
        drawBlink();
        HAL::Delay(200);
    }
    

    //spdlog::info("咩啊");
    //HAL::Delay(1000);

    //_data.count++;
    //if (_data.count > 5)
    destroyApp();
}

void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
