#include "option_pricer_server.h"
#include "httplib.h"
#include "black_scholes_engine.h"
#include "binomial_tree_engine.h"
#include "input_validator.h"
#include "json_serialization.h"
#include "visualization_generator.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <future>
#include <thread>

namespace option_pricer {

namespace {
    // 超时时间常量（5秒）
    constexpr std::chrono::seconds CALCULATION_TIMEOUT{5};
    
    /**
     * 使用超时控制执行定价计算
     * 
     * @param engine 定价引擎
     * @param params 期权参数
     * @return 定价结果（如果超时，返回失败结果）
     */
    PricingResult calculate_with_timeout(IPricingEngine& engine, const OptionParams& params) {
        // 使用std::async异步执行计算
        std::future<PricingResult> future = std::async(
            std::launch::async,
            [&engine, &params]() {
                return engine.calculate(params);
            }
        );
        
        // 等待结果，设置超时
        if (future.wait_for(CALCULATION_TIMEOUT) == std::future_status::timeout) {
            // 超时：返回错误结果
            PricingResult timeout_result;
            timeout_result.success = false;
            timeout_result.error_message = "Calculation timeout: computation exceeded 5 seconds";
            timeout_result.warnings.push_back("Consider using fewer binomial tree steps or simpler parameters");
            timeout_result.option_price = 0.0;
            return timeout_result;
        }
        
        // 获取计算结果
        return future.get();
    }
} // anonymous namespace

OptionPricerServer::OptionPricerServer(int port) 
    : port_(port), server_(std::make_unique<httplib::Server>()) {
    setup_routes();
}

OptionPricerServer::~OptionPricerServer() {
    stop();
}

void OptionPricerServer::setup_routes() {
    // POST /api/price - 计算期权价格
    server_->Post("/api/price", [this](const httplib::Request& req, httplib::Response& res) {
        handle_price_request(req, res);
    });
    
    // POST /api/visualize - 生成可视化数据
    server_->Post("/api/visualize", [this](const httplib::Request& req, httplib::Response& res) {
        handle_visualize_request(req, res);
    });
    
    // OPTIONS请求处理（CORS预检）
    server_->Options("/api/price", [this](const httplib::Request&, httplib::Response& res) {
        add_cors_headers(res);
        res.status = 200;
    });
    
    server_->Options("/api/visualize", [this](const httplib::Request&, httplib::Response& res) {
        add_cors_headers(res);
        res.status = 200;
    });
    
    // 根路径重定向到主页 (Requirement 4.1)
    server_->Get("/", [this](const httplib::Request&, httplib::Response& res) {
        serve_static_file(res, "web/index.html", "text/html");
    });
    
    // 通用静态文件服务 - 自动检测MIME类型
    // 支持所有web目录下的文件
    server_->Get(R"(/(.+))", [this](const httplib::Request& req, httplib::Response& res) {
        std::string path = req.matches[1];
        std::string file_path = "web/" + path;
        std::string mime_type = get_mime_type(file_path);
        serve_static_file(res, file_path, mime_type);
    });
    
    // API信息端点
    server_->Get("/api", [this](const httplib::Request&, httplib::Response& res) {
        nlohmann::json info = {
            {"service", "OptionPricer API"},
            {"version", "1.0"},
            {"endpoints", {
                {
                    {"path", "/api/price"},
                    {"method", "POST"},
                    {"description", "Calculate option price and Greeks"}
                },
                {
                    {"path", "/api/visualize"},
                    {"method", "POST"},
                    {"description", "Generate visualization data for option pricing"}
                }
            }}
        };
        set_json_response(res, info);
    });
}

void OptionPricerServer::handle_price_request(const httplib::Request& req, httplib::Response& res) {
    try {
        // 解析JSON请求
        nlohmann::json request_json = nlohmann::json::parse(req.body);
        
        // 提取参数
        std::string model_type = request_json.value("model", "black-scholes");
        int binomial_steps = request_json.value("binomial_steps", 100);
        
        // 反序列化期权参数
        OptionParams params = JsonSerializer::deserialize_option_params(request_json);
        
        // 验证输入
        auto validation_result = InputValidator::validate(params);
        if (!validation_result.is_valid) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid input parameters",
                validation_result.errors
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        // 如果是二叉树模型，验证步数
        if (model_type == "binomial-tree" || model_type == "binomial_tree") {
            auto steps_validation = InputValidator::validate_binomial_steps(binomial_steps);
            if (!steps_validation.is_valid) {
                nlohmann::json error_response = JsonSerializer::serialize_error_response(
                    "Invalid binomial tree steps",
                    steps_validation.errors
                );
                set_json_response(res, error_response, 400);
                return;
            }
        }
        
        // 创建定价引擎
        auto engine = create_engine(model_type, binomial_steps);
        if (!engine) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid model type",
                {"Supported models: black-scholes, binomial-tree"}
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        // 计算期权价格（记录时间，使用超时控制）
        auto start_time = std::chrono::high_resolution_clock::now();
        PricingResult result = calculate_with_timeout(*engine, params);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        double computation_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time
        ).count();
        
        // 检查计算是否成功
        if (!result.success) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                result.error_message,
                result.warnings
            );
            set_json_response(res, error_response, 500);
            return;
        }
        
        // 序列化结果
        nlohmann::json response = JsonSerializer::serialize_pricing_result(
            result,
            engine->get_model_name(),
            computation_time_ms
        );
        
        set_json_response(res, response);
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json error_response = JsonSerializer::serialize_error_response(
            "JSON parsing error",
            {e.what()}
        );
        set_json_response(res, error_response, 400);
    } catch (const std::exception& e) {
        nlohmann::json error_response = JsonSerializer::serialize_error_response(
            "Internal server error",
            {e.what()}
        );
        set_json_response(res, error_response, 500);
    }
}

