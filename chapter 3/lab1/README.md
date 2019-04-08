# 实验一 循环冗余校验CRC生成和校验程序

C++实现：赖泽强，黄天

Java实现：赖泽强，黄天

Python实现：赖泽强，黄天

## C++

### 编译说明

```shell
g++ crc.cpp -o crc
```

### 运行说明

```shell
./crc [confirguare path] // Linxu
crc.exe [confirguare path] // Windows
// confirguare path 默认为crc.ini
```

## Java

### 编译说明

```shell
javac -encoding UTF-8 crc.java
jar cfe crc.jar crc crc.class 
```

### 运行说明

```
java -jar crc.jar [confirguare path]
// confirguare path 默认为crc.ini
```

## Python

### 运行说明

```
python crc.py [confirguare path]
// confirguare path 默认为crc.ini
```

## 配置文件

输入存储在配置文件中，其格式如下:

4-6行可替换为你的输入。

```
# line1: polynomials
# line2: bit string to be send
# line3: bit string to be received
10001000000100001
01101111011011010010101010101101
01101111011011010010101010101101
```
