# pthread_yyaudioplayer
当开发新项目时，有时候触摸屏不好用，就可以通过adb shell来直接通过该工具来播放MP3, 从而观察audio从上至下是否工作良好。  

	用法：通过Android.mk规则编译出yyaudioplayer并push到Android设备中。
	     在adb shell中运行yyaudioplayer.
***注意：该工具使用Android中的libmeia库***