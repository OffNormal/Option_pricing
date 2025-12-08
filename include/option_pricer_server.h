#ifndef OPTION_PRICER_SERVER_H
#define OPTION_PRICER_SERVER_H

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "pricing_engine.h"

// Forward declaration for httplib
namespace httplib {
    class Server;
    struct Request;
    struct Response;
}

namespace option_pricer {

/**
 * HTTP服务器类
 * 
 * 提供RESTful API用于期权定价计算
 * 支持CORS和JSON格式的请求/响应
 */
class OptionPricerServer {
public:
    /**
     * 构造函数
     * @param port 服务器监听端口（默认8080）
     */
    explicit OptionPricerServer(int port = 8080);
    
    /**
     * 析构函数
     */
    ~OptionPricerServer();
    
    /**
     * 启动HTTP服务器
     * 阻塞调用，直到服务器停止
     */
    void start();
    
    /**
     * 停止HTTP服务器
     */
    void stop();
    
    /**
     * 获取服务器端口
     * @return 端口号
     */
    int get_port() const { return port_; }
    
private:
    int port_;
    std::unique_ptr<httplib::Server> server_;
    
    /**
     * 设置HTTP路由
     * 配置所有API端点
     */
    void setup_routes();
    
    /**
     * 处理价格计算请求
     * POST /api/price
     * 
     * @param req HTTP请求对象
     * @param res HTTP响应对象
     */
    void handle_price_request(const httplib::Request& req, httplib::Response& res);
    
    /**
     * 处理可视化数据请求
     * POST /api/visualize
     * 
     * @param req HTTP请求对象
     * @param res HTTP响应对象
     */
    void handle_visualize_request(const httplib::Request& req, httplib::Response& res);
    
    /**
     * 定价引擎工厂函数
     * 根据模型类型创建相应的定价引擎
     * 
     * @param model_type 模型类型字符串（"black-scholes" 或 "binomial-tree"）
     * @param binomial_steps 二叉树步数（仅用于二叉树模型）
     * @return 定价引擎智能指针
     */
    std::unique_ptr<IPricingEngine> create_engine(const std::string& model_type, 
                                                   int binomial_steps = 100);
    
    /**
     * 添加CORS头部
     * 允许跨域请求
     * 
     * @param res HTTP响应对象
     */
    void add_cors_headers(httplib::Response& res);
    
    /**
     * 设置JSON响应
     * 设置Content-Type和CORS头部
     * 
     * @param res HTTP响应对象
     * @param json JSON响应数据
     * @param status_code HTTP状态码（默认200）
     */
    void set_json_response(httplib::Response& res, 
                          const nlohmann::json& json, 
                          int status_code = 200);
    
    /**
     * 提供静态文件服务
     * 
     * @param res HTTP响应对象
     * @param file_path 文件路径
     * @param content_type MIME类型
     */
    void serve_static_file(httplib::Response& res,
                          const std::string& file_path,
                          const std::string& content_type);
    
    /**
     * 根据文件扩展名获取MIME类型
     * 
     * @param file_path 文件路径
     * @return MIME类型字符串
     */
    std::string get_mime_type(const std::string& file_path);
};

} // namespace option_pricer

#endif // OPTION_PRICER_SERVER_H
