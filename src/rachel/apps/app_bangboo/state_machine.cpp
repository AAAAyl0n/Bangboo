/**
 * @file state_machine.cpp
 * @brief 简单而可扩展的状态机实现
 * @author Assistant
 * @date 2025-01-24
 */

#include "app_bangboo.h"
#include "../../hal/hal.h"
#include "spdlog/spdlog.h"
#include <cstdlib>
#include "esp_system.h"

using namespace MOONCAKE::APPS;

// 状态机更新函数 - 在主循环中调用
void AppBangboo::updateStateMachine() {
    uint32_t currentTime = HAL::Millis();
    uint32_t stateElapsed = currentTime - _data.stateStartTime;
    
    // 更新当前表情
    _data.currentExpression = getExpressionForState(_data.currentState);
    
    // 全局高优先级：摇晃触发则无论当前在哪个普通状态，直接进入WINCE状态
    if (_data.shakeState == SHAKE_TRIGGERED
        && _data.currentState != STATE_PREWINCE
        && _data.currentState != STATE_WINCE
        && _data.currentState != STATE_POSTWINCE) {
        changeState(STATE_PREWINCE);
        return;
    }
    // 状态转换逻辑
    switch (_data.currentState) {

        case STATE_PREIDLE:
            if (stateElapsed > 100) {
                // 切空闲前弹个短提示
                changeState(STATE_IDLE);
                showStatusMessage("^^", 2000);
            }
            break;

        case STATE_IDLE:
            // 空闲状态 - 随机选择下一个状态
            if (stateElapsed > (4000 + rand() % 3000)) {
                int nextState = esp_random() % 7;
                switch (nextState) {
                    case 0: changeState(STATE_BLINKING0); break;
                    case 1: changeState(STATE_BLINKING0); break;
                    case 2: changeState(STATE_BLINKING0); break;
                    case 3: changeState(STATE_BLINKING0); break;
                    case 4: changeState(STATE_BLINKING0); break;
                    case 5: changeState(STATE_PREHAPPY); break;
                    case 6: changeState(STATE_PREANGER); break;
                }
            }
            break;
        

        case STATE_BLINKING0:
            // 眨眼状态 - 短暂眨眼后回到空闲
            if (stateElapsed > 100) {
                // INSERT_YOUR_CODE
                // 0.6的概率去IDLE，0.4的概率去BLINKTEMP
                if ((esp_random() % 10) < 4) {
                    changeState(STATE_BLINKTEMP);
                } else {
                    changeState(STATE_IDLE);
                }
            }
            break;
        
        case STATE_BLINKTEMP:
            if (stateElapsed > 100) {
                changeState(STATE_BLINKING1);
            }
            break;
            
        case STATE_BLINKING1:
            // 眨眼状态 - 短暂眨眼后回到空闲
            if (stateElapsed > 100) {
                changeState(STATE_IDLE);
            }
            break;
            

        case STATE_PREHAPPY:
            if (stateElapsed > 100) {
                changeState(STATE_HAPPY);
            }
            break;
        case STATE_HAPPY:
            // 开心状态 - 保持1-2秒后回到空闲
            if (stateElapsed > (2000 + rand() % 1000)) {
                changeState(STATE_PREIDLE);
            }
            break;
        

        case STATE_PREANGER:
            if (stateElapsed > 100) {
                changeState(STATE_ANGER);
            }
            break;
        case STATE_ANGER:
            // 愤怒状态 - 保持1-2秒后回到空闲
            if (stateElapsed > (2000 + rand() % 1000)) {
                changeState(STATE_PREIDLE);
            }
            break;
        
        case STATE_SLEEPING:
            // 睡眠状态 - 保持3-5秒后转为空闲
            if (stateElapsed > (3000 + rand() % 2000)) {
                changeState(STATE_IDLE);
            }
            break;
        
        case STATE_PREWINCE:
            if (stateElapsed > 100) {
                changeState(STATE_WINCE);
                {
                    int idx = esp_random() % 2;
                    const char* audio_files[] = {
                        "/bangboo_audio/1s_4.wav",
                        "/bangboo_audio/1s_5.wav"
                    };
                    HAL::PlayWavFile(audio_files[idx]);
                }
            }
            break;
        case STATE_WINCE:
            if (stateElapsed > 1800) {
                changeState(STATE_POSTWINCE);
            }
            break;
        case STATE_POSTWINCE:
            if (stateElapsed > 100) {
                changeState(STATE_IDLE);
            }
            break; 
    }
}

// 改变状态
void AppBangboo::changeState(BangbooState_t newState) {
    if (_data.currentState != newState) {
        _data.currentState = newState;
        _data.stateStartTime = HAL::Millis();
        
        // 调试信息
        const char* stateNames[] = {"SLEEPING", "IDLE", "HAPPY", "BLINKING"};
        spdlog::debug("状态改变: {}", stateNames[newState]);
    }
}

// 根据状态获取对应的表情
AppBangboo::ExpressionType_t AppBangboo::getExpressionForState(BangbooState_t state) {
    switch (state) {
        case STATE_SLEEPING:  return EXPR_SLEEP;

        case STATE_PREIDLE:   return EXPR_BLINK;
        case STATE_IDLE:      return EXPR_EYES;

        case STATE_PREHAPPY:  return EXPR_BLINK;
        case STATE_HAPPY:     return EXPR_SMILE;

        case STATE_BLINKING0:  return EXPR_BLINK;
        case STATE_BLINKTEMP:  return EXPR_EYES;
        case STATE_BLINKING1:  return EXPR_BLINK;

        case STATE_PREANGER:  return EXPR_BLINK;
        case STATE_ANGER:      return EXPR_ANGER;

        case STATE_PREWINCE:  return EXPR_WINCE;
        case STATE_WINCE:     return EXPR_WINCE;
        case STATE_POSTWINCE: return EXPR_BLINK;

        case STATE_PRESAD:  return EXPR_BLINK;
        case STATE_SAD:     return EXPR_SAD;

        default:              return EXPR_EYES;
    }
}
