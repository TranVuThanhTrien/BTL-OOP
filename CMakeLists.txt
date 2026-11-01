cmake_minimum_required(VERSION 3.10)
project(DT08 CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include third_party)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra)   # cảnh báo là lỗi cần sửa, không commit code còn warning
endif()
file(GLOB LIB_SRC src/*.cpp)
list(REMOVE_ITEM LIB_SRC ${CMAKE_SOURCE_DIR}/src/main.cpp ${CMAKE_SOURCE_DIR}/src/main_test.cpp)
add_executable(dt08 src/main.cpp ${LIB_SRC})
add_executable(dt08_test src/main_test.cpp ${LIB_SRC})
find_package(Threads REQUIRED)
target_link_libraries(dt08 Threads::Threads)

# Người phụ trách: D (server), C (test)
# TODO(D-5): Windows cần thêm ws2_32 vào target_link_libraries khi dùng cpp-httplib
# TODO(C-6): thêm -fsanitize=address cho dt08_test (target_compile_options và target_link_options)
