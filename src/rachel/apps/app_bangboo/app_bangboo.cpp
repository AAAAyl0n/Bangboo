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

// 表情序列定义：一个循环周期的表情和时间
static const AppBangboo::ExpressionSequence_t expression_sequence[] = {
    {AppBangboo::EXPR_EYES, 800},   // 正常眼睛 0.1秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_EYES, 100},   // 正常眼睛 0.1秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_EYES, 3000},   // 正常眼睛 3秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_ANGER, 3000},  // 愤怒 3秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_EYES, 3000},   // 正常眼睛 3秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_SMILE, 3000},  // 微笑 3秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_EYES, 100},    // 正常眼睛 0.1秒
    {AppBangboo::EXPR_BLINK, 100},   // 眨眼 0.1秒
    {AppBangboo::EXPR_EYES, 3000},   // 正常眼睛 3秒
    //{AppBangboo::EXPR_BLINK, 130}    // 眨眼 0.13秒
};

static const int SEQUENCE_LENGTH = sizeof(expression_sequence) / sizeof(expression_sequence[0]);
static const int MAX_LOOPS = 100;

void AppBangboo::drawExpression(ExpressionType_t expression)
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    
    switch (expression) {
        case EXPR_EYES:
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
            break;
            
        case EXPR_BLINK:
            HAL::GetCanvas()->fillSmoothRoundRect(29, 86, 64, 11, 2, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothRoundRect(149, 86, 64, 11, 2, BANGBOO_EYE_COLOR);
            break;
            
        case EXPR_SMILE:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillArc(60, 150, 61, 71, 241, 299);
            HAL::GetCanvas()->fillArc(180, 150, 61, 71, 241, 299);
            break;
            
        case EXPR_ANGER:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(0, 30, 60, 91, 150, 91, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(240, 30, 180, 91, 90, 91, THEME_COLOR_BLACK);
            break;
            
        case EXPR_SAD:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(20, 91, 50, 0, 100, 70, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(220, 91, 190, 0, 140, 70, THEME_COLOR_BLACK);
            break;
            
        case EXPR_WINCE:
            HAL::GetCanvas()->fillSmoothTriangle(95, 91, 33, 55, 33, 127, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothTriangle(73, 91, 33, 67.72, 33, 114.28, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(145, 91, 207, 55, 207, 127, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothTriangle(167, 91, 207, 67.72, 207, 114.28, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillSmoothTriangle(33, 73, 33, 55, 53, 55, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(33, 109, 33, 127, 53, 127, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillSmoothTriangle(207, 73, 207, 55, 187, 55, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(207, 109, 207, 127, 187, 127, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillTriangle(90, 55, 90, 127, 120, 91, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(150, 55, 150, 127, 120, 91, THEME_COLOR_BLACK);
            break;
    }
    
    HAL::CanvasUpdate();
}

void AppBangboo::drawBlinkWithColor(uint32_t color)
{
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    HAL::GetCanvas()->fillSmoothRoundRect(29, 86, 64, 11, 2, color);
    HAL::GetCanvas()->fillSmoothRoundRect(149, 86, 64, 11, 2, color);
    HAL::CanvasUpdate();
}

void AppBangboo::onCreate() { 
    spdlog::info("{} onCreate", getAppName());
    initExpressionSequence();
}

void AppBangboo::onResume() { 
    spdlog::info("{} onResume", getAppName()); 
}

void AppBangboo::initExpressionSequence()
{
    _data.current_time = HAL::Millis();
    _data.sequence_start_time = _data.current_time;
    _data.current_sequence_index = -1; // -1 表示初始启动动画状态
    _data.current_loop = 0;
    _data.current_expression = EXPR_BLINK;
    
    // 显示初始黑色眨眼状态
    drawBlinkWithColor(THEME_COLOR_BLACK);
}

void AppBangboo::updateExpressionSequence()
{
    _data.current_time = HAL::Millis();
    unsigned long elapsed_time = _data.current_time - _data.sequence_start_time;
    
    if (_data.current_sequence_index == -1) {
        if (elapsed_time >= 500) {
            // 开始循环序列
            _data.current_sequence_index = 0;
            _data.sequence_start_time = _data.current_time;
            _data.current_expression = expression_sequence[0].expression;
            drawExpression(_data.current_expression);
        } else {
            // 启动动画：眨眼状态下颜色从黑色渐变到绿色
            float progress = (float)elapsed_time / 500.0f;
            progress = progress > 1.0f ? 1.0f : progress;
            
            // 从黑色(0,0,0)到BANGBOO_EYE_COLOR的RGB插值
            // THEME_COLOR_LawnGreen = 0x7CFC00 = RGB(124, 252, 0)
            uint8_t r = (uint8_t)(124 * progress);
            uint8_t g = (uint8_t)(252 * progress);
            uint8_t b = (uint8_t)(0 * progress);
            uint32_t current_color = (r << 16) | (g << 8) | b;
            
            drawBlinkWithColor(current_color);
        }
        return;
    }
    
    // 检查当前表情是否到时间了
    if (elapsed_time >= expression_sequence[_data.current_sequence_index].duration_ms) {
        // 切换到下一个表情
        _data.current_sequence_index++;
        
        // 检查是否完成了一个循环
        if (_data.current_sequence_index >= SEQUENCE_LENGTH) {
            _data.current_loop++;
            
            // 检查是否完成了所有循环
            if (_data.current_loop >= MAX_LOOPS) {
                spdlog::info("Bangboo animation completed {} loops", MAX_LOOPS);
                destroyApp();
                return;
            }
            
            // 重新开始循环
            _data.current_sequence_index = 0;
        }
        
        // 更新当前表情并绘制
        _data.current_expression = expression_sequence[_data.current_sequence_index].expression;
        drawExpression(_data.current_expression);
        _data.sequence_start_time = _data.current_time;
    }
}

void AppBangboo::onRunning()
{
    // 检查按键SELECT退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        destroyApp();
        return;
    }
    
    // 更新表情序列
    updateExpressionSequence();
}

void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
