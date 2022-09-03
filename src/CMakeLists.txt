find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(app)
add_subdirectory(graphics)

add_library(vkeng INTERFACE)
target_link_libraries(vkeng INTERFACE vkeng-app vkeng-grph)
target_include_directories(vkeng INTERFACE ${PROJECT_SOURCE_DIR}/include)
