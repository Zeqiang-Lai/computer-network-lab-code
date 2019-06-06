# -*- coding: utf-8 -*-

import os,sys,time,subprocess
from scapy.as_resolvers import AS_resolver_radb  
from scapy.all import traceroute
domains='www.baidu.com'
target = domains.split(' ')
dport = [80]
if len(target) >= 1 and target[0] != '':
	# 启动路由跟踪
	res, unans = traceroute(domains, dport=dport, retry=-2)
	# traceroute生成的信息绘制成svg
	#res.graph(target="> graph.svg", ASres=AS_resolver_radb(), type="svg") # ASres=AS_resolver_radb()改变为可用的whois提供商,而非原来的ASres=None后默认的被qiang了的提供商
	time.sleep(1)
	# svg 转格式为 png
	#subprocess.Popen("/usr/local/bin/convert test.svg test.png", shell=True)
else:
    print("IP/domain number of errors, exit")
