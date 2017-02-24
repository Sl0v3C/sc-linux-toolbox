# sc-linux-toolbox
This project contains many scripts or linux tools which run in PC or ARM devices.
本项目包含一些在PC或者ARM设备中运行的脚本或者linux工具。

Many tools need run in Android device and need some Android shared libraries. The dependency rules in Android.mk 
有些工具是运行在Android adb shell环境，需要一些Android动态库，一般都包含在Android.mk里。

Wish these tools can help you.
希望这个项目的工具能够对你有所帮助.

1. back -- add back function into .bashrc, then you can back to the num folder by "back Num" when you want and back to your last folder by "back b".
Copy the content of the back file into .bashrc, then source .bashrc. You can use "back" in your shell.
将back中的内容粘贴到.bashrc文件中，实现为一个函数:
function back()
{
    if [ $# -eq 1 ]; then
        if [ $1 = "b" ]; then
                cd $pathbak
        else
                pathbak=`pwd`
                local count=$1
                while :
                do
                local path=`pwd`
                if [ $path = "/" ]; then
                        echo "You are now in the root path..."
                        break
                fi
                if [ $count -eq 0 ]; then
                        break
                else
                        cd ..
                fi
                count=`expr $(($count-1))`
                done
        fi
    else
        echo "Usage: Please only take one arg, like: back 3 will cd ../../../"
        echo "or back b will return the path before"
    fi
}
这样再source .bashrc文件后就可以直接在shell下使用back +参数来实现该功能。
具体功能： back 1 --> 表示后退到前一级目录，类似cd ../
           back Num --> 表示后退Num级目录，如果已经回退到/根目录则无论Num多大，都至多进入到/根目录
		   back b --> 表示进入到执行back Num之前的那一级目录
Usage:     back Num --> Means you can back to the number level parent of current folder. If you reach the root folder, it will stop.
           back b   --> Means you can return the last folder when you execute the "back Num".
		   
2. push_mp3 -- Push MP3 files into your ARM device by adb push command
将指定的后缀名为.MP3/.mp3的文件push到ARM设备的/sdcard/Music/目录中
It will push *.MP3 or *.mp3 into /sdcard/Music/ of your ARM device.
Usage:     ./push_mp3 *.mp3

3. recursive_rm_null_directory.sh -- Recursively delete empty directories.
递归地删除空目录
用法：     ./recursive_rm_null_directory.sh  $1 # $1必须是绝对路径
Usage:     ./recursive_rm_null_directory.sh  $1 # $1 must be absolute path

4. seek -- Which use the vi to the exact line you searched.
通过vi来快速的打开某个文件到某一行，一般搭配grep使用。
用法：     通常通过grep命令搜索某个pattern返回的内容，如： grep -rsn "bash" ./ 返回如下内容.
           /Uboot/u-boot-2016-09/Makefile:253:	  else if [ -x /bin/bash ]; then echo /bin/bash; 
		   接着就可以使用seek /Uboot/u-boot-2016-09/Makefile:253:, 将会通过vi打开/Uboot/u-boot-2016-09/Makefile，并跳到253行，等同于vi /Uboot/u-boot-2016-09/Makefile +253
Usage:     When you use "grep" command to search some pattern, it will return like follows:
           /Uboot/u-boot-2016-09/Makefile:253:	  else if [ -x /bin/bash ]; then echo /bin/bash; 
		   Then you can use "seek /Uboot/u-boot-2016-09/Makefile:253:", it equals to "vi /Uboot/u-boot-2016-09/Makefile +253"
		   
5. yy_cp -- Enhance for the cp command.
增强cp工具。
用法： 1. 可以将源文件拷贝到新目录下的新文件里
         例子： yy_cp source ~/a/b/c/d/e/test
	   2. 可以将源文件或者源目录拷贝到新目录下
         例子： yy_cp source -d ~/a/b/c/d/e/temp/
Usage: 1: Copy file to file
         Example: yy_cp source ~/a/b/c/d/e/test
	   2: Copy file/dir to direcory which not exist yet
	     Example: yy_cp source -d ~/a/b/c/d/e/temp/
    
6. logpyy -- it can run in adb shell & capture the logcat logs into file or show in display.
logpyy是一个可以在linux shell下抓取logcat device log信息并保存到/data/tinylogcat.txt文档下
用法： logpyy
         input args: logstart   开始抓取log并保存到/data/tinylogcat.txt
                     logstop    停止抓取
                     logexit    退出tinylogthread线程
                     exit       退出logpyy程序

7. pthread_yyaudioplayer -- A tool which can run in adb shell to play music files by using MediaPlayer in Andriod.
当开发新项目时，有时候触摸屏不好用，就可以通过adb shell来直接通过该工具来播放MP3，从而观察audio从上至下是否工作良好。
用户： 通过Android.mk规则编译出yyaudioplayer并push到Android设备中。
       在adb shell中运行yyaudioplayer.
	注意：该工具使用Android中的libmeia库
Usage: Compile the yyaudioplayer tool by Android.mk
       run yyaudioplayer in adb shell
	Notice: It use the libmedia of the Android. It apply the Apache Licence.
	
8. yypgrep -- yy_pgrep can search the process name and return its PID.
用于通过进程的名字找到其PID, 就是pgrep的实现。
用法： 通过Android.mk规则编译出yyaudioplayer并push到Android设备中。
Usage: Compile it via Android.mk
       Then push it into Andriod device
	   run it in adb shell： yygrep mediaserver

9. tinypyyplayer -- It contains many tools: A. pcmplay; B. mp3ToPcm; C. tinypyyplay D. mp3Parse E. tinypyyplay_fifo
用法： A.pcmplay
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
	   注意： -D 声卡号 -d pcm设备号 必须对应你的android设备的声卡号和PCM设备号
Usage: A. pcmplay
         run "pcmplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]" in adb shell
	   B. mp3ToPcm
	     run "mp3ToPcm 1.mp3 2.pcm" in adb shell
	     it can parse the 1.mp3 file into 2.pcm file.
	   C. tinypyyplay
	     run "tinypyyplay sample.mp3 out.pcm [-D card] [-d device]" in adb shell
	     it can play sample.mp3 and parse it to out.pcm file.
	   D. mp3Parse
         run "mp3Parse 1.mp3" in adb shell
         it can help you to parse mp3 file and get the sampleRate/channels/bitRate and so on.
       E. tinypyyplay_fifo
         run "tinypyyplay_fifo sample.mp3 FIFO_name [-D card] [-d device]" in adb shell
         it can create fifo file, decoding & playing at the same time.
	   Notice: -D card & -d device should be the correct number with your android device.

Notice: This tool use the libmad & id3v2lib.
	   
	     
	