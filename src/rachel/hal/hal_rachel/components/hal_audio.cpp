/**
 * @file hal_audio.cpp
 * @author Forairaaaaa
 * @brief HAL层音频系统实现
 * @version 0.1
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <mooncake.h>
#include "../hal_rachel.h"
#include <Arduino.h>
#include "../hal_config.h"
#include <FS.h>
#include <SD.h>
#include "driver/i2s.h"

// 静态成员，用于任务访问HAL实例
static HAL_Rachel* g_hal_instance = nullptr;

void HAL_Rachel::_audio_init()
{
    spdlog::info("音频系统初始化开始...");
    
    // 设置全局HAL实例指针，供静态任务函数使用
    g_hal_instance = this;
    
    // 创建M5EchoBase实例
    _echobase = new M5EchoBase(I2S_NUM_0);
    if (_echobase == nullptr) {
        spdlog::error("无法创建M5EchoBase实例!");
        return;
    }
    
    
    // 初始化音频硬件 - 使用设备引脚配置和I2C总线
    const int SAMPLE_RATE = 48000;
    bool init_success = _echobase->init(
        SAMPLE_RATE,    // 采样率
        14,             // I2C SDA
        13,             // I2C SCL  
        36,             // I2S DIN
        37,             // I2S WS
        38,             // I2S DOUT
        35,             // I2S BCK
        _i2c_bus        // I2C总线实例
    );
    
    if (!init_success) {
        spdlog::error("M5EchoBase初始化失败!");
        delete _echobase;
        _echobase = nullptr;
        return;
    }

    // 设置音频参数
    _echobase->setSpeakerVolume(_audio_volume);        // 音量设置
    _echobase->setMicGain(ES8311_MIC_GAIN_6DB);       // 设置麦克风增益
    _echobase->setMute(_audio_muted);                 // 设置静音状态
    
    // 创建FreeRTOS音频队列
    _audio_queue = xQueueCreate(5, sizeof(AudioCommand_t));
    if (_audio_queue == nullptr) {
        spdlog::error("无法创建音频命令队列");
        delete _echobase;
        _echobase = nullptr;
        return;
    }
    
    // 创建音频播放任务
    BaseType_t result = xTaskCreatePinnedToCore(
        _audioPlaybackTask,        // 任务函数
        "HAL_AudioTask",           // 任务名称
        12288,                     // 栈大小12KB
        this,                      // 参数（传递HAL实例）
        configMAX_PRIORITIES - 4,  // 优先级
        &_audio_task_handle,       // 任务句柄
        0                          // 分配到core0
    );
    
    if (result != pdPASS) {
        spdlog::error("无法创建音频播放任务");
        vQueueDelete(_audio_queue);
        _audio_queue = nullptr;
        delete _echobase;
        _echobase = nullptr;
        return;
    }
    
    spdlog::info("音频系统初始化完成！");
    i2s_zero_dma_buffer(I2S_NUM_0);
}

// FreeRTOS音频播放任务（静态函数）
void HAL_Rachel::_audioPlaybackTask(void* parameter)
{
    HAL_Rachel* hal = static_cast<HAL_Rachel*>(parameter);
    AudioCommand_t command;
    
    spdlog::info("HAL音频播放任务已启动");
    
    while (true) {
        // 等待音频播放命令
        if (xQueueReceive(hal->_audio_queue, &command, portMAX_DELAY) == pdTRUE) {
            // 设置播放状态
            hal->_is_audio_playing = true;
            hal->_should_stop_audio = false;
            
            spdlog::info("开始播放音频: {}", command.filename);
            
            // 播放前取消静音
            if (hal->_echobase != nullptr) {
                hal->_echobase->setMute(false);
            }
            
            // 播放音频文件
            bool success = _playWavFileInTask(*command.fs_ptr, command.filename);
            
            // 播放结束后恢复静音
            if (hal->_echobase != nullptr) {
                hal->_echobase->setMute(hal->_audio_muted);
            }
            
            if (success) {
                spdlog::info("音频播放完成");
            } else {
                spdlog::error("音频播放失败");
            }
            
            // 清除播放状态
            hal->_is_audio_playing = false;
            hal->_should_stop_audio = false;
        }
    }
}

// 在任务中播放WAV文件（支持中断）
bool HAL_Rachel::_playWavFileInTask(FS& fs, const char* filename)
{
    if (g_hal_instance == nullptr || g_hal_instance->_echobase == nullptr) {
        spdlog::error("音频系统未正确初始化");
        return false;
    }
    
    // 打开文件
    File file = fs.open(filename, FILE_READ);
    if (!file) {
        spdlog::error("无法打开音频文件: {}", filename);
        return false;
    }

    // 读取WAV头部
    WavHeader_t wav_header;
    if (file.read((uint8_t*)&wav_header, sizeof(wav_header)) != sizeof(wav_header)) {
        spdlog::error("无法读取WAV文件头");
        file.close();
        return false;
    }

    // 复制packed字段到局部变量避免编译错误
    uint16_t audiofmt = wav_header.audiofmt;
    uint16_t channels = wav_header.channel;
    uint32_t sample_rate = wav_header.sample_rate;
    uint16_t bits_per_sample = wav_header.bit_per_sample;

    // 验证WAV文件格式
    if (memcmp(wav_header.RIFF, "RIFF", 4) != 0 || 
        memcmp(wav_header.WAVEfmt, "WAVEfmt ", 8) != 0 ||
        audiofmt != 1) {
        spdlog::error("不支持的WAV文件格式");
        file.close();
        return false;
    }

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
    SubChunk_t sub_chunk;
    bool found_data = false;
    
    file.seek(sizeof(WavHeader_t));
    
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

    uint32_t data_size = sub_chunk.chunk_size;
    spdlog::info("找到音频数据，大小: {} 字节", data_size);

    // 播放PCM数据
    const size_t CHUNK_SIZE = 8192;  // 8KB缓冲区
    uint8_t* buffer = (uint8_t*)malloc(CHUNK_SIZE);
    if (!buffer) {
        spdlog::error("无法分配音频缓冲区");
        file.close();
        return false;
    }
    
    size_t bytes_remaining = data_size;
    uint32_t total_chunks = (data_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    uint32_t chunks_processed = 0;

    while (bytes_remaining > 0 && file.available() && !g_hal_instance->_should_stop_audio) {
        size_t bytes_to_read = min(CHUNK_SIZE, bytes_remaining);
        size_t bytes_read = file.read(buffer, bytes_to_read);
        
        if (bytes_read > 0) {
            size_t bytes_written = 0;
            esp_err_t err = i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
            if (err != ESP_OK) {
                spdlog::error("I2S写入失败");
                break;
            }
            bytes_remaining -= bytes_read;
        } else {
            break;
        }
        
        chunks_processed++;
        
        // 定期让出CPU
        if ((chunks_processed * 4) % total_chunks == 0) {
            taskYIELD();
        }
    }
    
    free(buffer);
    file.close();
    
    // 检查是否是被停止的
    if (g_hal_instance->_should_stop_audio) {
        spdlog::info("音频播放被用户停止");
    } else {
        spdlog::info("音频播放正常完成");
    }
    
    // 清零DMA缓冲区以消除播放结束后的噪声
    i2s_zero_dma_buffer(I2S_NUM_0);
    
    return true;
}

// HAL接口实现
bool HAL_Rachel::playWavFile(const char* filename)
{
    if (_audio_queue == nullptr || _echobase == nullptr) {
        spdlog::error("音频系统未初始化");
        return false;
    }
    
    if (!checkSdCard()) {
        spdlog::error("SD卡未准备就绪");
        return false;
    }
    
    // 检查是否正在播放
    if (isAudioPlaying()) {
        spdlog::warn("已有音频在播放中，请先停止当前播放");
        return false;
    }
    
    // 创建播放命令
    AudioCommand_t command;
    strncpy(command.filename, filename, sizeof(command.filename) - 1);
    command.filename[sizeof(command.filename) - 1] = '\0';
    command.fs_ptr = &SD;  // 使用SD卡文件系统
    
    // 发送命令到队列
    if (xQueueSend(_audio_queue, &command, pdMS_TO_TICKS(100)) != pdTRUE) {
        spdlog::error("无法发送音频播放命令");
        return false;
    }
    
    spdlog::info("已发送音频播放命令: {}", filename);
    return true;
}

bool HAL_Rachel::isAudioPlaying()
{
    return _is_audio_playing;
}

void HAL_Rachel::stopAudioPlayback()
{
    _should_stop_audio = true;
    spdlog::info("已发送停止播放信号");
}

void HAL_Rachel::setAudioVolume(uint8_t volume)
{
    _audio_volume = volume;
    if (_echobase != nullptr) {
        _echobase->setSpeakerVolume(volume);
    }
}

uint8_t HAL_Rachel::getAudioVolume()
{
    return _audio_volume;
}

void HAL_Rachel::setAudioMute(bool mute)
{
    _audio_muted = mute;
    if (_echobase != nullptr) {
        _echobase->setMute(mute);
    }
} 