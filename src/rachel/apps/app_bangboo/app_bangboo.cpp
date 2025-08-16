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
 #include <cmath>
 #include <cstdlib>

using namespace MOONCAKE::APPS;

void AppBangboo::CheckStatusBar() {
    // 检测START按键
    if (HAL::GetButton(GAMEPAD::BTN_START)) {
        _data.statusBarDisplayDuration = 3000;//时间显示3s
        _show_status_bar();
    }

    if (_data.statusBarVisible && !_data.statusBarAnimating) {
        if (HAL::Millis() - _data.statusBarShowTime > _data.statusBarDisplayDuration) {
            _hide_status_bar();
        }
    }
}

//------------------------------------------APP核心区------------------------------------------


void AppBangboo::onCreate() {
    spdlog::info("{} onCreate", getAppName());
    
    // 初始化状态栏动画
    _data.statusBarAnim.setAnim(LVGL::ease_out, 0, 24, 600);
    _data.statusBarVisible = false;
    _data.statusBarAnimating = false;
    
    // 初始化状态机
    _data.currentState = STATE_SLEEPING;
    _data.stateStartTime = HAL::Millis();
    _data.stateTimer = 0;
}

void AppBangboo::onResume() {
    spdlog::info("{} onResume", getAppName());
    HAL::GetCanvas()->setFont(&fonts::Font0);
}

void AppBangboo::onRunning()
{
    // 初始化随机种子
    srand(HAL::Millis());

    while (true)
    {
        // 检查按键SELECT退出
        if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
            destroyApp();
            return;
        }
        // 更新IMU检测状态
        imu_detection(); 

        // 更新状态机 - 自动管理表情变化
        updateStateMachine();

        // 更新时钟
        _update_clock();

        // 检查状态栏
        CheckStatusBar();

        // 渲染画面
        render();
    }
}


void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
 