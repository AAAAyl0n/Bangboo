/**
 * @file app_timeview.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "app_timeview.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"
#include <string>
#include <ctime>
#include <nvs_flash.h>
#include <nvs.h>

using namespace MOONCAKE::APPS;

// Static function to calibrate RTC using compile time
static void calibrateRTCWithCompileTime()
{
    // Initialize NVS if not already done
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    nvs_handle_t nvs_handle;
    ret = nvs_open("timeview_rtc", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        spdlog::error("Failed to open NVS handle for RTC calibration");
        return;
    }

    // Get current stored compile time
    char stored_compile_time[32] = {0};
    size_t required_size = sizeof(stored_compile_time);
    ret = nvs_get_str(nvs_handle, "compile_time", stored_compile_time, &required_size);

    // Create current compile time string
    char current_compile_time[32];
    snprintf(current_compile_time, sizeof(current_compile_time), "%s %s", __DATE__, __TIME__);

    // Check if compile time has changed
    bool need_calibrate = false;
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        // First time, need to calibrate
        need_calibrate = true;
        spdlog::info("First time RTC calibration needed");
    } else if (ret == ESP_OK) {
        // Compare with stored compile time
        if (strcmp(stored_compile_time, current_compile_time) != 0) {
            need_calibrate = true;
            spdlog::info("Compile time changed, RTC calibration needed");
            spdlog::info("Old: {}, New: {}", stored_compile_time, current_compile_time);
        } else {
            spdlog::info("Compile time unchanged, skipping RTC calibration");
        }
    } else {
        spdlog::error("Failed to read stored compile time from NVS");
    }

    if (need_calibrate) {
        // Parse compile date and time
        tm compile_tm = {0};
        
        // Parse __DATE__ (format: "MMM DD YYYY")
        char month_str[4];
        int day, year;
        sscanf(__DATE__, "%s %d %d", month_str, &day, &year);
        
        // Convert month string to number
        const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (int i = 0; i < 12; i++) {
            if (strcmp(month_str, months[i]) == 0) {
                compile_tm.tm_mon = i;
                break;
            }
        }
        
        compile_tm.tm_mday = day;
        compile_tm.tm_year = year - 1900;
        
        // Parse __TIME__ (format: "HH:MM:SS")
        int hour, min, sec;
        sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
        compile_tm.tm_hour = hour;
        compile_tm.tm_min = min;
        compile_tm.tm_sec = sec;
        
        // Set weekday (optional, will be calculated automatically)
        compile_tm.tm_wday = -1;
        
        // Add 30 seconds to compensate for upload delay
        compile_tm.tm_sec += 30;
        
        // Handle overflow of seconds/minutes/hours
        if (compile_tm.tm_sec >= 60) {
            compile_tm.tm_sec -= 60;
            compile_tm.tm_min += 1;
            
            if (compile_tm.tm_min >= 60) {
                compile_tm.tm_min -= 60;
                compile_tm.tm_hour += 1;
                
                if (compile_tm.tm_hour >= 24) {
                    compile_tm.tm_hour -= 24;
                    compile_tm.tm_mday += 1;
                    
                    // Simple day overflow handling (not perfect for all months, but good enough)
                    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                    
                    // Check for leap year
                    bool is_leap = ((compile_tm.tm_year + 1900) % 4 == 0 && 
                                   (compile_tm.tm_year + 1900) % 100 != 0) || 
                                   (compile_tm.tm_year + 1900) % 400 == 0;
                    if (is_leap && compile_tm.tm_mon == 1) {
                        days_in_month[1] = 29;
                    }
                    
                    if (compile_tm.tm_mday > days_in_month[compile_tm.tm_mon]) {
                        compile_tm.tm_mday = 1;
                        compile_tm.tm_mon += 1;
                        
                        if (compile_tm.tm_mon >= 12) {
                            compile_tm.tm_mon = 0;
                            compile_tm.tm_year += 1;
                        }
                    }
                }
            }
        }
        
        // Set RTC time
        try {
            HAL::SetSystemTime(compile_tm);
            spdlog::info("RTC calibrated with compile time: {}", current_compile_time);
            
            // Store current compile time in NVS
            ret = nvs_set_str(nvs_handle, "compile_time", current_compile_time);
            if (ret == ESP_OK) {
                ret = nvs_commit(nvs_handle);
                if (ret == ESP_OK) {
                    spdlog::info("Compile time stored in NVS successfully");
                } else {
                    spdlog::error("Failed to commit compile time to NVS");
                }
            } else {
                spdlog::error("Failed to store compile time in NVS");
            }
        } catch (...) {
            spdlog::error("Failed to set RTC time, exception occurred");
        }
    }

    nvs_close(nvs_handle);
}

void AppTimeview::onCreate() { 
    spdlog::info("{} onCreate", getAppName()); 
    
    // Calibrate RTC with compile time if needed
    calibrateRTCWithCompileTime();
}

// Like setup()...
void AppTimeview::onResume() { spdlog::info("{} onResume", getAppName()); }

// Like loop()...
void AppTimeview::onRunning()
{
    HAL::LoadTextFont24();
    while(1)
    {
        // Get RTC time
        auto time_now = HAL::GetLocalTime();
        char date_buffer[32];
        char time_buffer[16];
        strftime(date_buffer, sizeof(date_buffer), "%a %d", time_now);
        strftime(time_buffer, sizeof(time_buffer), "%H:%M", time_now);

        HAL::GetCanvas()->fillScreen(THEME_COLOR_BLACK);
        HAL::GetCanvas()->setTextColor(THEME_COLOR_LawnGreen);
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->drawCenterString(date_buffer, 120, 40);
        HAL::GetCanvas()->setTextSize(2);
        HAL::GetCanvas()->drawCenterString(time_buffer, 120, 66);
        HAL::CanvasUpdate();

        HAL::Delay(1000);

        _data.count++;
        //if (_data.count > 5) {// Display for about 5 minutes
        //    destroyApp();break;
        //}
    }
}

void AppTimeview::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
