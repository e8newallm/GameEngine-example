#ifndef PLAYER_H
#define PLAYER_H

#include "physicsobject.h"
#include "spritemap.h"
#include "texture_base.h"
#include <memory>

using namespace GameEng;

class Player : public PhysicsObject
{
    public:
        Player(SDL_Rect body, int flags, std::string spriteMap);
        Player(SDL_Rect body, int flags, std::shared_ptr<SpriteMap> spriteMap);

        virtual void update(double deltaTime, World& world) override;

    private:
        inline std::shared_ptr<SpriteMap> getTexture() { return std::static_pointer_cast<SpriteMap>(tex); };

        const float speed = 0.2f;
        const float jump = 0.2f;
};

#endif