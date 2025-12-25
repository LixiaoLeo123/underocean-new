// cpp
#ifndef UNDEROCEAN_NETWORKENTITY_H
#define UNDEROCEAN_NETWORKENTITY_H

#include "server/core(deprecate)/GameData.h"
#include <algorithm>
#include <cmath>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "ResourceManager.h"
#include "common/Types.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class NetworkEntity {
public:
    virtual ~NetworkEntity() = default;
    NetworkEntity()
    : clientPos_(0.f, 0.f), prevNetPos_(0.f,0.f), netPos_(0.f,0.f),
      velocity_(0.f,0.f),
      SERVER_DT_(static_cast<float>(TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC) / static_cast<float>(GameData::SERVER_TPS)),
      interpTimer_(0.f), hasNet_(false), hasPrevNet_(false)
    {
        hpBarBg_.setFillColor(sf::Color(40, 40, 40));
        hpBarBg_.setOutlineColor(sf::Color::Black);
        hpBarBg_.setOutlineThickness(0.25f);
        hpBarFill_.setOutlineThickness(0.15f);
        hpBarFill_.setFillColor(sf::Color(0, 220, 0));
    }
    //for gore
    [[nodiscard]] const sf::Sprite& getSprite() const { return sprite_; }
    [[nodiscard]] const sf::Vector2f& getScale() const { return sprite_.getScale(); }
    virtual void render(sf::RenderWindow& window) {
        if (hurtFlash_ > 0.f && hurtShader_) {
            hurtShader_->setUniform("u_flash", hurtFlash_);
            sf::RenderStates states;
            states.shader = hurtShader_;
            window.draw(sprite_, states);
        } else {
            window.draw(sprite_);
        }
        if (hpDisplayTimer_ > 0.f) {
            window.draw(hpBarBg_);
            window.draw(hpBarFill_);
        }
        sf::FloatRect textBounds = nameText_.getLocalBounds();
        sf::Vector2f basePos = sprite_.getPosition();
        if (hasNameTag_) {
            float yOffset = sprite_.getGlobalBounds().height / 2.f + nameTagYOffset_;
            sf::Vector2f textPos(
                basePos.x - textBounds.width / 2.f,
                basePos.y - yOffset - textBounds.height
            );
            constexpr float paddingX = 0.1f;
            constexpr float paddingY = 0.02f;
            nameBg_.setSize({
                textBounds.width + paddingX * 2.f,
                textBounds.height + paddingY * 2.f
            });
            nameBg_.setOrigin(0.f, 0.f);
            nameBg_.setPosition(
                textPos.x - paddingX,
                textPos.y - paddingY
            );
            nameText_.setPosition(textPos);
            window.draw(nameBg_);
            window.draw(nameText_);
        }
    }
    // Immediate local placement (no interpolation)
    void setPos(float x, float y) {
        clientPos_.x = x;
        clientPos_.y = y;
        sprite_.setPosition(clientPos_);
        hasNet_ = false;
        hasPrevNet_ = false;
        velocity_ = {0.f, 0.f};
    }
    void setPos(sf::Vector2f pos){ setPos(pos.x, pos.y); }
    void setLightProps(float radius, sf::Color color) {
        lightRadius_ = radius;
        lightColor_ = color;
        hasLight_ = (radius > 0.01f);
    }
    // server sends only position and server update interval
    void setNetworkState(const sf::Vector2f& pos, unsigned currentTick) {  //tick to avoid two state in one tick, causing inf velocity
        if (!hasNet_) {
            // first authoritative sample: set both prev and current to same value
            prevNetPos_ = pos;
            netPos_ = pos;
            velocity_ = {0.f, 0.f};
            interpTimer_ = SERVER_DT_;
            hasNet_ = true;
            hasPrevNet_ = false;
            // snap client to server if desired; keep current clientPos_ to avoid pops
            clientPos_ = netPos_;
            sprite_.setPosition(clientPos_);
            return;
        }
        if (lastTick_ != currentTick) {
            if (interpTimer_ > 0.001f)
                velocity_ = (pos - netPos_) / interpTimer_;
            prevNetPos_ = netPos_;
            netPos_ = pos;
            interpTimer_ = 0.f;
        }
        else {
            netPos_ = pos;
            if (interpTimer_ > 0.001f)
                velocity_ = (pos - prevNetPos_) / interpTimer_;
        }
        hasPrevNet_ = true;
        lastTick_ = currentTick;
    }
    // Call each frame with delta time in seconds
    virtual void update(float dt) {
        updatePos(dt);
        updateAngle();
        updateAnim(dt);
        updateHpBar(dt);
        if (hurtFlash_ > 0.f) {
            hurtFlash_ -= hurtFlashDecay_ * dt;
            if (hurtFlash_ < 0.f) hurtFlash_ = 0.f;
        }
    }
    void setHpPercentage(float pct);
    void updatePos(float dt);
    void updateAngle();
    void updateAnim(float dt);
    void setType(EntityTypeID type);
    void setSize(float size);
    void setNameTag(const std::string& name);
    void clearNameTag() { hasNameTag_ = false; }
    void triggerHurtFlash(float strength = 1.f) {
        hurtFlash_ = std::min(1.f, hurtFlash_ + strength);
    }
    [[nodiscard]] sf::Vector2f getPosition() const { return clientPos_; }
    [[nodiscard]] sf::Vector2f getVelocity() const { return velocity_; }
    [[nodiscard]] bool hasLight() const { return hasLight_; }
    [[nodiscard]] float getLightRadius() const { return lightRadius_; }
    [[nodiscard]] sf::Color getLightColor() const { return lightColor_; }
    [[nodiscard]] EntityTypeID getType() const { return type_; }
    [[nodiscard]] virtual bool isDead() const { return false; }
