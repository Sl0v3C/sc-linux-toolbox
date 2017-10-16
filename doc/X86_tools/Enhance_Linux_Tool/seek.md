# seek  
Which use the vi to the exact line you searched.
	
	Usage: When you use "grep" command to search some pattern, it will return like follows:
	       /Uboot/u-boot-2016-09/Makefile:253：  else if [ -x /bin/bash ]; then echo /bin/bash; 
	       Then you can use "seek /Uboot/u-boot-2016-09/Makefile:253:"
	       it equals to "vi /Uboot/u-boot-2016-09/Makefile +253"