//
// Created by 15201 on 12/20/2025.
//

#ifndef UNDEROCEAN_DEATHEFFECTSYSTEM_H
#define UNDEROCEAN_DEATHEFFECTSYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include <random>
#include "ResourceManager.h"
class DeathEffectSystem {
private:
    struct GoreChunk {
        sf::Sprite sprite;
        sf::Vector2f velocity;
        float angularVelocity;
        float lifetime;
        float maxLifetime;
    };
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Color color;
        float size;
        float lifetime;
        float maxLifetime;
    };
    std::vector<GoreChunk> gores_;
    std::vector<Particle> particles_;
    sf::Shader& goreShader_;
    sf::Texture* noiseTexture_ = nullptr;
    sf::Texture* particleTexture_ = nullptr;
    std::mt19937 rng_;
    sf::VertexArray particleVertices_;
public:
    DeathEffectSystem() : goreShader_(ResourceManager::getShader("shaders/gore.frag")),
    particleVertices_(sf::Quads) {
        rng_.seed(std::random_device()());
        goreShader_.setUniform("texture", sf::Shader::CurrentTexture);
    }
    void init(sf::Texture* noiseTex, sf::Texture* particleTex) {
        noiseTexture_ = noiseTex;
        particleTexture_ = particleTex;
        if (noiseTexture_) {
            noiseTexture_->setRepeated(true);
            goreShader_.setUniform("noiseTexture", *noiseTexture_);
        }
    }
    void spawnDeathEffect(const sf::Sprite& originalSprite, const sf::Vector2f& entityVel, float entitySizeScale) {
        const sf::Texture* tex = originalSprite.getTexture();
        if (!tex) return;
        sf::IntRect rect = originalSprite.getTextureRect();
        sf::Vector2f pos = originalSprite.getPosition();
        sf::Vector2f scale = originalSprite.getScale();
        float rotation = originalSprite.getRotation() * 3.14159f / 180.f;
        int cuts = static_cast<int>(rect.width * entitySizeScale * 1.3f);
        if (cuts < 2) cuts = 2;
        if (cuts > originalSprite.getTexture()->getSize().y)
            cuts = originalSprite.getTexture()->getSize().y;
        float chunkW = static_cast<float>(rect.width) / cuts;
        float chunkH = static_cast<float>(rect.height) / cuts;
        for (int i = 0; i < cuts; ++i) {
            for (int j = 0; j < cuts; ++j) {
                GoreChunk chunk;
                chunk.sprite.setTexture(*tex);
                chunk.sprite.setTextureRect(sf::IntRect(rect.left + i * chunkW, rect.top + j * chunkH, chunkW, chunkH));
                chunk.sprite.setOrigin(chunkW / 2.f, chunkH / 2.f);
                chunk.sprite.setScale(scale);
                float localX = (i - (cuts - 1) / 2.f) * chunkW * scale.x;
                float localY = (j - (cuts - 1) / 2.f) * chunkH * scale.y;
                float worldX = pos.x + localX * std::cos(rotation) - localY * std::sin(rotation);
                float worldY = pos.y + localX * std::sin(rotation) + localY * std::cos(rotation);
                chunk.sprite.setPosition(worldX, worldY);
                chunk.sprite.setRotation(originalSprite.getRotation());
                sf::Vector2f explodeDir = randomVec();
                float explodeSpeed = randomFloat(2.0f, 6.0f);
                chunk.velocity = entityVel * 0.5f + explodeDir * explodeSpeed;
                chunk.angularVelocity = randomFloat(-720.f, 720.f);
                chunk.maxLifetime = randomFloat(2.5f, 4.0f);
                chunk.lifetime = chunk.maxLifetime;
                gores_.push_back(chunk);
            }
        }
        int bloodCount = 20 * rect.width * rect.height * entitySizeScale * entitySizeScale;
        for (int i = 0; i < bloodCount; ++i) {
            Particle p;
            p.position = pos + randomVec() * Random::randFloat(0.f, (float(rect.width) * entitySizeScale * 0.6f));
            sf::Vector2f dir = randomVec();
            float speed = randomFloat(2.0f, 16.0f);
            p.velocity = entityVel * 0.3f + dir * speed;
            p.size = randomFloat(0.048f, 0.16f);
            p.maxLifetime = randomFloat(1.0f, 2.5f);
            p.lifetime = p.maxLifetime;
            int r = std::uniform_int_distribution<int>(150, 255)(rng_);
            p.color = sf::Color(r, 0, 0, 255);
            particles_.push_back(p);
        }
    }
    void update(float dt) {
        for (auto it = gores_.begin(); it != gores_.end();) {
            it->lifetime -= dt;
            if (it->lifetime <= 0) {
                it = gores_.erase(it);
                continue;
            }
            it->velocity *= 0.96f;
            it->angularVelocity *= 0.97f;
            it->velocity.y += 2.0f * dt;
            it->sprite.move(it->velocity * dt);
            it->sprite.rotate(it->angularVelocity * dt);
            float lifeRatio = it->lifetime / it->maxLifetime;
            auto darkness = static_cast<sf::Uint8>(255 * (0.5f + 0.5f * lifeRatio));
            it->sprite.setColor(sf::Color(darkness, darkness, darkness, 255));
            ++it;
        }
        for (auto it = particles_.begin(); it != particles_.end();) {
            it->lifetime -= dt;
            if (it->lifetime <= 0) {
                it = particles_.erase(it);
                continue;
            }
            it->velocity *= 0.92f;
            it->velocity.y += 1.0f * dt;
            it->position += it->velocity * dt;
            ++it;
        }
    }
    void render(sf::RenderWindow& window) {
        constexpr static float EROSION_START = 0.1;
        for (const auto& gore : gores_) {
            float erosion = EROSION_START;
            if (gore.lifetime < gore.maxLifetime * 0.5f) {
                //map 0.x->0.0 to 0.x->1.0
                erosion = 1.0f - (gore.lifetime / (gore.maxLifetime * 0.5f)) * (1.f - EROSION_START);
            }
            goreShader_.setUniform("erosion", erosion);
            goreShader_.setUniform("colorTint", sf::Glsl::Vec4(1.0, 0.9, 0.9, 1.0));
            window.draw(gore.sprite, &goreShader_);
        }
        if (particles_.empty()) return;
        particleVertices_.clear();
        particleVertices_.resize(particles_.size() * 4);
        // sf::Vector2f centerOffset(8.f, 8.f);  //16 x 16
        // if (particleTexture_) {
        //      centerOffset = sf::Vector2f(particleTexture_->getSize().x / 2.f, particleTexture_->getSize().y / 2.f);
        // }
        for (size_t i = 0; i < particles_.size(); ++i) {
            const auto& p = particles_[i];
            size_t idx = i * 4;
            float size = p.size;
            float scale = size * (p.lifetime / p.maxLifetime);
            sf::Color col = p.color;
            col.a = static_cast<sf::Uint8>(255.f * 0.64f * (p.lifetime / p.maxLifetime));
            sf::Vector2f pos = p.position;
            float halfSize = 2.f * scale;
            particleVertices_[idx + 0].position = pos + sf::Vector2f(-halfSize, -halfSize);
            particleVertices_[idx + 1].position = pos + sf::Vector2f(halfSize, -halfSize);
            particleVertices_[idx + 2].position = pos + sf::Vector2f(halfSize, halfSize);
            particleVertices_[idx + 3].position = pos + sf::Vector2f(-halfSize, halfSize);
            for(int j=0; j<4; ++j) particleVertices_[idx+j].color = col;
            if (particleTexture_) {
                sf::Vector2u texSize = particleTexture_->getSize();
                particleVertices_[idx + 0].texCoords = sf::Vector2f(0.f, 0.f);
                particleVertices_[idx + 1].texCoords = sf::Vector2f(texSize.x, 0.f);
                particleVertices_[idx + 2].texCoords = sf::Vector2f(texSize.x, texSize.y);
                particleVertices_[idx + 3].texCoords = sf::Vector2f(0.f, texSize.y);
            }
        }
        sf::RenderStates states;
        states.texture = particleTexture_;
        states.blendMode = sf::BlendAlpha;
        window.draw(particleVertices_, states);
    }
private:
    float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng_);
    }
    sf::Vector2f randomVec() {
        float angle = randomFloat(0.f, 6.28318f);
        return sf::Vector2f(std::cos(angle), std::sin(angle));
    }
};

#endif //UNDEROCEAN_DEATHEFFECTSYSTEM_H