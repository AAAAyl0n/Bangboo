/**
 * @file app_timeview_stopwatch.cpp
 */
#include "app_timeview.h"
#include "../assets/theme/theme.h"
#include "../../hal/hal.h"
#include <cstdio>

using namespace MOONCAKE::APPS;

static void _format_ms_to_hhmmss(uint32_t ms, char* out, size_t outlen)
{
    uint32_t total = ms / 1000UL;
    uint32_t hh = total / 3600UL;
    uint32_t mm = (total % 3600UL) / 60UL;
    uint32_t ss = total % 60UL;
    snprintf(out, outlen, "%02u:%02u:%02u", (unsigned)hh, (unsigned)mm, (unsigned)ss);
}

void AppTimeview::_renderStopwatch()
{
    uint32_t elapsed = _data.swAccumulatedMs + (_data.swRunning ? (HAL::Millis() - _data.swStartMs) : 0);
    uint32_t total_seconds = elapsed / 1000UL;
    uint32_t centiseconds = (elapsed % 1000UL) / 10UL;  // 分秒（百分之一秒）
    
    uint32_t hh = total_seconds / 3600UL;
    uint32_t mm = (total_seconds % 3600UL) / 60UL;
    uint32_t ss = total_seconds % 60UL;
    
    char first_line[16];
    char second_line[16];
    char centisec_str[8];
    
    // 第一排：STW + 小时数
    snprintf(first_line, sizeof(first_line), "STW %02u", (unsigned)hh);
    // 第二排：分钟:秒
    snprintf(second_line, sizeof(second_line), "%02u:%02u", (unsigned)mm, (unsigned)ss);
    // 分秒显示
    snprintf(centisec_str, sizeof(centisec_str), ".%02u", (unsigned)centiseconds);

    HAL::GetCanvas()->setTextColor(THEME_COLOR_LawnGreen);
    HAL::GetCanvas()->setTextSize(2);
    HAL::GetCanvas()->drawCenterString(first_line, 120, 42);
    
    // 第二排：分钟:秒（大字体）
    HAL::GetCanvas()->setTextSize(4);
    HAL::GetCanvas()->drawCenterString(second_line, 120, 68);
    
    // 分秒计时（小字体，SeaGreen颜色，显示在秒数右侧）
    //HAL::GetCanvas()->setTextColor(THEME_COLOR_SeaGreen);
    HAL::GetCanvas()->setTextSize(1);
    // 计算秒数右侧位置：120是中心，second_line大约占用的宽度的一半 + 分秒文本的偏移
    HAL::GetCanvas()->drawString(centisec_str, 120 + 55, 68 + 21, &fonts::Font0);
}

void AppTimeview::_updateStopwatch()
{
    // 逻辑已在渲染阶段按需计算
}


