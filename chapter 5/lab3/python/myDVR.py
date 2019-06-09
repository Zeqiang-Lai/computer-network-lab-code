from tkinter import *
from tkinter import filedialog
from tkinter import messagebox
import threading,time,copy,os


#全局变量
lock = threading.Lock() #进程锁
tables = []             #网络上的路由表的集合
tables_new = []          #更新后的路由表的集合
neibortuple_cost={}     #到邻居的耗费
me_neibor={}            #记录路由器和它的邻居
                        #通过这两个表的交换赋值来更新
luyou_wrong = '--'      #故障的表的名字（默认一次只能故障一个路由）
#每次更新控件显示的字符串
str_send = ''           #发送了自己路由表的信息
str_update = ''         #更新了自己路由表的信息
count = 0
def time_now():#用于获得当前时间
    return str(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()))
#data[0-3]分别表示路由器名、目标路由器、距离、下一跳
#tables[i][0-1]:第i个路由器的名称、表项集
def main():#主函数
    def add(data,tables):#由用户添加每个路由器的初始路由表
        if tables:
            flag = False
            for i in range(len(tables)):
                if data[0] == tables[i][0]:
                    if [data[1],data[2],data[3]] in tables[i][1]:
                        string = '路由器%s中已有该表项（目标地址：%s,距离：%d，下一跳：%s）' % (data[0],data[1],data[2],data[3])
                    else:
                        tables[i][1].append([data[1],data[2],data[3]])
                        string = '向路由器%s添加了表项（目标地址：%s,距离：%d，下一跳：%s）' % (data[0],data[1],data[2],data[3])
                    flag = True
                    break
            if not flag:
                tables.append([data[0],[[data[1],data[2],data[3]]]])
                string = '添加了%s路由器\n向%s路由器添加了表项（目标地址：%s,距离：%d，下一跳：%s）' % (data[0],data[0],data[1],data[2],data[3])
        else:
            tables.append([data[0],[[data[1],data[2],data[3]]]])
            string = '添加了%s路由器\n向路由器%s添加了表项（目标地址：%s,距离：%d，下一跳：%s）' % (data[0],data[0],data[1],data[2],data[3])
        print(string)
        return tables

    def send(table):#向相邻网络发送自己的路由表
        string = table[0] + '向相邻路由发送了自己的路由表 '
        global str_send
        str_send += time_now() + '\n' + string + '\n'


    def update(table, tables, tables_new):#更新自己的路由表
        table_n = copy.deepcopy(table)
        for temptable in tables:
            if temptable[0] in me_neibor[table_n[0]]:
                for neitem in temptable[1]:#对邻居路由表的每个项
                    flag=False
                    for myitem in table_n[1]:#如果该项的目的路由器与自己某项目的路由相同
                        if myitem[0]==neitem[0]:
                            flag=True
                            if neitem[1]+neibortuple_cost[(table_n[0],temptable[0])]<myitem[1]:
                                myitem[1]=neitem[1]+neibortuple_cost[(table_n[0],temptable[0])]
                                myitem[2]=temptable[0]
                            break
                    if flag==False and table_n[0] != neitem[0]:#如果遍历了自己的路由表，没有和neitem的目的路由相同的，则需要加入该项
                        tempitem = copy.deepcopy(neitem)
                        tempitem[1] = neitem[1] + neibortuple_cost[(table_n[0], temptable[0])]
                        tempitem[2] = temptable[0]
                        table_n[1].append(tempitem)

        lock.acquire()
        tables_new.append(table_n)
        lock.release()
    def threads(tables, tables_new):#多线程发送和更新，做到每一个路由器同时发送和更新(设置接收信息的时间为1s)
        global str_send
        global str_update
        str_send = ''
        str_update = ''
        threadpool_1 = []
        for each in tables:
            th = threading.Thread(target= send, args= (each,))
            threadpool_1.append(th)
        for th in threadpool_1:
            th.start()
        for th in threadpool_1:
            threading.Thread.join(th)
        time.sleep(1)
        threadpool_2 = []
        for each in tables:
            th = threading.Thread(target= update, args= (each, tables, tables_new))
            threadpool_2.append(th)
        for th in threadpool_2:
            th.start()
        for th in threadpool_2:
            threading.Thread.join(th)
        return tables_new

    def show_table(tables):
        global count
        str="------------------路由表第%d次更新------------------"%(count)
        print(str)
        count=count+1
        for table in tables:
            print("路由器%s: "%(table[0]))
            print("目的 花费 下一跳")
            for item in table[1]:
                print(" %s    %d    %s"%(item[0],item[1],item[2]))
        #控件的callback函数
    def add_callback(tables):#添加按钮
        for line in open('DVinit.ini', 'r'):
            line = line.replace('\n', '').split(' ')
            name = line[0]
            net_tar = line[1]
            distance=eval(line[2])
            next_ = line[3]
            if name != '' and net_tar != '' and distance != '':
                if distance >= 1 and distance <= 16:
                    data = [name, net_tar, distance, next_]
                    tables = add(data, tables)
                else:
                    messagebox.showinfo('警告', '距离应当为1-16的整数!')
            else:
                messagebox.showinfo('警告', '路由器名称,目标网络和距离不能为空！')

    def init_dist(tables):
        for table in tables:
            me_neibor[table[0]]=[]
            for item in table[1]:
                neibortuple_cost[(table[0], item[0])]=item[1]
                me_neibor[table[0]].append(item[0])
    def update_callback(tables, tables_new):#更新按钮
        tables_new.clear()
        tables_n = threads(tables, tables_new)
        tables.clear()
        tables.extend(tables_n)
        show_table(tables_n)
        return tables_n
    add_callback(tables)
    show_table(tables)
    init_dist(tables)
    for i in range(0,2):
        update_callback(tables, tables_new)
if __name__=='__main__':
    main()