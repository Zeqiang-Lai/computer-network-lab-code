# 实验二 实现透明传输程序

C++实现：金靖轩，蒋安立

Java实现：同上。

Python实现：同上。

## C++

### 编译说明

```shell
g++ -o Bit_Fill.exe Bit_Fill.cpp
g++ -o Byte_Fill.exe Byte_Fill.cpp
```

### 运行说明

```
运行命令：
./Bit_Fill.exe [configurations]
./Byte_Fill.exe [configurations]
```

### 配置文件

输入存储在配置文件中，其格式如下：

1-2行可替换为你的输入，注意：括号内不要作为输入

Bit_Fill_data.ini文件：

```
InfoString=01100101011111111111101010101110    （64bits）
FlagString=01111110
```

Byte_Fill_data.ini文件：

```
InfoString=347D7E807E40AA7D                    （32bits）
FlagString=7E
```

### 配置说明

linux下和windows下读取配置文件中换行符的区别：

- windows下换行，向文件中写的是\r\n，程序读取为\n
- linux下换行，向文件中写的是\n，程序读取为\n

所以在windows下配置的ini文件到linux下程序会多读一个\r，造成错误

**解决办法**

```
使用vi打开配置文件 
 
vi Byte_Fill_data.ini 

命令模式下输入 

:set fileformat=unix 
:w
:q
```



## Java

### 编译说明

```shell
// 怎么编译？，在这里写编译指令。
javac ....
不会Java命令行编译的话学下，学不懂？你可以选择弃疗，然后把这部分删了。
```

### 运行说明

写以下内容：

- 怎么运行？
- 怎么输入？
- 配置文件的格式？



## Python

### 运行说明

写以下内容：

- 怎么运行？
- 怎么输入？
- 配置文件的格式？