
#include <iostream>    // 标准输入输出流
#include <algorithm>
#include <string>      // 字符串处理
#include <thread>      // 多线程支持
#include <vector>      // 动态数组容器
#include <ws2tcpip.h>  // Windows Socket 扩展
using namespace std;


#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库,g++手动连接库,如果使用Visual Studio则不需要此行


// 游戏服务器类，负责管理客户端连接和消息收发
class GameServer {
private://只能在类内部访问
    SOCKET listenSocket;              // 监听客户端连接的 socket
    std::vector<SOCKET> clients;      // 已连接的客户端 socket 列表
    bool running;                     // 服务器运行状态标志

public://可以在类内部和外部访问
    // 构造函数，初始化服务器并开始监听指定端口
    /**
     * @brief 构造函数，初始化服务器并开始监听指定端口
     * @param port 监听端口号
     * 主要API：
     *   - WSAStartup (ws2tcpip.h)：初始化Winsock库
     *   - socket (ws2tcpip.h)：创建socket
     *   - bind (ws2tcpip.h)：绑定socket到端口
     *   - listen (ws2tcpip.h)：监听端口
     */
    GameServer(int port) : running(true) {//port参数指定监听端口
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData); // 初始化网络库（ws2tcpip.h）

        listenSocket = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP socket（ws2tcpip.h）

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;              // IPv4地址族（ws2tcpip.h）
        serverAddr.sin_port = htons(port);            // 端口转网络字节序（ws2tcpip.h）
        serverAddr.sin_addr.s_addr = INADDR_ANY;      // 监听所有IP（ws2tcpip.h）

        bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)); // 绑定端口（ws2tcpip.h）

        listen(listenSocket, SOMAXCONN); // 开始监听（ws2tcpip.h）
        std::cout << "Server started, listening on port " << port << std::endl; // 输出信息（iostream）
    }

    // 析构函数，关闭所有 socket 并清理资源
    /**
     * @brief 析构函数，关闭所有socket并清理资源
     * 主要API：
     *   - closesocket (ws2tcpip.h)：关闭socket
     *   - WSACleanup (ws2tcpip.h)：清理Winsock库
     */
    ~GameServer() {
        for (SOCKET client : clients) {
            closesocket(client); // 关闭每个客户端socket（ws2tcpip.h）
        }
        closesocket(listenSocket); // 关闭监听socket（ws2tcpip.h）
        WSACleanup();             // 清理Winsock库（ws2tcpip.h）
    }

    // 服务器主循环，持续接受客户端连接
    /**
     * @brief 服务器主循环，持续接受客户端连接
     * 主要API：
     *   - accept (ws2tcpip.h)：接受客户端连接
     *   - std::thread (thread)：创建线程
     *   - std::cout (iostream)：输出信息
     */
    void run() {
        while (running) {
            sockaddr_in clientAddr;
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &addrLen); // 等待客户端连接（ws2tcpip.h）

            if (clientSocket != INVALID_SOCKET) {
                int clientId = clients.size() + 1; // 分配客户端编号
                clients.push_back(clientSocket);    // 添加到客户端列表
                std::cout << "玩家 " << clientId << " 已连接" << std::endl; // 输出信息（iostream）

                std::thread(&GameServer::handleClient, this, clientSocket, clientId).detach(); // 新线程处理（thread）
            }
        }
    }

    // 处理单个客户端的消息收发
    /**
     * @brief 处理单个客户端的消息收发
     * @param clientSocket 客户端socket
     * @param clientId 客户端编号
     * 主要API：
     *   - recv (ws2tcpip.h)：接收数据
     *   - send (ws2tcpip.h)：发送数据
     *   - std::string (string)：字符串处理
     *   - std::cout (iostream)：输出信息
     *   - std::find (algorithm)：查找元素
     *   - closesocket (ws2tcpip.h)：关闭socket
     */
    void handleClient(SOCKET clientSocket, int clientId) {
        char buffer[1024]; // 接收缓冲区
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // 从 clientSocket 接收数据，最多接收 1023 字节，留 1 字节给 '\0' 结尾符
            if (bytesReceived <= 0) {
                std::cout << "玩家 " << clientId << " 断开连接" << std::endl; // 输出信息（iostream）
                break;
            }

            buffer[bytesReceived] = '\0'; // 字符串结束符（C标准库）
            std::string message(buffer);   // 转为std::string（string）,方便处理,把C风格字符串转换为C++字符串
            std::cout << "收到玩家 " << clientId << " 消息: " << message << std::endl; // 输出信息（iostream）

            std::string response = "服务器收到: " + message;
            send(clientSocket, response.c_str(), response.length(), 0); // 发送数据（ws2tcpip.h）
        }

        // 客户端断开后，从列表中移除并关闭socket
        auto it = std::find(clients.begin(), clients.end(), clientSocket); // 查找元素（algorithm）
        if (it != clients.end()) clients.erase(it);
        closesocket(clientSocket); // 关闭socket（ws2tcpip.h）
    }
};//这是GameServer类的定义，包含了服务器的核心功能，如监听端口、接受连接、处理消息等


// 程序入口，创建并运行游戏服务器
/**
 * @brief 程序入口，创建并运行游戏服务器
 * 主要API：
 *   - GameServer（自定义类）
 *   - server.run（自定义方法）
 */
int main() {
    GameServer server(9001); // 实例化服务器，监听9001端口
    server.run();            // 启动服务器主循环
    return 0;
}