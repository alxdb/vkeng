add_custom_command(
  OUTPUT shader.vert.spv
  COMMAND Vulkan::glslc ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/shader.vert.spv
  DEPENDS shader.vert
)

add_custom_command(
  OUTPUT shader.frag.spv
  COMMAND Vulkan::glslc ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag -o ${CMAKE_CURRENT_BINARY_DIR}/shader.frag.spv
  DEPENDS shader.frag
)

add_executable(shaders_main main.cpp)
target_link_libraries(shaders_main)

add_custom_command(
  OUTPUT shaders.c shaders.h
  COMMAND $<TARGET_FILE:shaders_main> ${CMAKE_CURRENT_BINARY_DIR}/shader.frag.spv ${CMAKE_CURRENT_BINARY_DIR}/shader.vert.spv
  DEPENDS shaders_main shader.vert.spv shader.frag.spv
)

add_library(shaders shaders.c shaders.h)
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
