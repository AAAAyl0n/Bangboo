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
// 添加FreeRTOS头文件
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#endif

using namespace MOONCAKE::APPS;

// 定义静态成员变量
M5EchoBase* AppGenshin::echobase = nullptr;
#ifdef ESP_PLATFORM
TaskHandle_t AppGenshin::audio_task_handle = nullptr;
QueueHandle_t AppGenshin::audio_queue = nullptr;
// 移除互斥锁声明
volatile bool AppGenshin::is_playing = false;
volatile bool AppGenshin::should_stop = false;
#endif

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

#ifdef ESP_PLATFORM
// FreeRTOS音频播放任务
void AppGenshin::audioPlaybackTask(void* parameter)
{
    audio_command_t command;
    
    spdlog::info("音频播放任务已启动");
    
    while (true) {
        // 等待音频播放命令
        if (xQueueReceive(audio_queue, &command, portMAX_DELAY) == pdTRUE) {
            // 设置播放状态
            // 使用原子操作设置标志
            is_playing = true;
            should_stop = false;
            
            spdlog::info("开始在任务中播放音频: {}", command.filename);
            
            // 播放音频文件
            bool success = playWavFileInTask(*command.fs_ptr, command.filename);
            
            if (success) {
                spdlog::info("音频播放任务完成");
            } else {
                spdlog::error("音频播放任务失败");
            }
            
            // 清除播放状态
            // 使用原子操作清除标志
            is_playing = false;
            should_stop = false;
        }
    }
}

