//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_IRENDERABLE_H
#define UNDEROCEAN_IRENDERABLE_H
namespace sf {
    class RenderWindow;
}
class IRenderable {
public:
    virtual void render(sf::RenderWindow& window) = 0;
    virtual ~IRenderable() = default;
};
#endif //UNDEROCEAN_IRENDERABLE_H