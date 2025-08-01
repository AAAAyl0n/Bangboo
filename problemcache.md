[2025-07-29 00:52:40.931] [info] Genshin onCreate
[2025-07-29 00:52:40.932] [info] 正在初始化 M5EchoBase...
[  6827][I][M5EchoBase.cpp:41] init(): [EchoBase] Using provided I2C bus instance
[  6872][I][es8311.cpp:331] es8311_fmt_config(): [ES8311] ES8311 in Slave mode and I2S format
[  6885][I][M5EchoBase.cpp:98] es8311_codec_init(): [EchoBase] ES8311 codec initialized successfully
[2025-07-29 00:52:41.001] [info] 音频系统初始化完成！
[2025-07-29 00:52:41.002] [info] app: Genshin opened
[2025-07-29 00:52:41.386] [info] Genshin onResume
[2025-07-29 00:52:41.406] [info] 正在播放音频文件: /bangboo_audio/bangboo.wav
[2025-07-29 00:52:41.425] [info] WAV文件信息:
[2025-07-29 00:52:41.425] [info]   声道数: 2
[2025-07-29 00:52:41.426] [info]   采样率: 48000 Hz
[2025-07-29 00:52:41.428] [info]   位深度: 16 bit
[2025-07-29 00:52:41.445] [info] 找到音频数据，大小: 928260 字节
[2025-07-29 00:52:46.241] [info] 音频播放完成
[2025-07-29 00:52:46.241] [info] 音频播放成功
[2025-07-29 00:52:46.342] [info] Genshin onDestroy
[2025-07-29 00:52:46.343] [info] 音频资源已清理
[2025-07-29 00:52:46.343] [info] back to launcher
[2025-07-29 00:52:46.706] [info] Launcher onResume
[2025-07-29 00:52:55.958] [info] Genshin onCreate
[2025-07-29 00:52:55.959] [info] 正在初始化 M5EchoBase...
[ 21854][I][M5EchoBase.cpp:41] init(): [EchoBase] Using provided I2C bus instance
E (21470) I2S: register I2S object to platform failed
[ 21871][I][M5EchoBase.cpp:125] i2s_driver_init(): [EchoBase] Failed to install I2S driver
[ 21884][I][M5EchoBase.cpp:45] init(): [EchoBase] I2S driver initialization failed
[2025-07-29 00:52:55.996] [error] M5EchoBase初始化失败!
[2025-07-29 00:52:55.996] [info] app: Genshin opened
[2025-07-29 00:52:56.381] [info] Genshin onResume 退出app再次打开后就无法运行了，且不掉电重启时，会出现无法检测到SD卡的问题 @app_genshin.cpp  你觉得更好的做法是不是在系统启动的时候就一直开着音频驱动（i2s）


没有装载SD卡或者SD卡未检测到的时候，就会出现炸扬声器的声音。请你推测原因。