private:
    void updateHpBar(float dt);
    static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
        return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
    }
    sf::Sprite sprite_;
    sf::Vector2f clientPos_;     // currently drawn position
    sf::Vector2f prevNetPos_;    // previous authoritative position
    sf::Vector2f netPos_;        // latest authoritative position
    sf::Vector2f velocity_;      // derived from consecutive authoritative positions
    float SERVER_DT_;   // server update interval
    float interpTimer_;    // time since last authoritative update
    float size_{};  //to draw hp bar
    float animTimer{ 0.f };
    bool hasNet_;
    bool hasPrevNet_;
    bool isFlipped{false};
    int totalFrames_{ -1 };   // decided by type
    unsigned lastTick_{ 0 };
    float frameInterval_{ -1.f }; // time per frame
    unsigned frameWidth_{ 0u };   // decided by type
    unsigned frameHeight_{ 0u };  // decided by type
    constexpr static float ANGLE_UPDATE_SPEED2_THRESHOLD{ 0.3f };
    static constexpr float BAR_WIDTH = 1.f;
    static constexpr float BAR_HEIGHT = 0.02f;
    //HP bar
    sf::RectangleShape hpBarBg_;
    sf::RectangleShape hpBarFill_;
    float hpDisplayTimer_{ 0.f };
    float currentHpPct_{ 1.f };
    constexpr static float HP_SHOW_DURATION{ 4.0f };
    bool hasLight_ { false };
    float lightRadius_ { 0.f };
    EntityTypeID type_{};
    //nameTag
    bool hasNameTag_ = false;
    sf::Text nameText_;
    sf::RectangleShape nameBg_;
    float nameTagYOffset_ = 0.f;
    // hurt flash
    float hurtFlash_ = 0.f;  //current intensity
    float hurtFlashDecay_ = 6.f;  //decreasing rate
    sf::Shader* hurtShader_ { &ResourceManager::getShader("shaders/hurtflash.frag") };
protected:
    sf::Color lightColor_ { 255, 255, 255 };
};

inline void NetworkEntity::setType(EntityTypeID type) {
    sprite_.setTexture(ResourceManager::getTexture(getTexturePath(type)));
    totalFrames_ = getTextureTotalFrame(type);
    frameInterval_ = getFrameInterval(type);
    frameHeight_ = sprite_.getTexture()->getSize().y;
    frameWidth_ = sprite_.getTexture()->getSize().x / totalFrames_;   //assume horizontal strip
    sprite_.setOrigin(static_cast<float>(frameWidth_) / 2.f, static_cast<float>(frameHeight_) / 2.f);
    sprite_.setTextureRect(sf::IntRect(0, 0, frameWidth_, frameHeight_));
    type_ = type;
}

