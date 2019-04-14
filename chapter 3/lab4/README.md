# 实验四 基于连续ARQ协议的可靠通信

C++实现：[谁实现的，自己把名字写这]，删掉方括号别犯傻。

Java实现：邹博文。

Python实现：同上。

## C++

### 编译说明

```shell
// 怎么编译？，在这里写编译指令。
g++ ....
```

### 运行说明

写以下内容：

- 怎么运行？
- 怎么输入？
- 配置文件的格式？

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