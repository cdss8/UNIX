#!/bin/bash
SCRIPT_DIR=" $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
WORK_DIR=$(mktemp -d)

trap exit_handler EXIT HUP INT QUIT PIPE TERM
function exit_handler(){
	local rc=$?
	[ -d $WORK_DIR ] && rm -r $WORK_DIR
	exit $rc
}

search_line=//Output:
compile_name=hi

if (gcc --version)
then
	if grep "$search_line" main.cpp
	then
		compile_path=$(grep "$search_line" main.cpp)
		compile_name=$(echo "$compile_path" | cut -d':' -f 2)
	fi
	make FN=$WORK_DIR/$compile_name
else
	echo "gcc not installed"
	exit 1
fi
cp -f $WORK_DIR/$compile_name $SCRIPT_DIR
rm -r $WORK_DIR
exit 0

