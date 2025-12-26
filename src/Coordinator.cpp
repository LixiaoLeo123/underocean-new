#include "server/new/Coordinator.h"

#include "client/common/KeyBindingManager.h"
#include "server/new/component/Components.h"
Coordinator::Coordinator(){
    registerComponent<Transform>();
    registerComponent<Velocity>();
    registerComponent<Force>();
    registerComponent<MaxVelocity>();
    registerComponent<MaxAcceleration>();
    registerComponent<EntityType>();
    registerComponent<Boids>();
    registerComponent<NetworkPeer>();
    registerComponent<Size>();
    registerComponent<ForceLoadChunk>();
    registerComponent<HP>();
    registerComponent<FP>();
    registerComponent<Mass>();
    registerComponent<NetSyncComp>();
    registerComponent<Collision>();
    registerComponent<Attack>();
    registerComponent<Defence>();
    registerComponent<Invincibility>();
    registerComponent<EntityClearTag>();
    registerComponent<FoodBall>();
    registerComponent<NoNetControl>();
    registerComponent<FoodBall>();
    registerComponent<NameTag>();
    registerComponent<Chase>();
    registerSystem(Signature{});  //empty to get to
}
