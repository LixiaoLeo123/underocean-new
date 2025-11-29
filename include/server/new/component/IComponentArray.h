//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_ICOMPONENTARRAY_H
#define UNDEROCEAN_ICOMPONENTARRAY_H
#include "common/Types.h"
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};
#endif //UNDEROCEAN_ICOMPONENTARRAY_H