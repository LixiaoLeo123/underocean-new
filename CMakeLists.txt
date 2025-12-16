cmake_minimum_required(VERSION 4.0)
project(Underocean)

set(CMAKE_CXX_STANDARD 17)


set(SFML_DIR "C:/Users/15201/Documents/code/cpp/SFML-2.6.2-windows-vc17-64-bit/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS audio network graphics window system)

if (NOT SFML_FOUND)
    message(FATAL_ERROR "SFML NOT FOUND!")
else()
    message("SFML FOUND!")
endif()


add_executable(Underocean
        main.cpp
        src/DerivedAttributeSystem.cpp
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
        src/Level1.cpp
        src/LevelSceneBase.cpp
        src/PlayerStatus.cpp
        src/PlayerStatus.cpp
        src/Integrity.cpp
        src/ChatBox.cpp
        src/SkillBar.cpp
        src/SkillSystem.cpp
)


target_include_directories(Underocean PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(Underocean PRIVATE
        sfml-audio
        sfml-network
        sfml-graphics
        sfml-window
        sfml-system
)


if(WIN32)
    target_link_libraries(Underocean PRIVATE
            ws2_32
            winmm
    )
endif()