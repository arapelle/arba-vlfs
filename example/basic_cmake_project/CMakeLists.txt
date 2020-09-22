cmake_minimum_required(VERSION 3.18)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
# Find package vlfs:
find_package(vlfs 0.1.0 CONFIG REQUIRED)
# Use vlfs release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE vlfs)
# Use vlfs release static target:
#target_link_libraries(${PROJECT_NAME} PRIVATE vlfs-static)