// 在任务中播放WAV文件（支持中断）
bool AppGenshin::playWavFileInTask(FS& fs, const char* filename)
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

    // 播放PCM数据（完全优化版本，最小化中断）
    const size_t CHUNK_SIZE = 8192;  // 进一步增加缓冲区到8KB
    uint8_t* buffer = (uint8_t*)malloc(CHUNK_SIZE);
    if (!buffer) {
        spdlog::error("无法分配音频缓冲区");
        file.close();
        return false;
    }
    
    size_t bytes_remaining = data_size;
    uint32_t total_chunks = (data_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    uint32_t chunks_processed = 0;

    while (bytes_remaining > 0 && file.available() && !should_stop) {
        size_t bytes_to_read = min(CHUNK_SIZE, bytes_remaining);
        size_t bytes_read = file.read(buffer, bytes_to_read);
        
        if (bytes_read > 0) {
            size_t bytes_written = 0;
            // 使用portMAX_DELAY确保数据完全写入
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
        
        // 进一步减少CPU让出频率：只在处理了总数据的25%、50%、75%时让出
        if ((chunks_processed * 4) % total_chunks == 0) {
            taskYIELD();
        }
    }
    
    free(buffer);

    file.close();
    
    // 检查是否是被停止的
    if (should_stop) {
        spdlog::info("音频播放被用户停止");
    } else {
        spdlog::info("音频播放正常完成");
    }
    
    // 清零DMA缓冲区以消除播放结束后的噪声
    i2s_zero_dma_buffer(I2S_NUM_0);
    
    return true;
}

// 异步播放音频文件
bool AppGenshin::playWavFileAsync(FS& fs, const char* filename)
{
    if (audio_queue == nullptr) {
        spdlog::error("音频队列未初始化");
        return false;
    }
    
    // 检查是否正在播放
    if (isPlaying()) {
        spdlog::warn("已有音频在播放中，请先停止当前播放");
        return false;
    }
    
    // 创建播放命令
    audio_command_t command;
    strncpy(command.filename, filename, sizeof(command.filename) - 1);
    command.filename[sizeof(command.filename) - 1] = '\0';
    command.fs_ptr = &fs;
    
    // 发送命令到队列
    if (xQueueSend(audio_queue, &command, pdMS_TO_TICKS(100)) != pdTRUE) {
        spdlog::error("无法发送音频播放命令");
        return false;
    }
    
    spdlog::info("已发送异步播放命令: {}", filename);
    return true;
}

// 检查播放状态
bool AppGenshin::isPlaying()
{
    return is_playing; // 直接返回原子变量
}

// 停止播放
void AppGenshin::stopPlayback()
{
    should_stop = true; // 直接设置原子变量
    spdlog::info("已发送停止播放信号");
}
#endif

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
        
#ifdef ESP_PLATFORM
        // 如果FreeRTOS任务还没有创建，创建它们
        if (audio_task_handle == nullptr) {
            spdlog::info("创建FreeRTOS音频任务...");
            
            // 创建音频命令队列
            audio_queue = xQueueCreate(5, sizeof(audio_command_t));
            if (audio_queue == nullptr) {
                spdlog::error("无法创建音频命令队列");
                return;
            }
            
            // 创建音频播放任务
            BaseType_t result = xTaskCreatePinnedToCore(
                audioPlaybackTask,     // 任务函数
                "AudioTask",           // 任务名称
                12288,                 // 增加栈大小到12KB
                nullptr,               // 参数
                configMAX_PRIORITIES - 2,  // 更高优先级（仅次于最高）
                &audio_task_handle,    // 任务句柄
                1                      // 固定在核心1
            );
            
            if (result != pdPASS) {
                spdlog::error("无法创建音频播放任务");
                vQueueDelete(audio_queue);
                audio_queue = nullptr;
                return;
            }
            
            spdlog::info("FreeRTOS音频任务创建成功");
        }
#endif
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
    
#ifdef ESP_PLATFORM
    // 创建FreeRTOS任务和同步对象
    spdlog::info("创建FreeRTOS音频任务...");
    
    // 创建音频命令队列
    audio_queue = xQueueCreate(5, sizeof(audio_command_t));
    if (audio_queue == nullptr) {
        spdlog::error("无法创建音频命令队列");
        return;
    }
    
    // 创建音频播放任务
    BaseType_t result = xTaskCreatePinnedToCore(
        audioPlaybackTask,     // 任务函数
        "AudioTask",           // 任务名称
        12288,                 // 增加栈大小到12KB
        nullptr,               // 参数
        configMAX_PRIORITIES - 2,  // 更高优先级（仅次于最高）
        &audio_task_handle,    // 任务句柄
        1                      // 固定在核心1
    );
    
    if (result != pdPASS) {
        spdlog::error("无法创建音频播放任务");
        vQueueDelete(audio_queue);
        audio_queue = nullptr;
        return;
    }
    
    spdlog::info("FreeRTOS音频任务创建成功");
#endif
    
    spdlog::info("音频系统首次初始化完成！");
}

void AppGenshin::onResume() { 
    spdlog::info("{} onResume", getAppName()); 
}

using namespace SYSTEM::UI;

