cmake_minimum_required(VERSION 3.26)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME}-static main.cpp)
# Find package arba-vlfs:
find_package(arba-vlfs 0.3.0 CONFIG REQUIRED)
# Use arba-vlfs release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE arba-vlfs)
# Use arba-vlfs release static target:
target_link_libraries(${PROJECT_NAME}-static PRIVATE arba-vlfs-static)
