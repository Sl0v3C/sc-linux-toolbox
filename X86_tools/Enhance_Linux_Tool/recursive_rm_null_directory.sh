#!/bin/bash

# recursive_rm_null_directory script used in bash shell
#                                                                                                                                                                                                                  
# Copyright (C) 2016 Sl0v3C.
#
# Author: Sl0v3C <pyy101727@gmail.com>
#
# License terms:
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.


# $1 must be absolute path
path=$1

find_del_ndir(){
    echo $path
    cd $path

    for item in `ls -l | grep "^d" | awk '{print $9}'`; do
        path=$path/$item
        find_del_ndir $path
    done

    dirc=`ls $path`
    if [ "$dirc" = "" ]; then # 判断是否为空： 1. "$dirc" = "" 2. !$dirc 3. if test -z "$dirc"
    echo $path
    rm -rf $path
    fi
    path=${path%/*}
}

find_del_ndir


