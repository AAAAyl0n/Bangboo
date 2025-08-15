/**
 * @file app_bangboo.h
 * @author Bowen
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
    enum ShakeState_t {
        SHAKE_IDLE,        // 空闲状态，显示"- -"
        SHAKE_DETECTING,   // 检测摇晃中
        SHAKE_TRIGGERED    // 摇晃触发，显示"> <"
    };
    
        struct Data_t
        {
            unsigned long count = 0;
            
            // 摇晃检测相关
            ShakeState_t shakeState = SHAKE_IDLE;
            unsigned long lastTime = 0;
            unsigned long shakeStartTime = 0;
            unsigned long triggerStartTime = 0;
            
            // 加速度相关
            float lastAccelX = 0.0f;
            float lastAccelY = 0.0f;
            float lastAccelZ = 0.0f;
            float shakeIntensity = 0.0f;
            
            // 眨眼相关
            unsigned long lastBlinkTime = 0;
            unsigned long nextBlinkDelay = 6000;  // 初始眨眼间隔6秒
            
            // 摇晃计数相关
            int shakeCount = 0;                    // 累计摇晃次数
            unsigned long lastShakeResetTime = 0; // 上次重置摇晃计数的时间
            int angerTriggerCount = 0;             // 随机选择的触发次数（3或4）
            static constexpr unsigned long SHAKE_RESET_INTERVAL = 60000; // 60秒无摇晃则重置计数
            
            // 表情序列播放状态
            bool isPlayingSequence = false;
            const ExpressionSequence_t* currentSequence = nullptr;
            int currentSequenceLength = 0;
            int currentSequenceIndex = 0;
            unsigned long sequenceStepStartTime = 0;
            
            // 阈值和时间常数
            static constexpr float SHAKE_THRESHOLD = 2.7f;  // 摇晃阈值 (m/s²)
            static constexpr unsigned long SHAKE_DETECT_TIME = 500;  // 检测时间 0.5s
            static constexpr unsigned long SHAKE_DISPLAY_TIME = 1900; // 显示时间 1.9s
            static constexpr float SHAKE_DECAY = 0.84f;     // 摇晃强度衰减系数
            static constexpr unsigned long BLINK_DURATION = 100;  // 眨眼持续时间100ms
            static constexpr unsigned long BLINK_MIN_INTERVAL = 4000;  // 最小眨眼间隔4s
            static constexpr unsigned long BLINK_MAX_INTERVAL = 6000;  // 最大眨眼间隔6s

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
        void drawALL(ExpressionType_t expression);
        void drawBlinkWithColor(uint32_t color);
        void updateExpressionSequence();
        void startExpressionSequence(const ExpressionSequence_t* sequence, int sequence_length);
        void updateExpressionSequencePlayback();
        
        // 眨眼序列定义
        static const ExpressionSequence_t single_blink_sequence[];
        static const ExpressionSequence_t double_blink_sequence[];
        static const ExpressionSequence_t anger_sequence[];
        static const int SINGLE_BLINK_LENGTH;
        static const int DOUBLE_BLINK_LENGTH;
        static const int ANGER_SEQUENCE_LENGTH;
    };

    class AppBangboo_Packer : public APP_PACKER_BASE
    {
        std::string getAppName() override { return "Bangboo"; }
        void* getAppIcon() override { return (void*)image_data_icon_app_bangboo; }
        void* newApp() override { return new AppBangboo; }
        void deleteApp(void* app) override { delete (AppBangboo*)app; }
    };
} // namespace MOONCAKE::APPS
