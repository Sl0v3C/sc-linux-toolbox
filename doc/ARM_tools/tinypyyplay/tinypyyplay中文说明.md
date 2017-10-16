# tinypyyplayer
	用法：A.pcmplay
	      使用方法： pcmplay sample.pcm -D 声卡号 -d pcm设备号
	      可以在adb shell里直接播放pcm文件
	     B. mp3Topcm
	      使用方法： mp3Topcm 1.mp3 2.pcm
	      在adb shell里将mp3文件解码成pcm文件
	     C. tinypyyplay
	      使用方法： tinypyyplay 1.mp3 2.pcm
	      在adb shell里直接播放mp3文件
	     D. mp3Parse
	      使用方法： mp3Parse 1.mp3
	      在adb shell里解析出MP3的采样率，CHANNELS，比特率等
	     E. tinypyyplay_fifo
	      使用方法： tinypyyplay_fifo sample.mp3 FIFO_name [-D card] [-d device]
	      在adb shell里创建fifo来边解码边播放
***注意：-D 声卡号 -d pcm设备号 必须对应你的android设备的声卡号和PCM设备号***  
***注意：该工具需要使用到 libmad 和 libid3v2 ***