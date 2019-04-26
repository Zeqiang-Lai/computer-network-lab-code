# 实验四 基于连续ARQ协议的可靠通信

C++实现：金靖轩，赖泽强

Java实现：邹博文。

Python实现：同上。

## C++

### 编译说明

请使用visual studio进行编译。

### 运行说明

只支持Windows。

双击运行bin文件夹下两个可执行文件即可。

## Java

### 编译说明

```shell
javac Client.java
javac Server.java
```

### 运行说明

写以下内容：

-  运行方法

  - 选择Jar文件运行

    ```
    java -jar Server.jar
    java -jar Client.jar
    ```

  - 选择Class文件运行

    ```
    java GBNClient
    java GBNServer
    ```

- 输入格式

  - 运行程序后，程序会要求输入配置文件地址，Server端使用Server.ini，Client端使用Client.ini.
  - 也可直接回车采用内置地址

- 配置文件的格式

  - Server.ini

    ```ini
    UDPPort = 8888
    portReceive = 8888
    portSend = 8800
    FilterError = 10
    FilterLost = 10
    ```

  - Client.ini

    ```ini
  UDPPort = 8888
    portReceive = 8800
    portSend = 8888
    FilterError = 10
    FilterLost = 10
    ```
  
  - crc.ini

    ```ini
  crc = 10001000000100001
    ```


## Python

### 运行说明

写以下内容：

- 怎么运行？
- 怎么输入？
- 配置文件的格式？

## 输出格式

`|`表示或，输出其中的一个。

发送：

```
------------------ 发送 --------------------------------
Ack_expected=1,  Next_frame_to_send=1,  Frame_expected=1
待发送帧: [Num=1] [Ack=1] [Data="00000101010"]
模拟: 正确发送 | 传输出错  | 丢失
```

接受：

```
------------------ 接受 --------------------------------
收到帧: [Num=1] [Ack=1] [Data="00000101010"]
Frame_expected=1,  Ack_expected=1
CRC校验成功  | CRC校验失败
```

超时：

```
------------------ 超时 --------------------------------
Ack_expected=1,  Next_frame_to_send=1,  Frame_expected=1
待发送帧: [Num=1] [Ack=1] [Data="00000101010"]
```

