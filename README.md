# OpenGL
偷懒没有把类函数声明和函数体分开到.h和.cpp，放在一起了，如果分开放，#include .h文件只需一些声明，效率更高。当需要另一个文件的宏定义时，需要include .h，如果把函数定义也放.h，效率会很低。
invalid render buffer,排查后发现是忘记generate renderbuffer id就bind。
invalid enum ,错误原因，过滤设置错误。