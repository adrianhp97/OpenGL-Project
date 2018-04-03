#!/bin/sh
bindir=$(pwd)
cd /home/adrianhp97/Desktop/College/OpenGL-Project/Tugas3/Shader/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/adrianhp97/Desktop/College/OpenGL-Project/Tugas3/build/Shader 
	else
		"/home/adrianhp97/Desktop/College/OpenGL-Project/Tugas3/build/Shader"  
	fi
else
	"/home/adrianhp97/Desktop/College/OpenGL-Project/Tugas3/build/Shader"  
fi