void AppGenshin::onRunning()
{
    static bool audio_started = false;
    static uint32_t counter = 0;
    static uint32_t last_display_time = 0;
    
    uint32_t current_time = millis();
    
    HAL::GetCanvas()->setFont(&fonts::Font0);

#ifdef ESP_PLATFORM
    // 启动异步音频播放（只执行一次）
    if (!audio_started && echobase != nullptr) {
        const String audio_path = "/bangboo_audio/bangboo.wav";
        
        // 检查文件是否存在
        if (SD.exists(audio_path)) {
            spdlog::info("开始异步播放音频文件: {}", audio_path.c_str());
            bool success = playWavFileAsync(SD, audio_path.c_str());
            if (success) {
                audio_started = true;
                spdlog::info("音频异步播放命令发送成功");
            } else {
                spdlog::error("发送音频播放命令失败!");
            }
        } else {
            spdlog::error("音频文件不存在: {}", audio_path.c_str());
            audio_started = true;  // 防止重复检查
        }
    }
#endif

    // 每100ms更新一次显示（10fps），减少对音频任务的干扰
    if (current_time - last_display_time >= 100) {
        last_display_time = current_time;
        counter += 50;  // 调整递增量保持视觉效果
        
        // 清屏并显示信息
        HAL::GetCanvas()->fillScreen(TFT_BLACK);
        HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(2);
        
        // 显示标题
        HAL::GetCanvas()->drawCenterString("FreeRTOS Audio", 120, 20);
        
#ifdef ESP_PLATFORM
        // 显示播放状态
        String status_text = "Status: ";
        if (isPlaying()) {
            status_text += "Playing";
            HAL::GetCanvas()->setTextColor(TFT_GREEN, TFT_BLACK);
        } else if (audio_started) {
            status_text += "Finished";
            HAL::GetCanvas()->setTextColor(TFT_YELLOW, TFT_BLACK);
        } else {
            status_text += "Ready";
            HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        }
        HAL::GetCanvas()->drawCenterString(status_text, 120, 50);
        
        // 显示快速递增的计数器
        HAL::GetCanvas()->setTextColor(TFT_CYAN, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(3);
        String counter_text = "Count: " + String(counter);
        HAL::GetCanvas()->drawCenterString(counter_text, 120, 80);
        
        // 显示操作提示
        HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->drawCenterString("SELECT: Exit", 120, 120);
        if (isPlaying()) {
            HAL::GetCanvas()->drawCenterString("START: Stop Audio", 120, 135);
        } else if (audio_started) {
            HAL::GetCanvas()->drawCenterString("RIGHT: Replay Audio", 120, 135);
        }
#else
        // 非ESP平台显示
        HAL::GetCanvas()->drawCenterString("ESP32 Only", 120, 50);
        HAL::GetCanvas()->setTextColor(TFT_CYAN, TFT_BLACK);
        HAL::GetCanvas()->setTextSize(3);
        String counter_text = "Count: " + String(counter);
        HAL::GetCanvas()->drawCenterString(counter_text, 120, 80);
#endif
        
        HAL::CanvasUpdate();
    }
    
#ifdef ESP_PLATFORM
    // 处理按键
    if (HAL::GetButton(GAMEPAD::BTN_START) && isPlaying()) {
        // 停止播放
        stopPlayback();
        spdlog::info("用户停止音频播放");
    }
    
    if (HAL::GetButton(GAMEPAD::BTN_RIGHT) && !isPlaying() && audio_started) {
        // 重新播放
        const String audio_path = "/bangboo_audio/bangboo.wav";
        if (SD.exists(audio_path)) {
            bool success = playWavFileAsync(SD, audio_path.c_str());
            if (success) {
                spdlog::info("重新开始播放音频");
            }
        }
    }
#endif
    
    // 检查按键退出
    if (HAL::GetButton(GAMEPAD::BTN_SELECT)) {
#ifdef ESP_PLATFORM
        // 退出前停止播放
        if (isPlaying()) {
            stopPlayback();
        }
#endif
        destroyApp();
    }
    
    delay(50);  // 适当延时，减少对音频任务的CPU竞争
}

void AppGenshin::onDestroy() { 
    spdlog::info("{} onDestroy", getAppName()); 
    
#ifdef ESP_PLATFORM
    // 停止音频播放
    if (isPlaying()) {
        spdlog::info("停止正在播放的音频");
        stopPlayback();
        
        // 等待音频停止
        int timeout = 50;  // 500ms超时
        while (isPlaying() && timeout > 0) {
            delay(10);  // 使用Arduino的delay而不是vTaskDelay
            timeout--;
        }
    }
#endif
    
    // 只做静音处理，保持I2S驱动运行，避免重复初始化问题
    if (echobase != nullptr) {
        echobase->setMute(true);  // 静音，停止音频输出
        spdlog::info("音频已静音，保持驱动运行状态");
        // 注意：不删除echobase对象，让I2S驱动保持活跃状态
        // 这样下次启动时不会出现"register I2S object to platform failed"错误
    }
    
    // 注意：保持FreeRTOS任务运行，因为它们可能被其他app实例复用
    // 如果需要完全清理，可以在应用程序最终退出时进行
    
    spdlog::info("音频资源保持运行状态（推荐的硬件管理方式）");
}
