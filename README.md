sc-linux-toolbox
===================================
****
### Author: Sl0v3C
### E-mail: pyy101727@gmail.com
****
	This project contains many scripts or linux tools which run in PC or ARM devices.
	Many tools need run in Android device and need some Android shared libraries. The dependency rules in Android.mk 
	Wish these tools can help you.
### 1. back -- add back function into .bashrc, then you can back to the num folder by "back Num" when you want and back to your last folder by "back b".
	Copy the content of the back file into .bashrc, then source .bashrc. You can use "back" in your shell.
``` bash
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
```
	Usage: back Num --> Means you can back to the number level parent of current folder. 
	                    If you reach the root folder, it will stop.
	       back b   --> Means you can return the last folder when you execute the "back Num".
		   
### 2. push_mp3 -- Push MP3 files into your ARM device by adb push command
	It will push *.MP3 or *.mp3 into /sdcard/Music/ of your ARM device.
	Usage: ./push_mp3 *.mp3

### 3. recursive_rm_null_directory.sh -- Recursively delete empty directories.
	Usage: ./recursive_rm_null_directory.sh  $1 # $1 must be absolute path

### 4. seek -- Which use the vi to the exact line you searched.
	Usage: When you use "grep" command to search some pattern, it will return like follows:
	       /Uboot/u-boot-2016-09/Makefile:253：  else if [ -x /bin/bash ]; then echo /bin/bash; 
	       Then you can use "seek /Uboot/u-boot-2016-09/Makefile:253:"
	       it equals to "vi /Uboot/u-boot-2016-09/Makefile +253"
		   
### 5. yy_cp -- Enhance for the cp command.
	Usage: 1: Copy file to file
	         Example: yy_cp source ~/a/b/c/d/e/test
	       2: Copy file/dir to direcory which not exist yet
	         Example: yy_cp source -d ~/a/b/c/d/e/temp/
    
### 6. logpyy -- it can run in adb shell & capture the logcat logs into file or show in display.
	Usage：logpyy
	     input args: logstart   start to capture log and save to /data/tinylogcat.txt
	                 logstop    stop to capture
	                 logexit    exit the tinylogthread thread
	                 exit       eixt logpyy

### 7. pthread_yyaudioplayer -- A tool which can run in adb shell to play music files by using MediaPlayer in Andriod.
	Usage: Compile the yyaudioplayer tool by Android.mk
	       run yyaudioplayer in adb shell
***Notice: It use the libmedia of the Android. It apply the Apache Licence.***
	
### 8. yypgrep -- yy_pgrep can search the process name and return its PID.
	Usage: Compile it via Android.mk
	       Then push it into Andriod device
	       Run it in adb shell： yygrep mediaserver
	       It like pgrep tool.

### 9. tinypyyplayer -- It contains many tools:
	Usage: A. pcmplay
	        run "pcmplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]" 
		in adb shell
	       B. mp3ToPcm
	        run "mp3ToPcm 1.mp3 2.pcm" in adb shell
	        it can parse the 1.mp3 file into 2.pcm file.
	       C. tinypyyplay
	        run "tinypyyplay sample.mp3 out.pcm [-D card] [-d device]" in adb shell
	        it can play sample.mp3 and parse it to out.pcm file.
	       D. mp3Parse
	        run "mp3Parse 1.mp3" in adb shell
	        it can help you to parse mp3 file and get the sampleRate/channels/bitRate
		and so on.
	       E. tinypyyplay_fifo
	        run "tinypyyplay_fifo sample.mp3 FIFO_name [-D card] [-d device]" in adb shell
	        it can create fifo file, decoding & playing at the same time.
	Notice: -D card & -d device should be the correct number with your android device.
 ***Notice: This tool use the libmad & id3v2lib.***
	
	   
	     
	
