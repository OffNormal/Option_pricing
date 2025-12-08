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

# 如果找不到nlohmann_json，使用FetchContent
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# 源文件列表（将在后续任务中添加）
set(OPTION_PRICER_SOURCES
    src/main.cpp
)

# 主可执行文件
add_executable(option_pricer ${OPTION_PRICER_SOURCES})

target_link_libraries(option_pricer
    nlohmann_json::nlohmann_json
)

# Windows特定链接库
if(WIN32)
    target_link_libraries(option_pricer ws2_32)
endif()

# 测试配置（如果找到GTest）
if(GTest_FOUND)
    enable_testing()
    
    # 测试源文件（将在后续任务中添加）
    set(TEST_SOURCES
        tests/test_core_structures.cpp
        # tests/test_black_scholes.cpp
        # tests/test_binomial_tree.cpp
        # tests/test_properties_pricing.cpp
        # tests/test_properties_greeks.cpp
        # tests/test_input_validator.cpp
    )
    
    if(TEST_SOURCES)
        add_executable(option_pricer_tests ${TEST_SOURCES})
        
        target_link_libraries(option_pricer_tests
            GTest::GTest
            GTest::Main
            nlohmann_json::nlohmann_json
        )
        
        # 添加测试
        add_test(NAME option_pricer_tests COMMAND option_pricer_tests)
    endif()
endif()

# 安装规则
install(TARGETS option_pricer DESTINATION bin)
