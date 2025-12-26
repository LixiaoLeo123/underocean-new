//
// Created by 15201 on 12/26/2025.
//

#ifndef UNDEROCEAN_TIMERESOURCE_H
#define UNDEROCEAN_TIMERESOURCE_H
struct TimeResource {
    float currentTime = 0.f;  //from 0.f to 10.f (min)
    void reset() {
        currentTime = 0.f;
    }
};
#endif //UNDEROCEAN_TIMERESOURCE_H