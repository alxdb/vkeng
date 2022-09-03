find_package(glfw3 3.3 REQUIRED)

add_library(vkeng-app app.cpp)

target_link_libraries(vkeng-app PRIVATE glfw spdlog fmt)
target_include_directories(vkeng-app PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(vkeng-app PUBLIC SPDLOG_FMT_EXTERNAL)
