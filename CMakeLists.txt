cmake_minimum_required(VERSION 3.10)

# Project name
project(My2DPlatformerSFML LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Find SFML
# ------------------------------------------------------------
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

# ------------------------------------------------------------
# Build unp13e Library
# ------------------------------------------------------------
set(UNP13E_DIR ${CMAKE_SOURCE_DIR}/unpv13e)

# Run `./configure` in the src directory
execute_process(
    COMMAND ./configure
    WORKING_DIRECTORY ${UNP13E_DIR}
    RESULT_VARIABLE CONFIGURE_RESULT
)

if(NOT CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to configure unp13e.")
endif()

# Build the libraries in unp13e
execute_process(
    COMMAND make
    WORKING_DIRECTORY ${UNP13E_DIR}/lib
    RESULT_VARIABLE LIB_MAKE_RESULT
)

if(NOT LIB_MAKE_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to build unp13e lib.")
endif()

execute_process(
    COMMAND make
    WORKING_DIRECTORY ${UNP13E_DIR}/libfree
    RESULT_VARIABLE LIBFREE_MAKE_RESULT
)

#if(NOT LIBFREE_MAKE_RESULT EQUAL 0)
#    message(FATAL_ERROR "Failed to build unp13e libfree.")
#endif()

# Optional: Build libroute if system supports 4.4BSD-style routing sockets
if(NOT WIN32 AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    execute_process(
        COMMAND make
        WORKING_DIRECTORY ${UNP13E_DIR}/libroute
        RESULT_VARIABLE LIBROUTE_MAKE_RESULT
    )

    if(NOT LIBROUTE_MAKE_RESULT EQUAL 0)
        message(WARNING "Failed to build unp13e libroute. Skipping.")
    endif()
endif()

# ------------------------------------------------------------
# Add unp13e Libraries and Headers
# ------------------------------------------------------------
# Add the generated libraries to the project
add_library(unp_lib STATIC IMPORTED)
set_target_properties(unp_lib PROPERTIES IMPORTED_LOCATION ${UNP13E_DIR}/lib/libunp.a)

add_library(unp_libfree STATIC IMPORTED)
set_target_properties(unp_libfree PROPERTIES IMPORTED_LOCATION ${UNP13E_DIR}/libfree/libunpfree.a)

# Include headers
include_directories(${UNP13E_DIR}/lib ${UNP13E_DIR}/libfree)

# Optional: Add libroute if it exists
if(EXISTS ${UNP13E_DIR}/libroute/libunproute.a)
    add_library(unp_libroute STATIC IMPORTED)
    set_target_properties(unp_libroute PROPERTIES IMPORTED_LOCATION ${UNP13E_DIR}/libroute/libunproute.a)
    set(UNP_LIBRARIES unp_lib unp_libfree unp_libroute)
else()
    set(UNP_LIBRARIES unp_lib unp_libfree)
endif()

# ------------------------------------------------------------
# Add Executable and Link Libraries
# ------------------------------------------------------------
set(SOURCES
    Bullet.cpp
    Coin.cpp
    Core.cpp
    Enemy.cpp
    Game.cpp
    Ingamebar.cpp
    Main.cpp
    Map.cpp
    Menu.cpp
    Player.cpp
    Portal.cpp
    RenderManager.cpp
    ResourceHolder.cpp
)

set(HEADERS
    Bullet.hpp
    Coin.hpp
    Core.hpp
    Enemy.hpp
    Game.hpp
    Map.hpp
    Menu.hpp
    Player.hpp
    Portal.hpp
    Render.hpp
    RenderManager.hpp
    ResourceHolder.hpp
    Ingamebar.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SFML and unp13e libraries
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-window sfml-graphics sfml-audio ${UNP_LIBRARIES})

# ------------------------------------------------------------
# Copy Resources (img, sound, font)
# ------------------------------------------------------------
file(COPY ${CMAKE_SOURCE_DIR}/img DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/sound DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/font.ttf DESTINATION ${CMAKE_BINARY_DIR})
