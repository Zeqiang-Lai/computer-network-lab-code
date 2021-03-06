# 实验三 基于停止等待协议的可靠通信

C++实现：蒋安立 金靖轩

Java实现：刘文卓 

Python实现：李安腾 张诗意

## C++

### 编译说明

```shell
g++ Sender.cpp -o Sender
g++ Receiver.cpp -o Receiver
```

### 运行说明

shell

./Receiver   //Linux

./Sender     //Linux

Receiver.exe    //Windows

Sender.exe     //Windows

// configuration file path 默认为init.txt

## Java

### 编译说明

```shell
javac Receiver.java
javac Sender.java

```

### 运行说明
选择jar文件运行
java -jar Sender.jar [configuration]
java -jar Receiver.jar [configuration]

配置文件
TIMEOUT=5000
PORT=8888
FILTERERROR=10
FILTERLOST=10
SEND_STR=message1
PRODUCTOR_STR=10001000000100001

## Python

### 运行说明
将server.py、client.py、crc.py、crc.ini放在同一个文件夹下，运行server.py，再运行client.py即可

## 输出格式

### Sender

```
Next frame to send:    [number]
Sending State:         Normal | Lost | Error
Receiving...
Receiving State:       Ack[number]   | Timeout
```

例子：

```
Next frame to send:    0
Sending State:         normal
Receiving...
Receiving State:       Ack1
---------------------------------
Next frame to send:    1
Sending State:         Lost
Receiving...
Receiving State:       Timeout
---------------------------------
Next frame to send:    1
Sending State:         Error
Receiving...
Receiving State:       Timeout
---------------------------------
```

### Receiver

```
Frame expected: [number]
Checking...
Invalid: CRC = [number]  | Valid: Frame[number]
Sending ACK[number]
```

例子：

```
Frame expected: 1
Checking...
Valid: Frame[1]
Sending ACK[1]
---------------------------------
Frame expected: 2
Checking...
Invalid: CRC = [1001112]         // invalid就没有send ack了
---------------------------------
```

## 关于模拟发送的补充说明

根据`编程实验要求.docx`，实验3的要求是

> 假设Host1要向Host2发送大文件，通过数据链路层的帧每次完成数据块的可靠传输

如果严格按照要求发送文件的话，这个模拟过程是要涉及到拆分文件的，即将文件拆分成许多个帧进行发送。

但是，鉴于大家的时间和能力，我们这里选择暂时忽略掉文件的问题。我们的策略是

> 每个帧都发送相同的内容。内容保存在crc.ini文件里（安腾写的）
>
> PS：这个文件命名不好，容易误解，需要改下@安腾。

### 关于帧大小的问题

实验采用UDP Socket接口模拟物理层的接口。实际上Socket更接近于数据链路层的接口，所以我们的工作实际上是建立在数据链路层上的对数据链路层的模拟。这句话比较难读，好好理解。

因此，我们的帧的组成是高度化简的，简单来说，实验的3的一个frame（帧）只包含两个内容：

```
[数字部分][数据部分]
```

- 帧的第一部分为一个数字，根据不同帧可能是frame expected或者是next frame to send。
- 帧的第二部分是数据，这里为字符串。

```python
# 对应python代码
frame=struct.pack('h100s',next_frame_to_send,bytes(data,'utf-8')) 
```

而实际上的帧的帧除了包含上述内容，还需要包含起始结束标记等等信息。

在这里我们的帧不包含起始结束标记的原因是，socket会帮我们做这些。

**因此**

- 关于要不要切帧或者是帧大小多大这两个问题：
  - 没有切帧。
  - 帧格式在上面写着，但是大小不固定。

