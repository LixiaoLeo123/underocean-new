//
// Created by 15201 on 12/11/2025.
//

#ifndef UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H
#define UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H
#include "ISystem.h"

class DerivedAttributeSystem : public ISystem{  //calculate maxHP, maxFP, etc. by entity size and type
public:
    void update(float dt) override {}
};
#endif //UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H