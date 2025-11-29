//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_ISYSTEM_H
#define UNDEROCEAN_ISYSTEM_H
class ISystem {
public:
    virtual void update(float dt) = 0;
    virtual ~ISystem() = default;
};
#endif //UNDEROCEAN_ISYSTEM_H