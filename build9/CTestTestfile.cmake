# CMake generated Testfile for 
# Source directory: C:/Users/xhs20/Desktop/code/c++/Option_pricing
# Build directory: C:/Users/xhs20/Desktop/code/c++/Option_pricing/build9
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(option_pricer_tests "C:/Users/xhs20/Desktop/code/c++/Option_pricing/build9/option_pricer_tests.exe")
set_tests_properties(option_pricer_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;134;add_test;C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;0;")
subdirs("_deps/nlohmann_json-build")
subdirs("_deps/httplib-build")
subdirs("_deps/googletest-build")
