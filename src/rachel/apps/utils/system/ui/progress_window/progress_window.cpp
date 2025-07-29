/**
 * @file progress_window.cpp
 * @author Bowen
 * @brief 
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "progress_window.h"
#include "../../../../../hal/hal.h"
#include "../common/data_structs.hpp"
#include "../../../../assets/theme/theme.h"
#include "../../../../assets/fonts/fonts.h"
#include <cmath>


void SYSTEM::UI::ProgressWindow(std::string title, uint8_t progress, bool lightMode, bool useCanvas)
{
    // 设置颜色方案：黑色背景
    uint32_t background_color = THEME_COLOR_BLACK;
    uint32_t text_color = THEME_COLOR_LIGHT;
    uint32_t ring_bg_color = THEME_COLOR_DARK;
    uint32_t ring_progress_color = THEME_COLOR_LIGHT; // 白色实心圆环

    // Clear screen - 黑色背景
    HAL::GetCanvas()->fillScreen(background_color);

    // 计算半圆环参数 - 在屏幕上半部分显示
    int center_x = HAL::GetCanvas()->width() / 2;
    int center_y = HAL::GetCanvas()->height() / 4 + 40; // 稍微下移一点给标题留空间
    int radius = HAL::GetCanvas()->width() / 3; // 圆环半径
    int ring_thickness = 8; // 圆环厚度
    
    // 限制进度值
    if (progress > 100)
        progress = 100;
    
    // 绘制半圆环背景 - 使用fillSmoothArc函数
    HAL::GetCanvas()->fillSmoothArc(center_x, center_y, radius - ring_thickness, radius, 180, 360, ring_bg_color);
    
    // 绘制进度半圆环 (白色实心) - 使用fillSmoothArc函数，从左往右
    if (progress > 0) {
        float progress_angle = 180.0 * progress / 100.0; // 计算进度对应的角度
        HAL::GetCanvas()->fillSmoothArc(center_x, center_y, radius - ring_thickness, radius, 180 , 180 + progress_angle, ring_progress_color);
    }

    // 在屏幕上半部分中间显示标题
    HAL::GetCanvas()->setTextColor(text_color, background_color);
    HAL::GetCanvas()->setFont(&fonts::Font0);
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->drawCenterString(
        title.c_str(), 
        center_x,
        center_y - 20  // 在半圆环上方显示标题
    );
    
    // 在屏幕上半部分中间显示进度数值
    char progress_text[8];
    sprintf(progress_text, "%d%%", progress);
    HAL::GetCanvas()->setTextSize(2);
    HAL::GetCanvas()->drawCenterString(
        progress_text, 
        center_x,
        center_y - 40  // 在标题上方显示百分比
    );
}

