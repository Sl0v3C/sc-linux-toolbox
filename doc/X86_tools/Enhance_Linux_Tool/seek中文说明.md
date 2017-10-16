# seek
通过vi来快速的打开某个文件到某一行，一般搭配grep使用。
	
	用法：通常通过grep命令搜索某个pattern返回的内容，如： grep -rsn "bash" ./ 返回如下内容.
	     /Uboot/u-boot-2016-09/Makefile:253：  else if [ -x /bin/bash ]; then echo /bin/bash;
	     接着就可以使用seek /Uboot/u-boot-2016-09/Makefile:253:, 将会通过vi打开/Uboot/u-boot-2016-09/Makefile，
	     并跳到253行，等同于vi /Uboot/u-boot-2016-09/Makefile +253