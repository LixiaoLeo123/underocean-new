cmake_minimum_required(VERSION 4.0)
project(Underocean)

set(CMAKE_CXX_STANDARD 17)

# 设置 SFML 路径
set(SFML_DIR "C:/Users/15201/Documents/code/cpp/SFML-2.6.2-windows-vc17-64-bit/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS audio network graphics window system)

if (NOT SFML_FOUND)
    message(FATAL_ERROR "SFML NOT FOUND!")
else()
    message("SFML FOUND!")
endif()

# 注意：Button.cpp 重复了多次，建议清理（不影响编译但不整洁）
add_executable(Underocean
        main.cpp
        src/Button.cpp
        src/GameData.cpp
        src/INodeScene.cpp
        src/LazyLayout.cpp
        src/StartMenu.cpp
        src/InputManager.cpp
        src/RootScene.cpp
        src/TextButton.cpp
        src/NameScene.cpp
        src/enet_wrapper.cpp
        src/LevelSelectMenu.cpp
        src/Coordinator.cpp
        src/GameServer.cpp
        src/BoidsSystem.cpp
        src/EntityFactory.cpp
        src/NetworkSyncSystem.cpp
)

# 添加头文件目录
target_include_directories(Underocean PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# ✅ 统一使用 keyword 语法（全部用 PRIVATE）
target_link_libraries(Underocean PRIVATE
        sfml-audio
        sfml-network
        sfml-graphics
        sfml-window
        sfml-system
)

# Windows 额外链接系统库
if(WIN32)
    target_link_libraries(Underocean PRIVATE
            ws2_32
            winmm
    )
endif()