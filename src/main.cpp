#include <iostream>
#include <string>

/**
 * OptionPricer - Option Pricing Platform
 * 
 * Main entry point for the option pricing system.
 * Requirements: 7.1 - Layered architecture with separation between pricing engine and web server
 */

int main(int argc, char* argv[]) {
    std::cout << "OptionPricer - Option Pricing Platform" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
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
    
    std::cout << "Configured port: " << port << std::endl;
    std::cout << std::endl;
    std::cout << "Note: HTTP server functionality will be implemented in subsequent tasks." << std::endl;
    std::cout << "This is currently a basic project structure framework." << std::endl;
    
    return 0;
}
