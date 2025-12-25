//
// Created by 15201 on 12/21/2025.
//

#ifndef UNDEROCEAN_CLIENTLIGHTINGSYSTEM_H
#define UNDEROCEAN_CLIENTLIGHTINGSYSTEM_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "client/common/ResourceManager.h"
struct Obstacle {
    sf::FloatRect rect;
};
class LightingSystem {
public:
    void init(unsigned width, unsigned height) {
        lightMap_.create(width, height);
        lightMap_.setSmooth(true);
        lightShader_ = &ResourceManager::getShader("shaders/lightsource.frag");
    }
    void onWindowResize(unsigned width, unsigned height) {
        lightMap_.create(width, height);
        // sf::View v;
        // v.reset(sf::FloatRect(0.f, 0.f, float(width), float(height)));
        // lightMap_.setView(v);
        // lightShader_ = &ResourceManager::getShader("shaders/lightsource.frag");
    }
    void clear(sf::Color ambientColor) {
        lightMap_.clear(ambientColor);
    }
    void updateObstacles(const std::vector<sf::FloatRect>& obstacles) {
        obstacles_ = obstacles;
    }
    void drawLight(const sf::Vector2f& pos, const sf::View& view, float radius, sf::Color color) {
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();
        if (std::abs(pos.x - viewCenter.x) > viewSize.x/2 + radius ||
            std::abs(pos.y - viewCenter.y) > viewSize.y/2 + radius) {
            return;
        }
        sf::Vector2f screenPos = renderWindowMapCoordsToPixel(pos, view);
        float windowHeight = static_cast<float>(lightMap_.getSize().y);
        lightShader_->setUniform("u_center", sf::Vector2f(screenPos.x, windowHeight - screenPos.y));
        lightShader_->setUniform("u_radius", radius * getZoomFactor(view));
        lightShader_->setUniform("u_color", sf::Glsl::Vec3(color.r/255.f, color.g/255.f, color.b/255.f));
        lightShader_->setUniform("u_time", totalTime_);
        lightShader_->setUniform("u_intensity", 1.0f);
        sf::RenderStates states;
        states.blendMode = sf::BlendAdd;
        states.shader = lightShader_;
        circle.setRadius(radius * getZoomFactor(view));
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setPosition(screenPos);
        lightMap_.draw(circle, states);
    }
    void update(float dt) {
        totalTime_ += dt;
    }
    void display() {
        lightMap_.display();
    }
    sf::Sprite getLightMapSprite() const {
        return sf::Sprite(lightMap_.getTexture());
    }
    constexpr static float getLightRadius(EntityTypeID type) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::LIGHT_RADIUS; break;
            LIGHT_ENTITY_TYPES
#undef X
            default:
                return 0.f;
        }
    }
    constexpr static const sf::Uint8* getLightColor(EntityTypeID type) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::LIGHT_COLOR; break;
            LIGHT_ENTITY_TYPES
#undef X
            default:
                return nullptr;
        }
    }
private:
    sf::RenderTexture lightMap_;
    sf::Shader* lightShader_{};
    std::vector<sf::FloatRect> obstacles_;
    float totalTime_ { 0.f };
    sf::CircleShape circle;
    std::vector<sf::Vector2f> calculateVisibilityPolygon(const sf::Vector2f& origin, float radius){
        std::vector<float> angles;
        std::vector<sf::FloatRect> validObstacles;
        for (const auto& obs : obstacles_) {
            if (obs.contains(origin)) continue;
            if (!obs.intersects({
                origin.x - radius,
                origin.y - radius,
                radius * 2.f,
                radius * 2.f
            })) continue;
            validObstacles.push_back(obs);
            const sf::Vector2f corners[4] = {
                {obs.left, obs.top},
                {obs.left + obs.width, obs.top},
                {obs.left + obs.width, obs.top + obs.height},
                {obs.left, obs.top + obs.height}
            };
            for (auto& c : corners) {
                float a = std::atan2(c.y - origin.y, c.x - origin.x);
                angles.push_back(a - 0.0005f);
                angles.push_back(a);
                angles.push_back(a + 0.0005f);
            }
        }
        constexpr int BASE_RAYS = 64;
        for (int i = 0; i < BASE_RAYS; ++i) {
            angles.push_back(i * 2.f * 3.1415926f / BASE_RAYS);
        }
        std::sort(angles.begin(), angles.end());
        std::vector<sf::Vector2f> points;
        points.reserve(angles.size());
        for (float angle : angles) {
            sf::Vector2f dir(std::cos(angle), std::sin(angle));
            float minT = radius;
            for (const auto& obs : validObstacles) {
                float t = castRay(origin, dir, obs);
                if (t < minT) minT = t;
            }
            points.emplace_back(origin + dir * minT);
        }
        return points;
    }
    static float castRay(sf::Vector2f origin, sf::Vector2f dir, sf::FloatRect rect) {
        float tMin = 0.0f;
        float tMax = 100000.0f;
        if (std::abs(dir.x) < 0.0001f) {
            if (origin.x < rect.left || origin.x > rect.left + rect.width) return 100000.0f;
        } else {
            float t1 = (rect.left - origin.x) / dir.x;
            float t2 = (rect.left + rect.width - origin.x) / dir.x;
            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        }
        if (std::abs(dir.y) < 0.0001f) {
            if (origin.y < rect.top || origin.y > rect.top + rect.height) return 100000.0f;
        } else {
            float t1 = (rect.top - origin.y) / dir.y;
            float t2 = (rect.top + rect.height - origin.y) / dir.y;
            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        }
        if (tMax >= tMin && tMin >= 0) return tMin;
        return 100000.0f;
    }
    sf::Vector2f renderWindowMapCoordsToPixel(const sf::Vector2f& point, const sf::View& view) {
         sf::FloatRect viewport = view.getViewport();
        sf::Vector2f size = sf::Vector2f(lightMap_.getSize());
        sf::Vector2f normalized;
        normalized.x = (point.x - (view.getCenter().x - view.getSize().x / 2.f)) / view.getSize().x;
        normalized.y = (point.y - (view.getCenter().y - view.getSize().y / 2.f)) / view.getSize().y;
        return sf::Vector2f(
            (normalized.x * viewport.width + viewport.left) * size.x,
            (normalized.y * viewport.height + viewport.top) * size.y
        );
    }
    float getZoomFactor(const sf::View& view) {
        return static_cast<float>(lightMap_.getSize().x) / view.getSize().x;
    }
};
#endif //UNDEROCEAN_CLIENTLIGHTINGSYSTEM_H