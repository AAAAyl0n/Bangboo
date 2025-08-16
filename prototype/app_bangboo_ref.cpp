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



float calculateDeltaAccel(float accelX, float accelY, float accelZ, float lastAccelX, float lastAccelY, float lastAccelZ) {
    float deltaX = accelX - lastAccelX;
    float deltaY = accelY - lastAccelY;
    float deltaZ = accelZ - lastAccelZ;
    return sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}



void AppBangboo::onCreate() {
    spdlog::info("{} onCreate", getAppName());
    initExpressionSequence();
}

void AppBangboo::onResume() {
    spdlog::info("{} onResume", getAppName());
    HAL::GetCanvas()->setFont(&fonts::Font0);
}

void AppBangboo::onRunning()
{
    drawALL(EXPR_BLINK);
    // 初始化随机种子
    srand(HAL::Millis());

    while (true)
    {
        // 检查按键SELECT退出
        if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
            destroyApp();
            return;
        }
        
        // 更新IMU数据
        HAL::UpdateImuData();
        
        // 获取IMU数据
        auto& imuData = HAL::GetImuData();
        
        // 获取当前时间
        unsigned long currentTime = HAL::Millis();
        
        // 计算加速度变化量 (摇晃强度)
        if (_data.lastTime > 0) {
            float deltaAccel = calculateDeltaAccel(imuData.accelX, imuData.accelY, imuData.accelZ, _data.lastAccelX, _data.lastAccelY, _data.lastAccelZ);
            // 更新摇晃强度（带衰减）
            _data.shakeIntensity = _data.shakeIntensity * _data.SHAKE_DECAY + deltaAccel;
        }
        
        // 摇晃计数重置逻辑：如果超过1分钟没有摇晃，重置计数器
        if (currentTime - _data.lastShakeResetTime >= _data.SHAKE_RESET_INTERVAL) {
            if (_data.shakeCount > 0) {
                _data.shakeCount = 0;
                _data.angerTriggerCount = 0;  // 同时重置触发次数
                spdlog::info("摇晃计数器已重置");
            }
            _data.lastShakeResetTime = currentTime;
        }
        
        // 摇晃检测状态机
        switch (_data.shakeState) {
            case SHAKE_IDLE:
                // 眨眼逻辑
                if (!_data.isPlayingSequence && currentTime - _data.lastBlinkTime >= _data.nextBlinkDelay) {
                    // 随机选择单次或双次眨眼
                    bool isDoubleBlink = (rand() % 2) == 1;
                    if (isDoubleBlink) {
                        startExpressionSequence(AppBangboo::double_blink_sequence, AppBangboo::DOUBLE_BLINK_LENGTH);
                    } else {
                        startExpressionSequence(AppBangboo::single_blink_sequence, AppBangboo::SINGLE_BLINK_LENGTH);
                    }
                    
                    // 设置下一次眨眼的随机间隔（4-6秒）
                    _data.lastBlinkTime = currentTime;
                    _data.nextBlinkDelay = _data.BLINK_MIN_INTERVAL + 
                                          (rand() % (_data.BLINK_MAX_INTERVAL - _data.BLINK_MIN_INTERVAL));
                }
                
                // 更新表情序列播放
                updateExpressionSequencePlayback();
                
                // 检测是否开始摇晃
                if (_data.shakeIntensity > _data.SHAKE_THRESHOLD) {
                    _data.shakeState = SHAKE_DETECTING;
                    _data.shakeStartTime = currentTime;
                }
                break;
                
            case SHAKE_DETECTING:
                // 检测摇晃是否持续足够长时间
                if (currentTime - _data.shakeStartTime >= _data.SHAKE_DETECT_TIME) {
                    // 摇晃持续足够时间，触发显示
                    _data.shakeState = SHAKE_TRIGGERED;
                    _data.triggerStartTime = currentTime;
                    _data.lastShakeResetTime = currentTime;  // 更新重置时间
                    
                    // 增加摇晃计数
                    _data.shakeCount++;
                    spdlog::info("摇晃计数: {}", _data.shakeCount);
                    
                    // 如果是第一次摇晃，随机选择第3次还是第4次触发愤怒
                    if (_data.shakeCount == 1) {
                        _data.angerTriggerCount = (rand() % 2) ? 3 : 4;  // 随机选择3或4
                        spdlog::info("本轮愤怒将在第{}次摇晃时触发", _data.angerTriggerCount);
                    }
                    
                    // 检查是否达到预设的触发次数
                    if (_data.shakeCount == _data.angerTriggerCount) {
                        spdlog::info("触发愤怒序列！摇晃次数: {}", _data.shakeCount);
                        HAL::PlayWavFile("/bangboo_audio/1s_5.wav");
                        startExpressionSequence(AppBangboo::anger_sequence, AppBangboo::ANGER_SEQUENCE_LENGTH);
                        _data.shakeCount = 0;         // 重置计数器
                        _data.angerTriggerCount = 0;  // 重置触发次数
                    } else {
                        // 正常摇晃反应
                        // 随机播放1s_1、1s_3、1s_4、1s_5中的一个音频
                        int idx = rand() % 3;
                        const char* audio_files[] = {
                            "/bangboo_audio/1s_1.wav",
                            "/bangboo_audio/1s_4.wav",
                            "/bangboo_audio/1s_5.wav"
                        };
                        HAL::PlayWavFile(audio_files[idx]);
                        drawALL(EXPR_WINCE);  // 显示眯眼状态
                    }
                } else if (_data.shakeIntensity < _data.SHAKE_THRESHOLD * 0.3f) {
                    // 摇晃强度太低，回到空闲状态
                    _data.shakeState = SHAKE_IDLE;
                    _data.shakeIntensity = 0.0f;
                    drawALL(EXPR_EYES);  // 恢复正常眼睛
                }
                break;
                
            case SHAKE_TRIGGERED:
                // 如果正在播放表情序列（如anger_sequence），让序列自然结束
                if (_data.isPlayingSequence) {
                    updateExpressionSequencePlayback();
                    // 如果序列播放完成，回到空闲状态
                    if (!_data.isPlayingSequence) {
                        _data.shakeState = SHAKE_IDLE;
                        _data.shakeIntensity = 0.0f;
                    }
                } else {
                    // 显示wince状态的正常计时
                    if (currentTime - _data.triggerStartTime >= _data.SHAKE_DISPLAY_TIME) {
                        _data.shakeState = SHAKE_IDLE;
                        _data.shakeIntensity = 0.0f;
                        drawALL(EXPR_EYES);  // 恢复正常眼睛
                    }
                }
                break;
        }
        
        // 更新上一次的加速度值
        _data.lastAccelX = imuData.accelX;
        _data.lastAccelY = imuData.accelY;
        _data.lastAccelZ = imuData.accelZ;
        _data.lastTime = currentTime;
        
        // 增加计数器
        _data.count++;
        
        // 延时一下，避免刷新太快
        HAL::Delay(10);
    }

}

void AppBangboo::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
 