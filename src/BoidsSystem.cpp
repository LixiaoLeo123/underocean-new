//
// Created by 15201 on 12/1/2025.
//
#include ""
#include "server/new/system/BoidsSystem.h"
const std::array<SmallFish::Params, static_cast<size_t>(SmallFishType::COUNT)>& BoidsSystem::getParamsTable() {
    static const std::array<Params, static_cast<size_t>(SmallFishType::COUNT)> table = { {
        {//0
            100.0f,   // neighborRadius2
            50.0f,     // separationRadius2
            300.0f,   // avoidRadius2
            20.0f,       // cohesionWeight
            1000.0f,       // separationWeight
            100.0f,       // alignmentWeight
            2.0f,       // avoidWeight
            10.0f,      // maxVelocity
            30,         // spriteWidth
            12,          // spriteHeight
            "images/smallfish/smallfish0.png",     //textureId
            2,           //minSize
            10           //maxSize
        },
        {//1
            70.0f,
            50.0f,
            600.0f,
            15.0f,
            1000.0f,
            100.0f,
            5.0f,
            40.0f,
            30,
            12,
            "images/smallfish/smallfish1.png",
            2,
            10
        },
        {//2
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            12,
            6,
            "images/smallfish/smallfish2.png",
            2,
            10
        },
        {//3
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            54,
            22,
            "images/smallfish/smallfish3.png",
            2,
            10
        },
        {//4
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            20,
            12,
            "images/smallfish/smallfish4.png",
            2,
            10
        },
        {//5
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            26,
            12,
            "images/smallfish/smallfish5.png",
            2,
            10
        },
        {//6
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            28,
            24,
            "images/smallfish/smallfish6.png",
            2,
            10
        },
        {//7
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            16,
            12,
            "images/smallfish/smallfish7.png",
            2,
            10
        }
    } };
    return table;
}