
add_executable(Game main.cpp player.cpp)
set_target_properties(Game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Game/"
)

target_link_libraries(Game PUBLIC ${SDL2_LIBRARIES} SDL2_image GameEngine)
target_include_directories(Game PUBLIC ${SDL2_INCLUDE_DIRS} GameEngine)

  add_custom_target(DataPackager
  ALL
  COMMAND PackagerBin ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:Game>/data.bin
  DEPENDS Packager PackagerBin
)