# README

## MiniBasic

A simple interpretor of Basic with GUI

You can edit your program or load it in this interpretor.

Debug is also supported.



## 关于错误处理的一些说明

* 错误分为：**语法错误**、**运行时错误**、**命令行错误**

* **语法错误**

  * 如变量名不合法，引号使用错误等（计算表达式不合法除外，比如“1+2-”）
  * 在LOAD程序，和命令行输入带行号的命令时出现在**结果界面**。

  * 在运行前必须全部修改，否则将无法运行代码
  * 实现：正则表达式过滤

* **运行时错误**

  * 如除零，表达式错误，GOTO地址不合法，变量未定义等
  * 一旦遇见，后面程序将不会执行，**错误将标红**，并且**提示框提示**

* **命令行错误**

  * 命令的错误，提示框提示

* **DEBUG模式下的错误处理**
* 通过命令行可以修改错误行，修改后可以继续运行
  * 如果不想继续debug，可以在命令行中输入CLEAR，可以直接退出调试



## 报错信息

* 除0错误：DIVIDE BY ZERO
* 不合法指令：INVALID INSTRUCTION XXX
* 未定义指令：INSTRUCTION NOT EXIST
* 未定义变量：UNDEFINED VAR
* 不合法表达式：INVALID EXPRESSION
* 不合法地址：INVALID GOTO ADDRESS
* 不合法命令：CMD NOT EXIST
* 程序不存在：PROGRAM MISSING
* 不合法运算数：INVALID OPERAND TYPE



## 其他

* DEBUG模式下，如果有INPUT指令，在按下DEBUG后需要按照命令行提示输入参数
* 所有部分**不支持中文**，在代码显示栏中中文会被显示为乱码。而**在命令行中，除了ascii外的所有字符都被设置为过滤**。

