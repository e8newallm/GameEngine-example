
add_executable(Game main.cpp player.cpp)
set_target_properties(Game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Game/"
)

target_link_libraries(Game PUBLIC ${SDL2_LIBRARIES} SDL2_image GameEngine)
target_include_directories(Game PUBLIC ${SDL2_INCLUDE_DIRS})
add_custom_command(TARGET Game PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/tex/ $<TARGET_FILE_DIR:Game>/tex)
