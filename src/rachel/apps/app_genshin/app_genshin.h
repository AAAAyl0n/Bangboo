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
#include <M5EchoBase.h>
#include <math.h>
#include "assets/icon_app_genshin.hpp"

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