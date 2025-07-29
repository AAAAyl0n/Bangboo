/**
 * @file app_genshin.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "app_genshin.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../../hal/hal_rachel/hal_rachel.h"
#include "../assets/theme/theme.h"
#include "../utils/system/ui/ui.h"
#ifdef ESP_PLATFORM
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include "driver/i2s.h"
#endif

using namespace MOONCAKE::APPS;

// 定义静态成员变量
M5EchoBase* AppGenshin::echobase = nullptr;

// 定义静态常量成员变量
const int AppGenshin::SAMPLE_RATE;

bool AppGenshin::playWavFile(FS& fs, const char* filename)
{
    // 打开文件
    File file = fs.open(filename, FILE_READ);
    if (!file) {
        spdlog::error("无法打开音频文件: {}", filename);
        return false;
    }

    // 读取WAV头部
    wav_header_t wav_header;
    if (file.read((uint8_t*)&wav_header, sizeof(wav_header)) != sizeof(wav_header)) {
        spdlog::error("无法读取WAV文件头");
        file.close();
        return false;
    }

    // 验证WAV文件格式
    if (memcmp(wav_header.RIFF, "RIFF", 4) != 0 || 
        memcmp(wav_header.WAVEfmt, "WAVEfmt ", 8) != 0 ||
        wav_header.audiofmt != 1) {
        spdlog::error("不支持的WAV文件格式");
        file.close();
        return false;
    }

    // 复制packed字段到局部变量避免编译错误
    uint16_t channels = wav_header.channel;
    uint32_t sample_rate = wav_header.sample_rate;
    uint16_t bits_per_sample = wav_header.bit_per_sample;

    // 打印WAV文件信息
    spdlog::info("WAV文件信息:");
    spdlog::info("  声道数: {}", channels);
    spdlog::info("  采样率: {} Hz", sample_rate);
    spdlog::info("  位深度: {} bit", bits_per_sample);

    // 验证音频格式兼容性
    if (bits_per_sample != 16) {
        spdlog::warn("警告: 音频位深度为{}位，可能影响播放质量", bits_per_sample);
    }
    if (channels > 2) {
        spdlog::error("不支持超过2声道的音频");
        file.close();
        return false;
    }

    // 寻找data chunk
    sub_chunk_t sub_chunk;
    bool found_data = false;
    
    // 跳过可能的额外格式数据
    file.seek(sizeof(wav_header_t));
    
    while (file.available()) {
        if (file.read((uint8_t*)&sub_chunk, 8) != 8) {
            break;
        }
        
        if (memcmp(sub_chunk.identifier, "data", 4) == 0) {
            found_data = true;
            break;
        } else {
            // 跳过这个chunk
            uint32_t skip_size = sub_chunk.chunk_size;
            file.seek(file.position() + skip_size);
        }
    }

    if (!found_data) {
        spdlog::error("未找到音频数据部分");
        file.close();
        return false;
    }

    // 复制packed字段到局部变量
    uint32_t data_size = sub_chunk.chunk_size;
    spdlog::info("找到音频数据，大小: {} 字节", data_size);

    // 播放PCM数据
    const size_t CHUNK_SIZE = 1024;
    uint8_t buffer[CHUNK_SIZE];
    size_t bytes_remaining = data_size;

    while (bytes_remaining > 0 && file.available()) {
        size_t bytes_to_read = min(CHUNK_SIZE, bytes_remaining);
        size_t bytes_read = file.read(buffer, bytes_to_read);
        
        if (bytes_read > 0) {
            size_t bytes_written = 0;
            esp_err_t err = i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
            if (err != ESP_OK) {
                spdlog::error("I2S写入失败");
                file.close();
                return false;
            }
            bytes_remaining -= bytes_read;
        } else {
            break;
        }
    }

    file.close();
    
    // 清零DMA缓冲区以消除播放结束后的噪声
    i2s_zero_dma_buffer(I2S_NUM_0);
    
    spdlog::info("音频播放完成");
    return true;
}

void AppGenshin::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
    
    // 检查SD卡
    if (!HAL::CheckSdCard()) {
        spdlog::error("SD卡未准备就绪!");
        HAL::PopFatalError("没SD卡啊朋友");
        return;
    }
    
    // 检查M5EchoBase是否已经初始化
    if (echobase != nullptr) {
        spdlog::info("音频系统已存在，取消静音并复用");
        echobase->setMute(false);  // 取消静音
        echobase->setSpeakerVolume(70);  // 确保音量设置
        return;
    }
    
    // 首次初始化 M5EchoBase
    spdlog::info("首次初始化 M5EchoBase...");
    
    // 创建M5EchoBase实例
    echobase = new M5EchoBase(I2S_NUM_0);

    // 获取HAL的I2C总线实例
    HAL_Rachel* hal_rachel = static_cast<HAL_Rachel*>(HAL::Get());
    if (hal_rachel == nullptr) {
        spdlog::error("无法获取HAL实例!");
        delete echobase;
        echobase = nullptr;
        return;
    }
    
    m5::I2C_Class* i2c_bus = hal_rachel->getI2C();
    if (i2c_bus == nullptr) {
        spdlog::error("无法获取I2C总线实例!");
        delete echobase;
        echobase = nullptr;
        return;
    }
    
    // 初始化音频硬件 - 使用设备引脚配置和HAL的I2C总线
    bool init_success = echobase->init(
        SAMPLE_RATE,    // 采样率
        14,             // I2C SDA
        13,             // I2C SCL  
        36,             // I2S DIN
        37,             // I2S WS
        38,             // I2S DOUT
        35,             // I2S BCK
        i2c_bus         // HAL的I2C总线实例
    );
    
    if (!init_success) {
        spdlog::error("M5EchoBase初始化失败!");
        delete echobase;
        echobase = nullptr;
        return;
    }

    echobase->setSpeakerVolume(70);             // 音量设置 (0-100)，当前为70%
    echobase->setMicGain(ES8311_MIC_GAIN_6DB);  // 设置麦克风增益
    echobase->setMute(false);  // 取消静音
    
    spdlog::info("音频系统首次初始化完成！");
}

void AppGenshin::onResume() { 
    spdlog::info("{} onResume", getAppName()); 
}

using namespace SYSTEM::UI;

void AppGenshin::onRunning()
{
    HAL::GetCanvas()->setFont(&fonts::Font0);

    // 显示音频播放状态
    HAL::GetCanvas()->fillScreen(TFT_BLACK);
    HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
    HAL::GetCanvas()->setTextSize(2);
    HAL::GetCanvas()->drawCenterString("Playing Audio", 120, 32);
    HAL::GetCanvas()->drawCenterString("hello.wav", 120, 62);

    HAL::CanvasUpdate();

#ifdef ESP_PLATFORM
    // 播放SD卡中的音频文件
    if (echobase != nullptr) {
        const String audio_path = "/bangboo_audio/bangboo.wav";
        
        // 检查文件是否存在
        if (SD.exists(audio_path)) {
            spdlog::info("正在播放音频文件: {}", audio_path.c_str());
            bool success = playWavFile(SD, audio_path.c_str());
            if (!success) {
                spdlog::error("播放音频文件失败!");
                HAL::GetCanvas()->drawCenterString("Play Failed!", 120, 92);
            } else {
                spdlog::info("音频播放成功");
                HAL::GetCanvas()->drawCenterString("Play Success!", 120, 92);
            }
        } else {
            spdlog::error("音频文件不存在: {}", audio_path.c_str());
            HAL::GetCanvas()->drawCenterString("File Not Found!", 120, 92);
        }
    }
#endif
    
    // 检查按键退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
        destroyApp();
    }
    
    delay(100);
}

void AppGenshin::onDestroy() { 
    spdlog::info("{} onDestroy", getAppName()); 
    
    // 只做静音处理，保持I2S驱动运行，避免重复初始化问题
    if (echobase != nullptr) {
        echobase->setMute(true);  // 静音，停止音频输出
        spdlog::info("音频已静音，保持驱动运行状态");
        // 注意：不删除echobase对象，让I2S驱动保持活跃状态
        // 这样下次启动时不会出现"register I2S object to platform failed"错误
    }
    
    spdlog::info("音频资源保持运行状态（推荐的硬件管理方式）");
}
