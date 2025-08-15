#include "app_genshin.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"
#include "../utils/system/ui/ui.h"

using namespace MOONCAKE::APPS;

// GIF 相关已抽离到 GifPlayer

void AppGenshin::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
}

void AppGenshin::onResume() { 
    spdlog::info("{} onResume", getAppName()); 

    // 居中并打开 GIF
    int gif_x = (HAL::GetDisplay()->width() - 220) / 2;
    int gif_y = (HAL::GetDisplay()->height() - 139) / 2;

    auto canvas = HAL::GetCanvas();
    canvas->fillScreen(TFT_BLACK);
    HAL::CanvasUpdate();

    if (!_gifPlayer.open("/gif/cat.gif", gif_x, gif_y)) {
        spdlog::error("GIF open failed: /gif/cat.gif");
    }
}

using namespace SYSTEM::UI;

void AppGenshin::onRunning()
{
    // 检查按键退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        // 退出前停止播放
        destroyApp();
    }

    // 驱动 GIF 帧推进
    if (_gifPlayer.isOpened()) {
        _gifPlayer.update();
    } else {
        delay(30);
    }
}

void AppGenshin::onDestroy() { 
    spdlog::info("{} onDestroy", getAppName()); 
    _gifPlayer.close();
}
