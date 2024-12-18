cmake_minimum_required(VERSION 3.10)

# 專案名稱，可以根據您的需求更改
project(My2DPlatformerSFML LANGUAGES CXX)

# 設定 C++ 標準（例如 C++17）
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# 尋找 SFML
# ------------------------------------------------------------
# 假設您在 WSL (Ubuntu 或其他 Linux 環境) 中安裝了 SFML：
# sudo apt-get install libsfml-dev
# find_package 會自動尋找 SFML 的 include 和 lib
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

#add unpv13e

# Run the shell script at configuration time
execute_process(
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/unpv13e/
    COMMAND ./configure
)

# Collect all .c files from lib and libfree
include_directories(
    "${CMAKE_SOURCE_DIR}/unpv13e/lib"
    "${CMAKE_SOURCE_DIR}/unpv13e/libfree"
    "${CMAKE_SOURCE_DIR}/unpv13e"
)
# Combine the source files
# ------------------------------------------------------------
# 設定 Debug 和 Release 模式下的編譯選項 (類似於 vcxproj 定義)
# ------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
    # 您可以在此處加入其他 debug 模式下的編譯選項
    #add_compile_options(-O3)
else()
    add_compile_definitions(NDEBUG)
endif()

add_compile_definitions(_CONSOLE)

# ------------------------------------------------------------
# 將所有原本在 vcxproj 中的 source 和 header 檔加入
# 您可以依實際檔案路徑調整。
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
    MySocket.cpp
    GameOnline.cpp
    PlayerOnline.cpp
    SocketManager.cpp)

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
    MySocket.hpp
    GameOnline.hpp
    PlayerOnline.hpp
    SocketManager.hpp)

# ------------------------------------------------------------
# 建立可執行檔
# ------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
#-------------------------------------------------------------
#將img ,sound 和 dont copy 到 exe
# ------------------------------------------------------------


# Function to copy a directory
file(COPY ${CMAKE_SOURCE_DIR}/img DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/sound DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/font.ttf DESTINATION ${CMAKE_BINARY_DIR})
# 設定 include 路徑（如果需要特定 include 路徑）
# 在 Windows 時您可能需要 `include_directories()` 指向 SFML 的 include 路徑，
# 但在 Linux 下若使用 find_package 通常不需要手動指定。
# 
# 如果你仍需要指定，可如此加入:
# include_directories(/path/to/SFML/include)
# ------------------------------------------------------------

# ------------------------------------------------------------
# 連結 SFML 函式庫
# ------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

# 如果需要其他預處理器定義（例如：WIN32），可根據實際需求加上：
# target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)

# 如果您需要加上 Windows 平台才有的定義，可使用條件式：
# if(WIN32)
#     target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
# endif()

# ------------------------------------------------------------
# 若您需要類似 vcxproj 中針對 release/debug 模式的最佳化旗標：
# 可使用 CMake 的 build-type 切換：
# cmake -DCMAKE_BUILD_TYPE=Release ..
# 或在 CMakeLists.txt 中設定不同模式的選項:
# 
# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2")
# ------------------------------------------------------------

