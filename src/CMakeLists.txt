cmake_minimum_required(VERSION 3.26.3)
project(ldaNES C)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-W -Wall -ggdb)

add_executable(ldaNES
	bus.c
	main.c
	cpu.c
	ppu.c
	cartridge.c
)

add_executable(test
	testCPUInstructions.c
	bus.c
	cpu.c
	ppu.c
)

target_link_libraries(test PRIVATE unity)
target_link_libraries(ldaNES PRIVATE SDL2)
target_link_libraries(ldaNES PRIVATE SDL2_image)