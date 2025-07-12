/**
 * @file app_timeview.h
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
#include "assets/icon_app_timeview.h"

namespace MOONCAKE::APPS
{
    /**
     * @brief Timeview
     *
     */
    class AppTimeview : public APP_BASE
    {
    private:
        struct Data_t
        {
            unsigned long count = 0;
        };
        Data_t _data;

    public:
        void onCreate() override;
        void onResume() override;
        void onRunning() override;
        void onDestroy() override;
    };

    class AppTimeview_Packer : public APP_PACKER_BASE
    {
        std::string getAppName() override { return "Timeview"; }
        void* getAppIcon() override { return (void*)image_data_icon_app_timeview; }
        void* newApp() override { return new AppTimeview; }
        void deleteApp(void* app) override { delete (AppTimeview*)app; }
    };
} // namespace MOONCAKE::APPS