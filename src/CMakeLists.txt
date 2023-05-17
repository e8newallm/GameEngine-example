find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(RapidJSON REQUIRED)

file(GLOB src 
)

file(GLOB headers
)

add_executable(Game ${src})
target_link_libraries(Game PUBLIC ${SDL2_LIBRARIES})
target_include_directories(Game PUBLIC . ${headers} ${SDL2_INCLUDE_DIRS} ../engine)
