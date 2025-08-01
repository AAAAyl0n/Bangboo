/**
 * @file app_genshin.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
 #include <mooncake.h>
 #include "../../hal/hal_rachel/utils/m5unified/audio/M5EchoBase.h"
 #include <math.h>
 #include "assets/icon_app_genshin.hpp"
 
 #ifdef ESP_PLATFORM
 #include <freertos/FreeRTOS.h>
 #include <freertos/task.h>
 #include <freertos/queue.h>
 #include <freertos/semphr.h>
 #endif
 
 // 前向声明
 class FS;
 
 namespace MOONCAKE::APPS
 {
     /**
      * @brief Genshin
      *
      */
     class AppGenshin : public APP_BASE
     {
     private:
         static M5EchoBase* echobase;
         static const int SAMPLE_RATE = 48000;  // 改为48kHz以匹配音频文件
         
 #ifdef ESP_PLATFORM
         // FreeRTOS相关静态成员
         static TaskHandle_t audio_task_handle;
         static QueueHandle_t audio_queue;
         // 移除互斥锁，使用原子变量
         static volatile bool is_playing;
         static volatile bool should_stop;
         
         // 音频播放命令结构
         struct audio_command_t {
             char filename[256];
             FS* fs_ptr;
         };
         
         // 静态任务函数
         static void audioPlaybackTask(void* parameter);
         static bool playWavFileInTask(FS& fs, const char* filename);
 #endif
         
         // WAV文件头结构
         struct __attribute__((packed)) wav_header_t {
             char RIFF[4];
             uint32_t chunk_size;
             char WAVEfmt[8];
             uint32_t fmt_chunk_size;
             uint16_t audiofmt;
             uint16_t channel;
             uint32_t sample_rate;
             uint32_t byte_per_sec;
             uint16_t block_size;
             uint16_t bit_per_sample;
         };
         
         struct __attribute__((packed)) sub_chunk_t {
             char identifier[4];
             uint32_t chunk_size;
             uint8_t data[1];
         };
         
         bool playWavFile(FS& fs, const char* filename);
         
 #ifdef ESP_PLATFORM
         // 新增：异步播放音频文件
         bool playWavFileAsync(FS& fs, const char* filename);
         // 检查播放状态
         bool isPlaying();
         // 停止播放
         void stopPlayback();
 #endif
         
     public:
         void onCreate() override;
         void onResume() override;
         void onRunning() override;
         void onDestroy() override;
     };
 
     class AppGenshin_Packer : public APP_PACKER_BASE
     {
         std::string getAppName() override { return "Genshin"; }
         void* getAppIcon() override { return (void*)image_data_icon_app_genshin; }
         void* newApp() override { return new AppGenshin; }
         void deleteApp(void* app) override { delete (AppGenshin*)app; }
     };
 } // namespace MOONCAKE::APPS