void OptionPricerServer::handle_visualize_request(const httplib::Request& req, httplib::Response& res) {
    try {
        // 解析JSON请求
        nlohmann::json request_json = nlohmann::json::parse(req.body);
        
        // 提取参数
        std::string model_type = request_json.value("model", "black-scholes");
        int binomial_steps = request_json.value("binomial_steps", 100);
        
        // 可视化参数
        double spot_min = request_json.value("spot_min", 0.0);
        double spot_max = request_json.value("spot_max", 0.0);
        int points = request_json.value("points", 50);
        
        // 反序列化期权参数
        OptionParams params = JsonSerializer::deserialize_option_params(request_json);
        
        // 如果未指定范围，使用默认范围（标的资产价格的50%-150%）
        if (spot_min == 0.0) {
            spot_min = params.spot_price * 0.5;
        }
        if (spot_max == 0.0) {
            spot_max = params.spot_price * 1.5;
        }
        
        // 验证输入
        auto validation_result = InputValidator::validate(params);
        if (!validation_result.is_valid) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid input parameters",
                validation_result.errors
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        // 验证可视化参数
        if (spot_min <= 0 || spot_max <= 0 || spot_min >= spot_max) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid visualization range",
                {"spot_min must be positive and less than spot_max"}
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        if (points < 10 || points > 200) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid number of points",
                {"points must be between 10 and 200"}
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        // 创建定价引擎
        auto engine = create_engine(model_type, binomial_steps);
        if (!engine) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Invalid model type",
                {"Supported models: black-scholes, binomial-tree"}
            );
            set_json_response(res, error_response, 400);
            return;
        }
        
        // 生成可视化数据（记录时间，使用超时控制）
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // 使用异步执行可视化生成，设置超时
        std::future<VisualizationGenerator::ChartData> price_future = std::async(
            std::launch::async,
            [&params, &engine, spot_min, spot_max, points]() {
                return VisualizationGenerator::generate_price_curve(
                    params, *engine, spot_min, spot_max, points
                );
            }
        );
        
        // 等待价格曲线生成，检查超时
        if (price_future.wait_for(CALCULATION_TIMEOUT) == std::future_status::timeout) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Visualization timeout: price curve generation exceeded 5 seconds",
                {"Consider using fewer data points or simpler parameters"}
            );
            set_json_response(res, error_response, 500);
            return;
        }
        
        auto price_curve = price_future.get();
        
        // 生成希腊值曲线，使用异步执行
        std::future<std::vector<VisualizationGenerator::ChartData>> greeks_future = std::async(
            std::launch::async,
            [&params, &engine, spot_min, spot_max, points]() {
                return VisualizationGenerator::generate_greeks_curves(
                    params, *engine, spot_min, spot_max, points
                );
            }
        );
        
        // 等待希腊值曲线生成，检查超时
        if (greeks_future.wait_for(CALCULATION_TIMEOUT) == std::future_status::timeout) {
            nlohmann::json error_response = JsonSerializer::serialize_error_response(
                "Visualization timeout: Greeks curves generation exceeded 5 seconds",
                {"Consider using fewer data points or simpler parameters"}
            );
            set_json_response(res, error_response, 500);
            return;
        }
        
        auto greeks_curves = greeks_future.get();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        
        double computation_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time
        ).count();
        
        // 序列化可视化数据
        nlohmann::json visualization_data = JsonSerializer::serialize_visualization_data(
            price_curve, greeks_curves
        );
        
        // 构建响应
        nlohmann::json response = {
            {"success", true},
            {"model", engine->get_model_name()},
            {"visualization_data", visualization_data},
            {"computation_time_ms", computation_time_ms}
        };
        
        set_json_response(res, response);
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json error_response = JsonSerializer::serialize_error_response(
            "JSON parsing error",
            {e.what()}
        );
        set_json_response(res, error_response, 400);
    } catch (const std::exception& e) {
        nlohmann::json error_response = JsonSerializer::serialize_error_response(
            "Internal server error",
            {e.what()}
        );
        set_json_response(res, error_response, 500);
    }
}

