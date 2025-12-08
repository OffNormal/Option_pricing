cmake_minimum_required(VERSION 3.15)
project(OptionPricer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O3 -march=native)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-flto)
    endif()
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找依赖项
find_package(nlohmann_json QUIET)
find_package(GTest QUIET)

# 使用FetchContent获取依赖项
include(FetchContent)

# 如果找不到nlohmann_json，使用FetchContent
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# 获取cpp-httplib（header-only HTTP库）
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.3
)
FetchContent_MakeAvailable(httplib)

# 如果找不到GTest，使用FetchContent
if(NOT GTest_FOUND)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    # Create aliases to match find_package
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
    set(GTest_FOUND TRUE)
endif()

# 源文件列表（将在后续任务中添加）
set(OPTION_PRICER_SOURCES
    src/main.cpp
    src/math_utils.cpp
    src/black_scholes_engine.cpp
    src/binomial_tree_engine.cpp
    src/input_validator.cpp
    src/error_logger.cpp
    src/parameter_validator.cpp
    src/visualization_generator.cpp
    src/json_serialization.cpp
    src/option_pricer_server.cpp
)

# 主可执行文件
add_executable(option_pricer ${OPTION_PRICER_SOURCES})

target_link_libraries(option_pricer
    nlohmann_json::nlohmann_json
    httplib::httplib
)

# Windows特定链接库
if(WIN32)
    target_link_libraries(option_pricer ws2_32)
endif()

# 测试配置（如果找到GTest）
if(GTest_FOUND)
    enable_testing()
    
    # 共享源文件（测试需要的实现文件）
    set(SHARED_SOURCES
        src/math_utils.cpp
        src/black_scholes_engine.cpp
        src/binomial_tree_engine.cpp
        src/input_validator.cpp
        src/error_logger.cpp
        src/parameter_validator.cpp
        src/visualization_generator.cpp
        src/json_serialization.cpp
        src/option_pricer_server.cpp
    )
    
    # 测试源文件（将在后续任务中添加）
    set(TEST_SOURCES
        tests/test_core_structures.cpp
        tests/test_math_utils.cpp
        tests/test_black_scholes.cpp
        tests/test_greeks_basic.cpp
        tests/test_binomial_tree.cpp
        tests/test_binomial_greeks.cpp
        tests/test_input_validator.cpp
        tests/test_error_handling.cpp
        tests/test_visualization_generator.cpp
        tests/test_json_serialization.cpp
        tests/test_timeout_control.cpp
        tests/test_performance.cpp
        # tests/test_properties_pricing.cpp
        # tests/test_properties_greeks.cpp
    )
    
    if(TEST_SOURCES)
        add_executable(option_pricer_tests ${TEST_SOURCES} ${SHARED_SOURCES})
        
        target_link_libraries(option_pricer_tests
            GTest::GTest
            GTest::Main
            nlohmann_json::nlohmann_json
            httplib::httplib
        )
        
        # 添加测试
        add_test(NAME option_pricer_tests COMMAND option_pricer_tests)
    endif()
endif()

# 安装规则
install(TARGETS option_pricer DESTINATION bin)
