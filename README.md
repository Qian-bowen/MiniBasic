# README

关于语法树和错误处理的一些说明

* 错误类型显示在**结果界面**

* 不合法错误和命令，在命令行直接输入，会被**过滤**。
  * 若**错误命令不影响程序其他部分执行**，可以再次RUN，会生成语法树。
  * 若**错误命令影响程序运行**，必须先修改，才能运行。
* 语法树在程序运行结束后显示



## 错误类型

* 除0错误：DIVIDE BY ZERO
* 不合法指令：INVALID INSTRUCTION
* 未定义指令：INSTRUCTION NOT EXIST
* 未定义变量：UNDEFINED VAR
* 不合法表达式：INVALID EXPRESSION
* 不合法地址：INVALID GOTO ADDRESS

错误处理：语法预处理、运行时错误

todo:支持整形、浮点型加减法，支持字符串加法