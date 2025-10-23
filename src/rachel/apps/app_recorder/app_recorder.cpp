/**
 * @file app_recorder.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "app_recorder.h"
#include "spdlog/spdlog.h"
#include "../../hal/hal.h"
#include "../assets/theme/theme.h"
#ifdef ESP_PLATFORM
#include <FS.h>
#include <SD.h>
#include "../../hal/hal_rachel/hal_rachel.h"
#endif
#include <ctime>

using namespace MOONCAKE::APPS;

// Like setup()...
void AppRecorder::onResume()
{
    spdlog::info("{} onResume", getAppName());
    _data.isRecording = false;
    _data.startWasPressed = false;
    _data.selectWasPressed = false;
    _data.lastUiUpdateMs = 0;

    // Use pure English font for better performance
    HAL::GetCanvas()->setFont(&fonts::Font0);
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->setTextColor(TFT_WHITE, TFT_BLACK);
}

// Like loop()...
void AppRecorder::onRunning()
{
    // Inputs
    bool startDown = HAL::GetButton(GAMEPAD::BTN_START);
    bool selectDown = HAL::GetButton(GAMEPAD::BTN_SELECT);

    if (startDown) {
        if (!_data.startWasPressed) {
            _data.startWasPressed = true;

#ifdef ESP_PLATFORM
            if (!HAL::CheckSdCard()) {
                HAL::PopFatalError("No SD card");
                return;
            }
#endif

            if (!_data.isRecording) {
                // Ensure directory and build path with timestamp YYMMDDHHMMSS
                auto time_now = HAL::GetLocalTime();
                char ts[32];
                strftime(ts, sizeof(ts), "%y%m%d%H%M%S", time_now);
#ifdef ESP_PLATFORM
                if (!SD.exists("/audio")) {
                    SD.mkdir("/audio");
                }
#endif
                snprintf(_data.lastSavedPath, sizeof(_data.lastSavedPath), "/audio/%s.wav", ts);

#ifdef ESP_PLATFORM
                bool okStart = static_cast<HAL_Rachel*>(HAL::Get())->startWavRecording(_data.lastSavedPath);
                if (okStart) {
                    _data.isRecording = true;
                    _data.recStartMs = HAL::Millis();
                } else {
                    HAL::GetCanvas()->fillScreen(TFT_BLACK);
                    HAL::GetCanvas()->setCursor(10, 60);
                    HAL::GetCanvas()->print("Start record failed");
                    HAL::CanvasUpdate();
                }
#else
                _data.isRecording = true;
                _data.recStartMs = HAL::Millis();
#endif
            } else {
                // Stop recording and save
#ifdef ESP_PLATFORM
                bool okStop = static_cast<HAL_Rachel*>(HAL::Get())->stopWavRecording();
#else
                bool okStop = true;
#endif
                _data.isRecording = false;

                HAL::GetCanvas()->fillScreen(TFT_BLACK);
                HAL::GetCanvas()->setCursor(10, 30);
                HAL::GetCanvas()->print("Recorder");
                HAL::GetCanvas()->setCursor(10, 60);
                if (okStop) {
                    HAL::GetCanvas()->print("Saved:");
                    HAL::GetCanvas()->setCursor(10, 80);
                    HAL::GetCanvas()->print(_data.lastSavedPath);
                } else {
                    HAL::GetCanvas()->print("Save failed");
                }
                HAL::GetCanvas()->setCursor(10, 120);
                HAL::GetCanvas()->print("START: Record  SELECT: Exit");
                HAL::CanvasUpdate();
            }
        }
    } else {
        _data.startWasPressed = false;
    }

    // SELECT exits only when not recording
    if (selectDown) {
        if (!_data.selectWasPressed && !_data.isRecording) {
            _data.selectWasPressed = true;
            destroyApp();
            return;
        }
    } else {
        _data.selectWasPressed = false;
    }

    // Recording step and UI
    if (_data.isRecording) {
#ifdef ESP_PLATFORM
        static_cast<HAL_Rachel*>(HAL::Get())->recordWavStep(8192);
#endif
        HAL::GetCanvas()->fillScreen(TFT_BLACK);
        HAL::GetCanvas()->setCursor(10, 30);
        HAL::GetCanvas()->print("Recorder");
        HAL::GetCanvas()->setCursor(10, 60);
        HAL::GetCanvas()->print("Recording...");
        unsigned long elapsed = (HAL::Millis() - _data.recStartMs) / 1000;
        HAL::GetCanvas()->setCursor(10, 80);
        HAL::GetCanvas()->printf("Elapsed: %lus", elapsed);
        HAL::GetCanvas()->setCursor(10, 120);
        HAL::GetCanvas()->print("START: Stop  SELECT: Disabled");
        HAL::CanvasUpdate();
        return;
    }

    // UI idle screen refresh (~10 fps)
    unsigned long now = HAL::Millis();
    if (!_data.isRecording && (now - _data.lastUiUpdateMs > 100)) {
        _data.lastUiUpdateMs = now;
        HAL::GetCanvas()->fillScreen(TFT_BLACK);
        HAL::GetCanvas()->setCursor(10, 30);
        HAL::GetCanvas()->print("Recorder");
        HAL::GetCanvas()->setCursor(10, 60);
        HAL::GetCanvas()->print("Ready.");
        if (_data.lastSavedPath[0] != '\0') {
            HAL::GetCanvas()->setCursor(10, 85);
            HAL::GetCanvas()->print("Last:");
            HAL::GetCanvas()->setCursor(10, 105);
            HAL::GetCanvas()->print(_data.lastSavedPath);
        }
        HAL::GetCanvas()->setCursor(10, 140);
        HAL::GetCanvas()->print("START: Record  SELECT: Exit");
        HAL::CanvasUpdate();
    }
}

void AppRecorder::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }
