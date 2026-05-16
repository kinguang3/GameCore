# GameCore - 简单游戏服务器

基于 Windows Socket 的多人游戏服务器 Demo，支持多客户端连接和消息收发。

## 功能特性

- TCP 连接监听，支持多玩家同时在线
- 多线程处理客户端消息
- 简单的请求/响应机制

## 技术栈

- C++11
- Windows Socket (Winsock)
- 标准库：thread, vector, string

## 编译运行

### Windows (Visual Studio)

1. 创建 C++ 控制台项目
2. 添加 `server.cpp` 到项目
3. 配置链接器：`ws2_32.lib`
4. 编译运行

### Windows (MinGW/g++)

```bash
g++ server.cpp -o server.exe -lws2_32
server.exe
```

服务器默认监听端口 **9001**。

## 后续扩展

- 玩家登录/注册系统
- 房间系统
- 消息广播
- 心跳检测
- 自定义协议 (JSON)
- 数据库存储
- 日志系统

## 目录结构

```
GameCore/
├── server.cpp    # 服务器源码
├── server.exe    # 编译后的可执行文件
└── README.md     # 项目说明
```