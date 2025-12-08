#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
Coordinator::Coordinator(){
    registerComponent<Transform>();
    registerComponent<Velocity>();
    registerComponent<Acceleration>();
    registerComponent<MaxVelocity>();
    registerComponent<MaxAcceleration>();
    registerComponent<EntityType>();
    registerComponent<Boids>();
    registerComponent<NetworkPeer>();
    registerComponent<Size>();
    registerSystem(Signature{});  //empty to get to
}
