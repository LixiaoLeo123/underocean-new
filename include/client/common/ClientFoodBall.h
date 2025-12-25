#ifndef CLIENT_FOOD_BALL_H
#define CLIENT_FOOD_BALL_H
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "NetworkEntity.h"
namespace sf {
    class View;
    class RenderWindow;
}
struct FoodParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float life;      //1.0 -> 0.0
    float maxLife;
    sf::Color color;
};
class ClientFoodBall : public NetworkEntity{
public:
    explicit ClientFoodBall(sf::Vector2f startPos = {});
    void setStartPos(sf::Vector2f startPos) {
        position_ = startPos;
        setNetworkState(startPos, 0);
    }
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    void startAbsorb();
    [[nodiscard]] bool isDead() const override { return dead_; }
    // [[nodiscard]] NetworkEntity* getTarget() const { return target_; }  //get absorbing target to set nullptr if target is gone
private:
    constexpr static float INIT_SCALE = 1.2f;
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::Sprite sprite_;
    sf::Shader* orbShader_;
    float totalTime_ = 0.f;
    bool isAbsorbing_ = false;  //absorb
    float absorbSpeed_ = 0.f;
    float scaleFactor_ = 1.f;
    float rotationAngle_ = 0.f;
    std::vector<FoodParticle> particles_;  //particle
    float particleSpawnTimer_ = 0.f;
    bool dead_ = false;
    float absorbTimer_ = 0.f;
    constexpr static float ABSORT_DURATION = 3.f;
    void spawnParticles(int count, float speedSpread);
    float alpha_{1.f};  //for absorb effect
};
#endif