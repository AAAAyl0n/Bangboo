/**
 * @file app_bangboo.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <mooncake.h>
#include "assets/icon_app_bangboo.hpp"
#include "../assets/icons/icons.h"

namespace MOONCAKE::APPS
{
    /**
     * @brief Bangboo
     *
     */
    class AppBangboo : public APP_BASE
    {
    public:
        enum ExpressionType_t {
            EXPR_EYES,     // 正常眼睛
            EXPR_BLINK,    // 眨眼
            EXPR_SMILE,    // 微笑
            EXPR_ANGER,    // 愤怒
            EXPR_SAD,      // 悲伤
            EXPR_WINCE     // 眯眼
        };
        
        struct ExpressionSequence_t {
            ExpressionType_t expression;
            unsigned long duration_ms;
        };
        
    private:
        
        struct Data_t
        {
            unsigned long count = 0;
            unsigned long current_time = 0;
            unsigned long sequence_start_time = 0;
            int current_sequence_index = 0;
            int current_loop = 0;
            ExpressionType_t current_expression = EXPR_EYES;
            unsigned long app_start_time = 0;  // app启动时间
            bool gpio_low_set = false;         // GPIO是否已设为低电平
        };
        Data_t _data;

    public:
        void onCreate() override;
        void onResume() override;
        void onRunning() override;
        void onDestroy() override;
        
        // 表情序列定义
        void initExpressionSequence();
        void drawExpression(ExpressionType_t expression);
        void drawBlinkWithColor(uint32_t color);
        void updateExpressionSequence();
    };

    class AppBangboo_Packer : public APP_PACKER_BASE
    {
        std::string getAppName() override { return "Bangboo"; }
        void* getAppIcon() override { return (void*)image_data_icon_app_bangboo; }
        void* newApp() override { return new AppBangboo; }
        void deleteApp(void* app) override { delete (AppBangboo*)app; }
    };
} // namespace MOONCAKE::APPS
