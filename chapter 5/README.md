# 第五章 网络层

所有使用都用三种语言, `C++`, `Java`, `Python3` 实现。

第五章共**四**个实验：

1. 基于ICMP的ping程序
2. 基于ICMP的traceroute程序
3. 距离矢量(Distance Vector)算法
4. 静态路由器软件

## 分组

- C++：蒋安立，金靖轩，赖泽强
- Python：李安腾，张诗意，黄天
- Java：刘文卓，邹博文

## 代码实现安排

> 静态路由器软件需求尚不明确。

写完自行提交源代码和可执行文件到对应文件夹，`src`和`bin`文件夹请自行创建。

| 语言 | 实验               | 人员 |
| -------- | ------------------ | ---- |
| C++      | 基于ICMP的ping程序 | 蒋安立 |
| C++      | 基于ICMP的traceroute程序 | 金靖轩 |
| C++ | 距离矢量(Distance Vector)算法 | 赖泽强 |

| 语言   | 实验                          | 人员   |
| ------ | ----------------------------- | ------ |
| Python | 基于ICMP的ping程序            | 黄天   |
| Python | 基于ICMP的traceroute程序      | 张诗意 |
| Python | 距离矢量(Distance Vector)算法 | 李安腾 |

| 语言 | 实验                          | 人员   |
| ---- | ----------------------------- | ------ |
| Java | 基于ICMP的ping程序            | 邹博文 |
| Java | 基于ICMP的traceroute程序      | 邹博文 |
| Java | 距离矢量(Distance Vector)算法 | 刘文卓 |

## 报告安排

**运行说明：** 参照lab3的README写。

**实验报告：** 

模版已经发群里了，请直接复制一份模版，在模版基础上直接写。模版显示效果有问题的联系我（标准的效果应该和pdf版本的一致）。

| 实验                               | 负责人 |
| ---------------------------------- | ------ |
| Lab1 基于ICMP的ping程序            | 黄天   |
| Lab2 基于ICMP的traceroute程序      | 张诗意 |
| Lab3 距离矢量(Distance Vector)算法 | 刘文卓 |
| Lab4 静态路由器软件                | 待定   |

需要运行截图的找对应的人要。遇到迟迟不回复的向我反映。

**汇报：**

| 实验                               | 负责人 |
| ---------------------------------- | ------ |
| Lab1 基于ICMP的ping程序            | 邹博文 |
| Lab2 基于ICMP的traceroute程序      | 张诗意 |
| Lab3 距离矢量(Distance Vector)算法 | 李安腾 |
| Lab4 静态路由器软件                | 待定   |

## 实验要求解释

### Lab1 基于ICMP的ping程序

与windows的ping命令的输入输出一致。

### Lab2 基于ICMP的traceroute程序

与windows的traceroute命令的输入输出一致。

### Lab3 距离矢量(Distance Vector)算法

模拟更新路由表。

伪代码如下：

```
输入: 一个图(节点数，邻接矩阵)
输出: 各个点在各个时刻的路由表

Table[Time][Point]: 路由表

输入图
初始化Table[0][所有点]
int step = 1;
while not converge:
	for 点 in 图
		Table[step][点] = f(Table[step-1][临接的点])
	step += 1;
输出路由表
```

### Lab4 静态路由器软件