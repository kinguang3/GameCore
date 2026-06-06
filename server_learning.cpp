#include <iostream>     // 输入输出流
#include "D:/GameCore/ThirdParty/httplib.h" // HTTP服务器库（假设路径正确）

int main() {
    httplib::Server server; // 创建HTTP服务器对象

    // 定义一个简单的GET请求处理器
    server.Get("/hello", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello, World!", "text/plain"); // 设置响应内容和类型
    });

    std::cout << "HTTP server is running on http://localhost:8080" << std::endl; // 输出服务器地址
    server.listen("localhost", 8080); // 启动服务器，监听本地8080端口

    return 0;
}