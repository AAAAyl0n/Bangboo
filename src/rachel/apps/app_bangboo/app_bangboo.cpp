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
#include "assets/page_about_png_test.hpp"

using namespace MOONCAKE::APPS;

// 邦布眼睛颜色
static const auto BANGBOO_EYE_COLOR = THEME_COLOR_LawnGreen;

static void drawEyes()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
    HAL::CanvasUpdate();
}

static void drawSmile()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR); 
    HAL::GetCanvas()->fillArc(60, 150, 61, 71, 241, 299);
    HAL::GetCanvas()->fillArc(180, 150, 61, 71, 241, 299);
    HAL::CanvasUpdate();
}

static void drawAnger()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
    
    //HAL::GetCanvas()->fillSmoothTriangle(20, 68, 100, 93, 70, 0, THEME_COLOR_BLACK);
    //HAL::GetCanvas()->fillSmoothTriangle(220, 68, 140, 93, 170, 0, THEME_COLOR_BLACK);

    HAL::GetCanvas()->fillSmoothTriangle(0, 30, 60, 91, 150, 91, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(240, 30, 180, 91, 90, 91, THEME_COLOR_BLACK);


    //HAL::GetCanvas()->fillArc(60, 91, 21, 32, 0, 225);
    //HAL::GetCanvas()->fillArc(180, 91, 21, 32, -45, 180);
    HAL::CanvasUpdate();
}

static void drawSad()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
    
    HAL::GetCanvas()->fillSmoothTriangle(20, 91, 50, 0, 100, 70, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(220, 91, 190, 0, 140, 70, THEME_COLOR_BLACK);

    HAL::CanvasUpdate();
}

static void drawWince(){
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);

    HAL::GetCanvas()->fillSmoothTriangle(95, 91, 33, 55, 33, 127, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothTriangle(73, 91, 33, 67.72, 33, 114.28, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(145, 91, 207, 55, 207, 127, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothTriangle(167, 91, 207, 67.72, 207, 114.28, THEME_COLOR_BLACK);

    HAL::GetCanvas()->fillSmoothTriangle(33, 70, 33, 55, 53, 55, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(33, 112, 33, 127, 53, 127, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(207, 70, 207, 55, 187, 55, THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothTriangle(207, 112, 207, 127, 187, 127, THEME_COLOR_BLACK);
    HAL::CanvasUpdate();
}

static void drawBlink()
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothRoundRect(29, 86, 64, 11, 2, BANGBOO_EYE_COLOR);
    HAL::GetCanvas()->fillSmoothRoundRect(149, 86, 64, 11, 2, BANGBOO_EYE_COLOR);
    
    HAL::CanvasUpdate();
}

void AppBangboo::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
}

// Like setup()...
void AppBangboo::onResume() { spdlog::info("{} onResume", getAppName()); }

// Like loop()...
void AppBangboo::onRunning()
{
    //HAL::RenderFpsPanel();
    //HAL::GetCanvas()->drawPng(page_about_png_test, page_about_png_test_size);
    //HAL::CanvasUpdate();
    //HAL::Delay(1000); 
    drawSad();
    //drawBlink();
    HAL::Delay(3000); 

    //drawWince();    
    //HAL::Delay(3000);
    
    // Blink animation loop
    for (int i = 0; i < 100; i++){
        drawEyes();
        HAL::Delay(3000); 
        drawBlink();
        HAL::Delay(100);
        drawAnger();
        HAL::Delay(3000); 
        drawBlink();
        HAL::Delay(100);
        drawEyes();
        HAL::Delay(3000); 
        drawBlink();
        HAL::Delay(100);
        drawSmile();
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
        HAL::Delay(130);
    }
    
    destroyApp();
}

void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
