#include <iostream>
#include <string>
#include <csignal>
#include "option_pricer_server.h"

/**
 * OptionPricer - Option Pricing Platform
 * 
 * Main entry point for the option pricing system.
 * Requirements: 7.1 - Layered architecture with separation between pricing engine and web server
 */

// Global server pointer for signal handling
option_pricer::OptionPricerServer* g_server = nullptr;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutting down server..." << std::endl;
        if (g_server) {
            g_server->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "OptionPricer - Option Pricing Platform" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << std::endl;
    
    // Default port
    int port = 8080;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            try {
                port = std::stoi(argv[i + 1]);
                ++i;
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid port number" << std::endl;
                return 1;
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --port <port>   Specify HTTP server port (default: 8080)" << std::endl;
            std::cout << "  --help, -h      Show this help message" << std::endl;
            return 0;
        }
    }
    
    try {
        // Create and start HTTP server
        option_pricer::OptionPricerServer server(port);
        g_server = &server;
        
        // Set up signal handlers for graceful shutdown
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        // Start server (blocking call)
        server.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Server stopped." << std::endl;
    return 0;
}
