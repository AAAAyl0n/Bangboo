# Bangboo！

bug1: 音频app再次打开会触发重启（退出逻辑和线程需要设计）
bug2: iic冲突，需要修改EchoBase库的代码

I2C引脚: SDA = GPIO 14, SCL = GPIO 13
ES8311 音频编解码器 (0x18)

SD卡目前的文件结构如下：
├── bangboo_audio                     
│   ├── hello.wav                     测试用音频
├── fonts                             字体
│   └── font_text_24.vlw