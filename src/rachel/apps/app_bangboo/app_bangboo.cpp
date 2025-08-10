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

void AppBangboo::onCreate() {
    spdlog::info("{} onCreate", getAppName());
    initExpressionSequence();
}

void AppBangboo::onResume() {
    spdlog::info("{} onResume", getAppName());
}

void AppBangboo::onRunning()
{
    _data.current_time = HAL::Millis();

    /*
    // 初始化GPIO1和GPIO2为输出模式并设为高电平
    if (!_data.gpio_low_set) {
        pinMode(1, OUTPUT);
        pinMode(2, OUTPUT);
        digitalWrite(1, HIGH);
        digitalWrite(2, HIGH);
    }

    // 检查是否已经过了6秒 (6000毫秒)
    if (!_data.gpio_low_set && (_data.current_time - _data.app_start_time) >= 6000) {
        digitalWrite(1, LOW);
        digitalWrite(2, LOW);
        _data.gpio_low_set = true;  // 标记已设为低电平
        spdlog::info("GPIO1 and GPIO2 set to LOW after 6 seconds");
    }
    */

    // 检查按键SELECT退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        // digitalWrite(1, LOW);
        // digitalWrite(2, LOW);
        destroyApp();
        return;
    }

    // 更新表情序列
    updateExpressionSequence();
}

void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
 