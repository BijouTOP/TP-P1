cmake_minimum_required(VERSION 3.10)
project(MyProject C)

# 1. Set Compiler Flags (CFLAGS)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Werror -Wpedantic")

# 2. Define Include Directories (-isystem)
# Using SYSTEM to suppress warnings from external headers
include_directories(SYSTEM ./libraries/)

# 3. Define Link Directories (-L)
link_directories(./libraries/)

# 4. Define the Target and Source Files
set(FOLDERPATH "./src/")
add_executable(output ${FOLDERPATH}main.c)

# 5. Link Libraries (-l)
target_link_libraries(output 
    raylib 
    GL 
    m 
    pthread 
    dl 
    rt 
    X11
)

# 6. Custom Command for 'check' (cppcheck)
find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    add_custom_target(check
        COMMAND ${CPPCHECK_PATH} --enable=all --std=c99 --suppress=missingIncludeSystem 
                -I./libraries/ -L./libraries/ -Dbool=_Bool ${FOLDERPATH}main.c
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cppcheck..."
    )
endif()