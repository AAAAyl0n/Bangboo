/**
 * @file app_bangboo_expressions.cpp
 * @brief Expression drawing and sequencing for Bangboo app
 */

#include "app_bangboo.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"

using namespace MOONCAKE::APPS;

// 邦布眼睛颜色
static const auto BANGBOO_EYE_COLOR = THEME_COLOR_LawnGreen;

// Y轴偏移量（正值向下，负值向上）
static const int Y_OFFSET = 0;


void AppBangboo::drawExpression(ExpressionType_t expression)
{
    //HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);

    switch (expression) {
        case EXPR_EYES:
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            break;

        case EXPR_BLINK:
            HAL::GetCanvas()->fillSmoothRoundRect(29, 86 + Y_OFFSET, 64, 11, 2, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothRoundRect(149, 86 + Y_OFFSET, 64, 11, 2, BANGBOO_EYE_COLOR);
            break;

        case EXPR_SMILE:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillArc(60, 150 + Y_OFFSET, 61, 71, 241, 299);
            HAL::GetCanvas()->fillArc(180, 150 + Y_OFFSET, 61, 71, 241, 299);
            break;

        case EXPR_ANGER:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(0, 30 + Y_OFFSET, 60, 91 + Y_OFFSET, 150, 91 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(240, 30 + Y_OFFSET, 180, 91 + Y_OFFSET, 90, 91 + Y_OFFSET, THEME_COLOR_BLACK);
            break;

        case EXPR_SAD:
            HAL::GetCanvas()->setColor(BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(60, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 32, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothCircle(180, 91 + Y_OFFSET, 21, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(20, 91 + Y_OFFSET, 50, 0 + Y_OFFSET, 100, 70 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(220, 91 + Y_OFFSET, 190, 0 + Y_OFFSET, 140, 70 + Y_OFFSET, THEME_COLOR_BLACK);
            break;

        case EXPR_WINCE:
            HAL::GetCanvas()->fillSmoothTriangle(95, 91 + Y_OFFSET, 33, 55 + Y_OFFSET, 33, 127 + Y_OFFSET, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothTriangle(73, 91 + Y_OFFSET, 33, 67.72 + Y_OFFSET, 33, 114.28 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(145, 91 + Y_OFFSET, 207, 55 + Y_OFFSET, 207, 127 + Y_OFFSET, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothTriangle(167, 91 + Y_OFFSET, 207, 67.72 + Y_OFFSET, 207, 114.28 + Y_OFFSET, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillSmoothTriangle(33, 73 + Y_OFFSET, 33, 55 + Y_OFFSET, 53, 55 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(33, 109 + Y_OFFSET, 33, 127 + Y_OFFSET, 53, 127 + Y_OFFSET, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillSmoothTriangle(207, 73 + Y_OFFSET, 207, 55 + Y_OFFSET, 187, 55 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillSmoothTriangle(207, 109 + Y_OFFSET, 207, 127 + Y_OFFSET, 187, 127 + Y_OFFSET, THEME_COLOR_BLACK);

            HAL::GetCanvas()->fillTriangle(90, 55 + Y_OFFSET, 90, 127 + Y_OFFSET, 120, 91 + Y_OFFSET, THEME_COLOR_BLACK);
            HAL::GetCanvas()->fillTriangle(150, 55 + Y_OFFSET, 150, 127 + Y_OFFSET, 120, 91 + Y_OFFSET, THEME_COLOR_BLACK);
            break;
        
        case EXPR_SLEEP:
            HAL::GetCanvas()->fillSmoothRoundRect(29, 86 + Y_OFFSET, 64, 11, 2, BANGBOO_EYE_COLOR);
            HAL::GetCanvas()->fillSmoothRoundRect(149, 86 + Y_OFFSET, 64, 11, 2, BANGBOO_EYE_COLOR);
            break;
    }

    //HAL::CanvasUpdate();
}

//------------------------------------------状态栏------------------------------------------

// 显示一条状态栏消息，durationMs=0 则使用默认显示时长
void AppBangboo::showStatusMessage(const char* msg, uint32_t durationMs) {
    if (msg == nullptr) {
        return;
    }
    _data.statusBarMessage = msg;
    // 设定消息过期时间
    uint32_t now = HAL::Millis();
    uint32_t showDuration = durationMs == 0 ? _data.statusBarDisplayDuration : durationMs;
    _data.statusBarDisplayDuration = showDuration;
    _data.statusBarMessageExpire = now + showDuration + 600;

    // 触发状态栏显示或刷新显示时间
    _show_status_bar();
}

void AppBangboo::_update_clock(bool updateNow)
{
    if ((HAL::Millis() - _data.clock_update_count) > _data.clock_update_interval || updateNow)
    {
        // 更新时钟
        strftime(_data.string_buffer, 10, "%H:%M", HAL::GetLocalTime());
        _data.clock = _data.string_buffer;
        _data.clock_update_count = HAL::Millis();
    }
}

void AppBangboo::_render_status_bar()
{
    if (!_data.statusBarVisible && !_data.statusBarAnimating) {
        return;
    }

    // 获取当前动画值
    float anim_value = _data.statusBarAnim.getValue(HAL::Millis());
    
    if (anim_value > 0) {
        // 计算圆角矩形尺寸
        int rect_width = 70;  // 适应大字体的宽度
        int rect_height = 24; // 适应大字体的高度
        int rect_x = (240 - rect_width) / 2; // 居中
        
        // 使用动画值控制下滑/上浮效果
        int rect_y;
        if (_data.statusBarAnimating && !_data.statusBarVisible) {
            // 进场动画 
            rect_y = anim_value - rect_height;
        } else {
            // 退场动画
            rect_y = 5 - rect_height + (anim_value * rect_height / 24);
        }

        // 绘制圆角矩形背景
        HAL::GetCanvas()->fillSmoothRoundRect(rect_x, rect_y, rect_width, rect_height, 8, THEME_COLOR_NIGHT);
        
        // 决定显示内容：优先显示消息，否则显示时间
        const char* toShow = nullptr;
        uint32_t now = HAL::Millis();
        if (_data.statusBarMessageExpire != 0 && now < _data.statusBarMessageExpire && !_data.statusBarMessage.empty()) {
            toShow = _data.statusBarMessage.c_str();
        } else {
            // 消息过期或不存在，清空消息占位
            _data.statusBarMessage.clear();
            _data.statusBarMessageExpire = 0;
            toShow = _data.clock.c_str();
        }

        HAL::GetCanvas()->setTextSize(2);
        HAL::GetCanvas()->setTextColor(TFT_WHITE, THEME_COLOR_NIGHT);
        HAL::GetCanvas()->drawCenterString(toShow, 120, rect_y + 4, &fonts::Font0);

        // 检查动画是否结束
        if (_data.statusBarAnimating) {
            if (_data.statusBarAnim.isFinished(HAL::Millis())) {
                _data.statusBarAnimating = false;
                if (!_data.statusBarVisible) {
                    _data.statusBarVisible = false;
                }
            }
        }
    }
}

void AppBangboo::_show_status_bar()
{
    // 如果状态栏已经可见，只重置显示时间
    if (_data.statusBarVisible && !_data.statusBarAnimating) {
        _data.statusBarShowTime = HAL::Millis();
        return;
    }

    // 如果正在隐藏动画中，重新显示
    _data.statusBarVisible = true;
    _data.statusBarAnimating = true;
    _data.statusBarShowTime = HAL::Millis();

    // 设置下滑动画
    _data.statusBarAnim.setAnim(
        LVGL::ease_out,  // 缓动函数
        0,               // 起始值
        24,              // 结束值
        600             // 动画时长(ms)
    );
    _data.statusBarAnim.resetTime(HAL::Millis());
}

void AppBangboo::_hide_status_bar()
{
    if (!_data.statusBarVisible || _data.statusBarAnimating) {
        return;
    }

    _data.statusBarAnimating = true;

    // 设置上浮动画
    _data.statusBarAnim.setAnim(
        LVGL::ease_out,  // 缓动函数
        24,              // 起始值
        -10,               // 结束值
        600             // 动画时长(ms)
    );
    _data.statusBarAnim.resetTime(HAL::Millis());
}

void AppBangboo::render(){
    HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
    drawExpression(_data.currentExpression);
    _render_status_bar();
    HAL::CanvasUpdate();
}
