#include "app_genshin.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"
#include "../utils/system/ui/ui.h"

using namespace MOONCAKE::APPS;

void AppGenshin::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
    
    // 检查SD卡
    if (!HAL::CheckSdCard()) {
        spdlog::error("SD卡未准备就绪!");
        HAL::PopFatalError("没SD卡啊朋友");
        return;
    }
    
    // 音频系统已在HAL层初始化，无需在应用层重复初始化
    spdlog::info("使用HAL层音频系统");
}

void AppGenshin::onResume() { 
    spdlog::info("{} onResume", getAppName()); 
}

using namespace SYSTEM::UI;

void AppGenshin::onRunning()
{
    static uint32_t counter = 0;
    static uint32_t last_display_time = 0;
    
    uint32_t current_time = millis();
    
    HAL::GetCanvas()->setFont(&fonts::Font0);

    // 启动音频播放（只执行一次）
    if (!_audio_started) {
        const char* audio_path = "/bangboo_audio/bangboo.wav";
        
        spdlog::info("开始播放音频文件: {}", audio_path);
        bool success = HAL::PlayWavFile(audio_path);
        if (success) {
            _audio_started = true;
            spdlog::info("音频播放命令发送成功");
        } else {
            spdlog::error("发送音频播放命令失败!");
            _audio_started = true;  // 防止重复尝试
        }
    }

    // 每100ms更新一次显示（10fps）
    if (current_time - last_display_time >= 100) {
        last_display_time = current_time;
        counter += 50;  // 调整递增量保持视觉效果
        
        // 清屏并显示信息
        HAL::GetCanvas()->fillScreen(TFT_BLACK);
        HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(2);
        
        // 显示标题
        HAL::GetCanvas()->drawCenterString("HAL Audio System", 120, 20);
        
        // 显示播放状态
        String status_text = "Status: ";
        if (HAL::IsAudioPlaying()) {
            status_text += "Playing";
            HAL::GetCanvas()->setTextColor(TFT_GREEN, TFT_BLACK);
        } else if (_audio_started) {
            status_text += "Finished";
            HAL::GetCanvas()->setTextColor(TFT_YELLOW, TFT_BLACK);
        } else {
            status_text += "Ready";
            HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        }
        HAL::GetCanvas()->drawCenterString(status_text, 120, 50);
        
        // 显示音量信息
        HAL::GetCanvas()->setTextColor(TFT_CYAN, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(1);
        String volume_text = "Volume: " + String(HAL::GetAudioVolume()) + "%";
        HAL::GetCanvas()->drawCenterString(volume_text, 120, 75);
        
        // 显示快速递增的计数器
        HAL::GetCanvas()->setTextColor(TFT_CYAN, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(3);
        String counter_text = "Count: " + String(counter);
        HAL::GetCanvas()->drawCenterString(counter_text, 120, 90);
        
        // 显示操作提示
        HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->drawCenterString("SELECT: Exit", 120, 120);
        if (HAL::IsAudioPlaying()) {
            HAL::GetCanvas()->drawCenterString("START: Stop Audio", 120, 135);
        } else if (_audio_started) {
            HAL::GetCanvas()->drawCenterString("RIGHT: Replay Audio", 120, 135);
        }
        HAL::GetCanvas()->drawCenterString("UP/DOWN: Volume", 120, 150);
        
        HAL::CanvasUpdate();
    }

    // 处理按键
    if (HAL::GetButton(GAMEPAD::BTN_START) && HAL::IsAudioPlaying()) {
        // 停止播放
        HAL::StopAudioPlayback();
        spdlog::info("用户停止音频播放");
    }
    
    if (HAL::GetButton(GAMEPAD::BTN_RIGHT) && !HAL::IsAudioPlaying() && _audio_started) {
        // 重新播放
        const char* audio_path = "/bangboo_audio/hello.wav";
        bool success = HAL::PlayWavFile(audio_path);
        if (success) {
            spdlog::info("重新开始播放音频");
        }
    }
    
    
    // 检查按键退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        // 退出前停止播放
        if (HAL::IsAudioPlaying()) {
            HAL::StopAudioPlayback();
        }
        destroyApp();
    }
    
    delay(50);  // 适当延时
}

void AppGenshin::onDestroy() { 
    spdlog::info("{} onDestroy", getAppName()); 
    
    // 停止音频播放
    if (HAL::IsAudioPlaying()) {
        spdlog::info("停止正在播放的音频");
        HAL::StopAudioPlayback();
        
        // 等待音频停止
        int timeout = 50;  // 500ms超时
        while (HAL::IsAudioPlaying() && timeout > 0) {
            delay(10);
            timeout--;
        }
    }
    
    spdlog::info("应用清理完成，音频系统由HAL层管理");
}
