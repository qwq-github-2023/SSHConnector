#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <Winsock2.h>
#include <thread>
#include <conio.h>
extern "C" {
#include "proxysocket.h"
}
#include <libssh2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssh2_static.lib")

using namespace std;

void PauseExit() {
    cout << "即将结束运行..."<< endl;
    ::system("pause");
    exit(1);
}

void Init() {
    SetConsoleTitleA("SSHConnector 1.0");
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        cout << "Socket: 套接字初始化失败！" << endl;
        PauseExit();
    }
    int rc = libssh2_init(0);
    if (rc != 0) {
        cout << "Libssh2: 初始化失败，返回代码: " << rc << endl;
        PauseExit();
    }
    proxysocket_initialize();
}

int main(int argc, char* argv[16])
{
    ::system("chcp 65001");
    Init();
    string ipaddress;
    string port;
    string username;
    string password;
    string proxytype;
    string proxyipaddress;
    string proxyport;
    bool havepw = false;
    proxysocketconfig proxy = proxysocketconfig_create_direct();
    struct sockaddr_in sin;
    unsigned long hostaddr;
    int sock;
    int cmdlen = 0;

    cout << "欢迎使用SSHConnector" << endl;
    cout << "Github: https://github.com/qwq-github-2023/SSHConnector" << endl;
    cout << "SSHConnector 遵循 GPL v3 开源协议" << endl;
    
    /* if (IsDebuggerPresent()) {
        ipaddress = "";
        port = "22";
        username = "root";
        password = "";
        proxytype = "SOCKS5";
        proxyipaddress = "127.0.0.1";
        proxyport = "7890";
        havepw = false;
    } 
    else { */
        if (argc < 4) {
            cout << "您似乎没有把您的服务器信息附带在命令行参数中？请您输入您的服务器信息。" << endl;
            cout << "请输入您的服务器IP地址: ";
            cin >> ipaddress;
            cout << "请输入您的服务器SSH运行端口: ";
            cin >> port;
            cout << "请输入您的服务器用户名: ";
            cin >> username;
            cout << "请输入您的服务器连接密码: ";
            cin >> password;
            cout << "请输入您的代理服务器类型(SOCKS4/SOCKS5/HTTP/无代理请回车): ";
            cin >> proxytype;
            if (proxytype != "") {
                cout << "请输入您的代理服务器IP地址: ";
                cin >> proxyipaddress;
                cout << "请输入您的代理服务器运行端口: ";
                cin >> proxyport;
                cout << "您的代理服务器有密码吗？(Y/N): ";
                string t;
                cin >> t;
                if (t == "Y") {
                    havepw = true;
                    argv[8] = new char[128];
                    argv[9] = new char[128];
                    cout << "请输入您的代理服务器用户名: ";
                    cin >> argv[8];
                    cout << "请输入您的代理服务器密码: ";
                    cin >> argv[9];
                }
                

            }
        }
        else {
            ipaddress = argv[1];
            port = argv[2];
            username = argv[3];
            password = argv[4];
        }
    // }
    if (argc > 4 || proxytype != "") {
        if (argc > 4) {
            proxytype = argv[5];
            proxyipaddress = argv[6];
            proxyport = argv[7];
            if (argc > 9) {
                havepw = true;
            }
        }
        
        if (proxytype == "SOCKS4") {
            proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS4, proxyipaddress.c_str(), stoul(proxyport), havepw ? argv[8] : NULL , havepw ? argv[9] : NULL);
        }
        else if (proxytype == "SOCKS5") {
            proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_SOCKS5, proxyipaddress.c_str(), stoul(proxyport), havepw ? argv[8] : NULL, havepw ? argv[9] : NULL);
        }
        else if (proxytype == "HTTP") {
            proxysocketconfig_add_proxy(proxy, PROXYSOCKET_TYPE_WEB_CONNECT, proxyipaddress.c_str(), stoul(proxyport), havepw ? argv[8] : NULL, havepw ? argv[9] : NULL);
        }
        else {
            cout << "proxysocketconfig: 未知的代理模式！" << endl;
            PauseExit();
        }
        cout << "已为您启用代理。" << endl;
    }
    cout << "正在连接 " << ipaddress << ":" << port << "  ..." << endl;
    if (proxytype == "") {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            cout << "Socket: 创建套接字失败！" << endl;
            PauseExit();
        }

        hostaddr = inet_addr(ipaddress.c_str());
        sin.sin_family = AF_INET;
        sin.sin_port = htons(stoul(port));
        sin.sin_addr.s_addr = hostaddr;


        if (::connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
            cout << "Socket: 套接字连接失败！" << endl;
            PauseExit();
        }
    }
    else {
        char* errmsg = NULL;
        sock = proxysocket_connect(proxy, ipaddress.c_str(), stoul(port), &errmsg);
        if (sock == INVALID_SOCKET) {
            printf("proxysocketconfig: %s\n", (errmsg ? errmsg : "Unknown error"));
            PauseExit();
        }
    }
    

    LIBSSH2_SESSION* session;
    session = libssh2_session_init();
    if (session == NULL) {
        cout << "Libssh2: Session创建失败！" << endl;
        PauseExit();
    }
    libssh2_session_set_blocking(session, 1);

    int rc = libssh2_session_handshake(session, sock);
    if (rc != 0) {
        cout << "Libssh2: 与服务器握手失败！" << endl;
        PauseExit();
    }
    
    rc = libssh2_userauth_password(session, username.c_str(), password.c_str());
    if (rc != 0) {
        cout << "Libssh2: SSH用户验证失败！" << endl;
        PauseExit();
    }

    // 执行远程命令
    LIBSSH2_CHANNEL* channel = libssh2_channel_open_session(session);
    if (channel == NULL) {
        cout << "Libssh2: 创建channel失败！" << endl;
        PauseExit();
    }

    if (libssh2_channel_request_pty(channel, "xterm") != 0) {
        cout << "Libssh2: 请求PTY失败！" << endl;
        PauseExit();
    }
    if (libssh2_channel_shell(channel)) {
        cout << "Libssh2: 请求Shell失败！" << endl;
        PauseExit();
    }
    
    cout << "Libssh2: 成功连接到服务器！" << endl;
    std::thread t([&channel, &cmdlen, ipaddress, port, username] {
        int nbytes;
        string data;

        LIBSSH2_POLLFD* fds = new LIBSSH2_POLLFD;
        fds->type = LIBSSH2_POLLFD_CHANNEL;
        fds->fd.channel = channel;
        fds->events = LIBSSH2_POLLFD_POLLIN | LIBSSH2_POLLFD_POLLOUT;
        char* buffer = new char[64*1024];
        while (true)
        {
            int rc = (libssh2_poll(fds, 1, 10));
            if (rc < 1)
            {
                Sleep(50);
                continue;
            }

            if (fds->revents & LIBSSH2_POLLFD_POLLIN)
            {
                size_t n = libssh2_channel_read(channel, buffer, 64 * 1024);
                buffer[n] = 0;
                if (n <= 0)
                {
                    return;
                }
                else
                {
                    cout << buffer;
                    SetConsoleTitleA(("SSH://" + ipaddress + ":" + port + "@" + username).c_str());
                }
            }
            Sleep(50);
        }
        });
    t.detach();
    char c;
    while (true) {
        c = _getch();
        rc = libssh2_channel_write_ex(channel, 0, &c, 1);
        if (rc == 0) {
            cout << "Libssh2: 进行远程命令失败！" << endl;
            break;
        }
    }
    libssh2_channel_free(channel);
    libssh2_session_disconnect(session, "Goodbye");
    libssh2_session_free(session);
    libssh2_exit();
    ::system("pause");
    return 0;
}


