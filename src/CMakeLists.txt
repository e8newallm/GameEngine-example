file(GLOB gamesrc
    main.cpp
    game.cpp
    player.cpp
)

add_executable(Game ${gamesrc})
set_target_properties(Game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Game/"
)

if(DEBUG)
    target_compile_definitions(Game PUBLIC DEBUG)
endif()


target_link_libraries(Game PUBLIC ${SDL3_LIBRARIES} SDL3_image GameEngine)
target_include_directories(Game PUBLIC ${SDL3_INCLUDE_DIRS} GameEngine)

package_gen(${CMAKE_SOURCE_DIR}/data "${CMAKE_BINARY_DIR}/Game/data.bin")