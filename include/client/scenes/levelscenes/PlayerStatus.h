//
// Created by 15201 on 12/10/2025.
//

#ifndef UNDEROCEAN_PLAYERSTATUS_H
#define UNDEROCEAN_PLAYERSTATUS_H
class PlayerStatus {  //including HP and FP
public:
    void render(sf::RenderWindow& window) {
        if (!hasWindowRatioInit_) {
            windowRatio = static_cast<float>(window.getSize().x)
                / static_cast<float>(window.getSize().y);
            hasWindowRatioInit_ = true;
        }

    }
    void onWindowSizeChange(unsigned newWidth, unsigned newHeight) {
        windowRatio = static_cast<float>(newWidth) / static_cast<float>(newHeight);
    }
private:
    static constexpr float statusBarWidthRatio = 0.3f;  //relative to window width
    bool hasWindowRatioInit_{false};
    float windowRatio{0.f};
};
#endif //UNDEROCEAN_PLAYERSTATUS_H