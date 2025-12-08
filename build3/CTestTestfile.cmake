# CMake generated Testfile for 
# Source directory: C:/Users/xhs20/Desktop/code/c++/Option_pricing
# Build directory: C:/Users/xhs20/Desktop/code/c++/Option_pricing/build3
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(option_pricer_tests "C:/Users/xhs20/Desktop/code/c++/Option_pricing/build3/Debug/option_pricer_tests.exe")
  set_tests_properties(option_pricer_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;103;add_test;C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(option_pricer_tests "C:/Users/xhs20/Desktop/code/c++/Option_pricing/build3/Release/option_pricer_tests.exe")
  set_tests_properties(option_pricer_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;103;add_test;C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(option_pricer_tests "C:/Users/xhs20/Desktop/code/c++/Option_pricing/build3/MinSizeRel/option_pricer_tests.exe")
  set_tests_properties(option_pricer_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;103;add_test;C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(option_pricer_tests "C:/Users/xhs20/Desktop/code/c++/Option_pricing/build3/RelWithDebInfo/option_pricer_tests.exe")
  set_tests_properties(option_pricer_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;103;add_test;C:/Users/xhs20/Desktop/code/c++/Option_pricing/CMakeLists.txt;0;")
else()
  add_test(option_pricer_tests NOT_AVAILABLE)
endif()
subdirs("_deps/nlohmann_json-build")
subdirs("_deps/googletest-build")
