# back  
add back function into .bashrc, then you can back to the num folder by "back Num" when you want and back to your last folder by "back b".  
Copy the content of the back file into .bashrc, then source .bashrc.   
You can use "back" in your shell.
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