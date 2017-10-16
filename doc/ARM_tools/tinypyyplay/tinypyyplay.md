# tinypyyplayer  
It contains many tools:  

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
 ***Notice: This tool use the libmad & id3v2lib.***  