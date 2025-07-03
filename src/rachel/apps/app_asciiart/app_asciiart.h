/**
 * @file app_asciiart.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-11-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mooncake.h>
#include "../assets/icons/icons.h"
#include <cmath>
#include <string>


namespace MOONCAKE::APPS
{
    /**
    * @brief Asciiart 
    *
    */
    class AppAsciiart : public APP_BASE
    {
    private:
        struct Data_t
        {
            unsigned long count = 0;
            unsigned long last_update_time = 0;
            unsigned long last_button_time = 0;
            unsigned long start_time = 0; // 记录应用启动时间
            int pattern_type = 0; // 0: chaos, 1: waves
            const int width = 40;
            const int height = 19;
            const int slowdown_factor = 8;
            const unsigned long frame_interval = 1; // 50ms per frame
            const unsigned long button_debounce = 200; // 200ms button debounce
        };
        Data_t _data;

        // ASCII 艺术函数
        float chaosPattern(float x, float y, float t);
        float wavesPattern(float x, float y, float t);
        void drawAsciiArt();
        char getAsciiChar(float value);

    public:
        void onCreate() override;
        void onResume() override;
        void onRunning() override;
        void onDestroy() override;
    };

    class AppAsciiart_Packer : public APP_PACKER_BASE
    {
        std::string getAppName() override { return "Asciiart"; }
        void* getAppIcon() override { return (void*)image_data_icon_app_default; }
        void* newApp() override { return new AppAsciiart; }
        void deleteApp(void *app) override { delete (AppAsciiart*)app; }
    };
}