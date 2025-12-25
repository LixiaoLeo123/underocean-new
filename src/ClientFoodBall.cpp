#include "client/common/ClientFoodBall.h"
#include <cmath>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include "client/common/ResourceManager.h"
#include "common/utils/Random.h"
static float length(const sf::Vector2f& v) { return std::sqrt(v.x * v.x + v.y * v.y); }
static sf::Vector2f normalize(const sf::Vector2f& v) { float l = length(v); return l == 0.f ? v : v / l; }
ClientFoodBall::ClientFoodBall(sf::Vector2f startPos)
    : position_(startPos), velocity_(0.f, 0.f) {
    static sf::Texture& blankTex = ResourceManager::getTexture("images/others/fp.png");
    sprite_.setTexture(blankTex);
    sf::FloatRect bounds = sprite_.getLocalBounds();
    sprite_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite_.setPosition(position_);
    sprite_.setScale(INIT_SCALE, INIT_SCALE);
    orbShader_ = &ResourceManager::getShader("shaders/energy_orb.frag");
}
void ClientFoodBall::startAbsorb() {
    isAbsorbing_ = true;
    spawnParticles(64, 100.f);;
}
void ClientFoodBall::update(float dt) {
    totalTime_ += dt;
    if (isAbsorbing_) {
        absorbTimer_ += dt;
        alpha_ -= 6.f * dt * alpha_;
        float r = lightColor_.r - 6.f * dt * lightColor_.r;
        float g = lightColor_.g - 6.f * dt * lightColor_.g;
        float b = lightColor_.b - 6.f * dt * lightColor_.b;
        lightColor_ = sf::Color(static_cast<sf::Uint8>(std::max(r, 0.f)),
                                 static_cast<sf::Uint8>(std::max(g, 0.f)),
                                 static_cast<sf::Uint8>(std::max(b, 0.f)));
        // lightColor_.a -= 6.f * dt * lightColor_.a;
        if (absorbTimer_ > ABSORT_DURATION) {
            dead_ = true;
        }
    } else {
        updatePos(dt);
        sprite_.setRotation(totalTime_ * 30.f);
        if (totalTime_ - particleSpawnTimer_ > 0.1f) {
            spawnParticles(1, 20.f);
            particleSpawnTimer_ = totalTime_;
        }
    }
    sprite_.setPosition(position_);
    for (auto it = particles_.begin(); it != particles_.end();) {
        it->life -= dt;
        it->position += it->velocity * dt;
        if (isAbsorbing_) {
             it->velocity += normalize(position_ - it->position) * 1.1f * dt;
        }
        if (it->life <= 0.f) {
            it = particles_.erase(it);
        } else {
            ++it;
        }
    }
}
void ClientFoodBall::spawnParticles(int count, float speedSpread) {
    for (int i = 0; i < count; ++i) {
        FoodParticle p;
        p.position = position_;
        float speed = Random::randFloat(0.f, speedSpread);
        p.velocity = Random::fastRandUnitVector() * speed;
        if (isAbsorbing_) {
             p.velocity -= velocity_ * 0.2f;
        }
        p.life = 1.0f;
        p.maxLife = 1.0f;
        p.color = Random::randInt(0, 1) ? sf::Color(0, 255, 255) : sf::Color(200, 50, 255);
        particles_.push_back(p);
    }
}
void ClientFoodBall::render(sf::RenderWindow& window) {
    if (!particles_.empty()) {
        sf::VertexArray va(sf::Points, particles_.size());
        for (size_t i = 0; i < particles_.size(); ++i) {
            va[i].position = particles_[i].position;
            sf::Color c = particles_[i].color;
            c.a = static_cast<sf::Uint8>(255 * particles_[i].life);
            va[i].color = c;
        }
        sf::RenderStates particleStates;
        particleStates.blendMode = sf::BlendAdd;
        window.draw(va, particleStates);
    }
    orbShader_->setUniform("u_time", totalTime_);
    orbShader_->setUniform("u_intensity", isAbsorbing_ ? 2.5f : 1.0f);
    orbShader_->setUniform("u_alpha", alpha_);
    sf::RenderStates states;
    states.shader = orbShader_;
    states.blendMode = sf::BlendAdd;
    window.draw(sprite_, states);
}