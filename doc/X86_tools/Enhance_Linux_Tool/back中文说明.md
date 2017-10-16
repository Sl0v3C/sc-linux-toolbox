# back  
将back中的内容粘贴到.bashrc文件中，实现为一个函数:·
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
	这样在source .bashrc文件后就可以直接在shell下使用back + 参数来实现该功能。
	用法：back 1 --> 表示后退到前一级目录，类似cd ../
	     back Num --> 表示后退Num级目录，如果已经回退到/根目录则无论Num多大，都至多进入到/根目录
	     back b --> 表示进入到执行back Num之前的那一级目录