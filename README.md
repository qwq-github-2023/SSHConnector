# SSHConnector
## 项目简介
### 声明
本项目遵循 GPL v3 开源协议
这是一个由C++开发的SSH客户端项目
在此项目中，使用了[proxysocket](https://github.com/brechtsanders/proxysocket/releases/tag/0.1.11)开源项目的代码
### 特点
+ 静态编译后文件大小仅350kb，极其简洁
+ 支持使用SOCKS4/SOCKS5/HTTP代理服务器
+ 使用UTF-8编码，防止乱码问题
+ 使用pty xterm，支持使用top等指令
### 作者
由 qwq(https://space.bilibili.com/414876797)) 编写
## 用法
### 命令行启动
**用"\(\)"括号的参数意为必要参数，用"\[\]"括号的参数意为可选参数**<br>
SSHConnector (SSH目标IP地址) (SSH目标端口) (SSH目标用户名) (SSH目标密码) \[代理类型，可填写SOCKS4/SOCKS5/HTTP\] \[代理服务器IP地址\] \[代理服务器端口\] \[代理服务器用户名\] \[代理服务器密码\]<br>
例子:<br>
*SSHConnector 0.0.0.0 22 root mypassword*<br>
*SSHConnector 0.0.0.0 22 root mypassword SOCKS5 0.0.0.0 1234*<br>
*SSHConnector 0.0.0.0 22 root mypassword HTTP 0.0.0.0 1234 qwq proxypassword*<br>
### 直接启动
直接启动exe，跟随提示输入信息即可
## 未来
未来将会添加SFTP功能，有相关意见可在Issues反馈
