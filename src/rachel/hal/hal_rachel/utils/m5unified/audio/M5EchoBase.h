/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef M5ECHOBASE_H
#define M5ECHOBASE_H

#include <Arduino.h>
#include "../I2C_Class.hpp"
#include <FS.h>
#include "es8311.h"
#include "driver/i2s.h"

/**
 * @brief Class to manage M5EchoBase operations including audio input/output and codec control.
 */
class M5EchoBase {
public:
    M5EchoBase(i2s_port_t i2s_num = I2S_NUM_0) : es_handle(nullptr), i2s_num(i2s_num) {};
    ~M5EchoBase() {};

    /**
     * @brief Initializes the M5EchoBase with specific audio settings.
     *
     * @param sample_rate Sampling rate in Hz (defaults to 16000 Hz).
     * @param i2c_sda I2C data line pin number.
     * @param i2c_scl I2C clock line pin number.
     * @param i2s_di I2S data input pin.
     * @param i2s_ws I2S word select pin.
     * @param i2s_do I2S data output pin.
     * @param i2s_bck I2S bit clock pin.
     * @param i2c_bus Pointer to I2C_Class object (required, use HAL->getI2C()).
     * @return bool True if initialization was successful, false otherwise.
     * 
     * @example
     * // Usage example:
     * // M5EchoBase audio;
     * // HAL_Rachel* hal = static_cast<HAL_Rachel*>(HAL::get());
     * // audio.init(16000, 38, 39, 7, 6, 5, 8, hal->getI2C());
     */
    bool init(int sample_rate = 16000, int i2c_sda = GPIO_NUM_38, int i2c_scl = GPIO_NUM_39, int i2s_di = GPIO_NUM_7,
              int i2s_ws = GPIO_NUM_6, int i2s_do = GPIO_NUM_5, int i2s_bck = GPIO_NUM_8, m5::I2C_Class* i2c_bus = nullptr);

    /**
     * @brief Sets the speaker volume.
     *
     * @param volume Volume level (0-100).
     * @return bool True if volume was set successfully, false if the operation failed.
     */
    bool setSpeakerVolume(int volume);

    /**
     * @brief Sets the microphone gain.
     *
     * @param gain Microphone gain level.
     * @return bool True if gain was set successfully, false if the operation failed.
     */
    bool setMicGain(es8311_mic_gain_t gain);

    /**
     * @brief Mutes or unmutes the speaker.
     *
     * @param mute Set true to mute, false to unmute.
     * @return bool True if the mute state was set successfully, false if the operation failed.
     */
    bool setMute(bool mute);

    /**
     * @brief Calculates the buffer size needed for a recording of the specified duration.
     *
     * @param duration Recording duration in seconds.
     * @param sample_rate Sampling rate in Hz (defaults to 0).
     * @return int Buffer size in bytes required for the recording.
     */
    int getBufferSize(int duration, int sample_rate = 0);

    /**
     * @brief Determines the duration of a recording for a specified buffer size.
     *
     * @param size Buffer size in bytes.
     * @param sample_rate Sampling rate in Hz (defaults to 0).
     * @return int Duration in seconds of the recording.
     */
    int getDuration(int size, int sample_rate = 0);

    /**
     * @brief Records audio to a file.
     *
     * @param fs Reference to the filesystem to use.
     * @param filename Name of the file to save the recording to.
     * @param size Buffer size in bytes (should be pre-calculated using getBufferSize()).
     * @return bool True if the recording was successful, false if it failed.
     */
    bool record(FS& fs, const char* filename, int size);

    /**
     * @brief Records audio to a buffer.
     *
     * @param buffer Pointer to the buffer where audio data will be stored.
     * @param size Buffer size in bytes (should be pre-calculated using getBufferSize()).
     * @return bool True if the recording was successful, false if it failed.
     */
    bool record(uint8_t* buffer, int size);

    /**
     * @brief Plays audio from a file.
     *
     * @param fs Reference to the filesystem from which to play the audio.
     * @param filename Name of the file containing the audio to play.
     * @return bool True if playback was successful, false if it failed.
     */
    bool play(FS& fs, const char* filename);

    /**
     * @brief Plays audio from a buffer.
     *
     * @param buffer Pointer to the audio buffer.
     * @param size Size of the audio buffer in bytes.
     * @return bool True if playback was successful, false if it failed.
     */
    bool play(const uint8_t* buffer, int size);

private:
    // ES8311 codec handle
    es8311_handle_t es_handle;

    // I2S configuration
    i2s_port_t i2s_num;
    i2s_config_t i2s_cfg;
    i2s_pin_config_t i2s_pin_cfg;

    // I2C and I2S pin numbers
    int _i2c_sda;
    int _i2c_scl;
    int _i2s_di;
    int _i2s_ws;
    int _i2s_do;
    int _i2s_bck;

    m5::I2C_Class* _i2c_bus;

    // Initializes the ES8311 codec
    bool es8311_codec_init(int sample_rate);

    // Initializes the I2S driver
    bool i2s_driver_init(int sample_rate);

    // Initializes the PI4IOE5V6408 I2C expander
    bool pi4ioe_init();

    // Reads a byte from the I2C bus
    uint8_t wire_read_byte(uint8_t i2c_addr, uint8_t reg_addr);

    // Writes a byte to the I2C bus
    void wire_write_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t value);
};

#endif  // M5ECHOBASE_H