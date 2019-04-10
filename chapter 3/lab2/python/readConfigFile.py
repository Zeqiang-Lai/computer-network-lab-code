import string

def readConfigFile(cfgFilePath, key, value):
    
    #打开文件
    f = open (cfgFilePath,'r')

    #检测是否打开
    


    #循环读取每一行
    for line in f.readlines():
        
        #找到每行“=”位置，之前是key，之后是value
        pos = line.index('=')
        
        #如果没有找到，返回null
        #tmpkey的值是取等号之前的值
        tmpKey = line[0:pos]
        
        #如果输入和读出的一样
        if key == tmpKey:
            value = line[pos+1:]
            return value



