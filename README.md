#hoskkey

## 功能

  - 调用C的库监听uos系统上的键盘输入
  - 调用C的库给uos系统发送键盘事件
  - 利用VBoxManage发送ctrl+alt+delete组合键至virtualbox指定虚拟机中

## 缺点
  - 需要本地一直开启，一直循环监听
  - 程序需要root权限启动    

## 原理

参考我的博客`http://linkmecry.cn/2021/06/04/go%E6%90%AD%E5%BB%BAhttp%E6%9C%8D%E5%8A%A1%E5%99%A8/`