std::unique_ptr<IPricingEngine> OptionPricerServer::create_engine(
    const std::string& model_type, 
    int binomial_steps) {
    
    if (model_type == "black-scholes" || model_type == "black_scholes") {
        return std::make_unique<BlackScholesEngine>();
    } else if (model_type == "binomial-tree" || model_type == "binomial_tree") {
        return std::make_unique<BinomialTreeEngine>(binomial_steps);
    }
    
    return nullptr;
}

void OptionPricerServer::add_cors_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void OptionPricerServer::set_json_response(
    httplib::Response& res, 
    const nlohmann::json& json, 
    int status_code) {
    
    res.set_content(json.dump(2), "application/json");
    res.status = status_code;
    add_cors_headers(res);
}

std::string OptionPricerServer::get_mime_type(const std::string& file_path) {
    // 查找文件扩展名
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    
    std::string extension = file_path.substr(dot_pos);
    
    // 根据扩展名返回正确的MIME类型
    if (extension == ".html" || extension == ".htm") {
        return "text/html";
    } else if (extension == ".css") {
        return "text/css";
    } else if (extension == ".js") {
        return "application/javascript";
    } else if (extension == ".json") {
        return "application/json";
    } else if (extension == ".png") {
        return "image/png";
    } else if (extension == ".jpg" || extension == ".jpeg") {
        return "image/jpeg";
    } else if (extension == ".gif") {
        return "image/gif";
    } else if (extension == ".svg") {
        return "image/svg+xml";
    } else if (extension == ".ico") {
        return "image/x-icon";
    } else if (extension == ".txt") {
        return "text/plain";
    } else if (extension == ".md") {
        return "text/markdown";
    } else if (extension == ".xml") {
        return "application/xml";
    } else if (extension == ".pdf") {
        return "application/pdf";
    } else if (extension == ".woff") {
        return "font/woff";
    } else if (extension == ".woff2") {
        return "font/woff2";
    } else if (extension == ".ttf") {
        return "font/ttf";
    } else if (extension == ".eot") {
        return "application/vnd.ms-fontobject";
    } else {
        return "application/octet-stream";
    }
}

void OptionPricerServer::serve_static_file(
    httplib::Response& res,
    const std::string& file_path,
    const std::string& content_type) {
    
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        res.status = 404;
        res.set_content("<!DOCTYPE html><html><head><title>404 Not Found</title></head>"
                       "<body><h1>404 - File Not Found</h1><p>The requested file was not found.</p></body></html>", 
                       "text/html");
        return;
    }
    
    // 读取文件内容
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 设置响应内容和MIME类型
    res.set_content(content, content_type.c_str());
    res.status = 200;
    
    // 添加缓存控制头部（静态资源可以缓存）
    res.set_header("Cache-Control", "public, max-age=3600");
}

void OptionPricerServer::start() {
    std::cout << "Starting OptionPricer HTTP server on port " << port_ << "..." << std::endl;
    std::cout << "Web interface: http://localhost:" << port_ << "/" << std::endl;
    std::cout << "API endpoints:" << std::endl;
    std::cout << "  POST http://localhost:" << port_ << "/api/price" << std::endl;
    std::cout << "  POST http://localhost:" << port_ << "/api/visualize" << std::endl;
    std::cout << std::endl;
    
    if (!server_->listen("0.0.0.0", port_)) {
        throw std::runtime_error("Failed to start server on port " + std::to_string(port_));
    }
}

void OptionPricerServer::stop() {
    if (server_) {
        server_->stop();
    }
}

} // namespace option_pricer
