#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
Coordinator::Coordinator(){
    registerComponent<Transform>();
    registerComponent<Velocity>();
    registerComponent<Acceleration>();
}