inline void NetworkEntity::setSize(float size) {
    assert(sprite_.getTexture() && "Texture(type) must be set before setting size");
    size_ = size;
    float scale = size / static_cast<float>(sprite_.getTexture()->getSize().x / totalFrames_);
    sprite_.setScale(scale, scale * (isFlipped ? -1.f : 1.f));
    nameTagYOffset_ = size * 0.75f;
}
inline void NetworkEntity::setHpPercentage(float pct) {  //only color
    pct = std::clamp(pct, 0.f, 1.f);
    currentHpPct_ = pct;
    hpDisplayTimer_ = HP_SHOW_DURATION;
    sf::Uint8 r, g, b;
    b = 0;
    if (pct > 0.5f) {
        //Green (0,255,0) -> Yellow (255,255,0)
        float t = (1.f - pct) * 2.f;
        r = static_cast<sf::Uint8>(255.f * t);
        g = 255;
    }
    else {
        //Yellow (255,255,0) -> Red (255,0,0)
        float t = pct * 2.f;
        r = 255;
        g = static_cast<sf::Uint8>(255.f * t);
    }
    hpBarFill_.setFillColor(sf::Color(r, g, b));
    static constexpr sf::Uint8 OUTLINE_OFFSET = 20;
    const auto& colorHandler = [&](sf::Uint8 origin) {
        if (255 - origin <= OUTLINE_OFFSET)
            return static_cast<sf::Uint8>(255);
        else
            return static_cast<sf::Uint8>(origin + OUTLINE_OFFSET);
    };
    hpBarFill_.setOutlineColor(sf::Color(colorHandler(r), colorHandler(g), colorHandler(b)));  //brighter outline
}
inline void NetworkEntity::updateHpBar(float dt) {
    if (hpDisplayTimer_ <= 0.f) return;
    hpDisplayTimer_ -= dt;
    hpBarBg_.setSize({BAR_WIDTH, BAR_HEIGHT});
    hpBarBg_.setOrigin(BAR_WIDTH / 2.f, BAR_HEIGHT / 2.f);
    hpBarFill_.setSize({BAR_WIDTH * currentHpPct_, BAR_HEIGHT});
    hpBarFill_.setOrigin(0.f, BAR_HEIGHT / 2.f);
    sf::Vector2f barPos = clientPos_;
    barPos.y -= size_ * 0.6f;
    hpBarBg_.setPosition(barPos);
    hpBarFill_.setPosition(barPos.x - BAR_WIDTH / 2.f, barPos.y);
}

inline void NetworkEntity::updatePos(float dt) {
    if (!hasNet_) {
        // no server info: simple prediction by last known velocity
        clientPos_ += velocity_ * dt;
        sprite_.setPosition(clientPos_);
        return;
    }
    if (!hasPrevNet_) {
        // only one sample received: snap to it (or could hold/extrapolate)
        clientPos_ = netPos_;
        sprite_.setPosition(clientPos_);
        return;
    }
    interpTimer_ += dt;
    constexpr float ALPHA = 0.4f;  //smoothing factor, less is smoother
    sf::Vector2f target;
    target = prevNetPos_ + (netPos_ - prevNetPos_) * (interpTimer_ / SERVER_DT_);
    // if (interpTimer_ <= SERVER_DT_ * 1.25f) {
    //     target = prevNetPos_ + (netPos_ - prevNetPos_) * (interpTimer_ / SERVER_DT_);
    // }
    // else {
    //     float extra = interpTimer_ - SERVER_DT_;
    //     target = netPos_ + velocity_ * extra;
    // }
    clientPos_ += (target - clientPos_) * ALPHA;
    sprite_.setPosition(clientPos_);
}
inline void NetworkEntity::updateAngle() {
    if (velocity_.x * velocity_.x + velocity_.y * velocity_.y < ANGLE_UPDATE_SPEED2_THRESHOLD) return; //no movement, no rotation
    float angle = std::atan2(velocity_.y, velocity_.x) * 180.f / 3.14159265f;
    float current = sprite_.getRotation();
    float delta = angle - current;
    while (delta > 180.f) delta -= 360.f;
    while (delta < -180.f) delta += 360.f;
    constexpr float alpha = 0.2f;  //less is smoother
    float newRot = current + delta * alpha;
    sprite_.setRotation(newRot);
    bool shouldFlip = newRot >= 90.f && newRot <= 270.f || newRot <= -90.f && newRot >= -270.f;
    if (shouldFlip && !isFlipped) {
        isFlipped = true;
        sf::Vector2f scale = sprite_.getScale();
        scale.y = -abs(scale.y);
        sprite_.setScale(scale);
    } else if (!shouldFlip && isFlipped) {
        isFlipped = false;
        sf::Vector2f scale = sprite_.getScale();
        scale.y = abs(scale.y);
        sprite_.setScale(scale);
    }
}
inline void NetworkEntity::updateAnim(float dt) {
    // update the frame of sprite texture
    animTimer += dt;
    unsigned frame = static_cast<unsigned>(animTimer / frameInterval_) % totalFrames_;
    sprite_.setTextureRect(sf::IntRect(frame * frameWidth_, 0, frameWidth_, frameHeight_));
}
inline void NetworkEntity::setNameTag(const std::string& name) {
    hasNameTag_ = true;
    nameText_.setFont(ResourceManager::getFont("fonts/font6.ttf"));
    nameText_.setString(name);
    nameText_.setCharacterSize(10);
    nameText_.setFillColor(sf::Color::White);
    nameText_.setOutlineThickness(0.f);
    nameBg_.setFillColor(sf::Color(50, 50, 50, 160));
}
#endif // UNDEROCEAN_NETWORKENTITY_H