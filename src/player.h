#ifndef PLAYER_H
#define PLAYER_H

#include "physicsobject.h"
#include "spritemap.h"

class Player : public PhysicsObject
{
    public:
        Player(SDL_Rect body, int flags, std::string spriteMap);
        Player(SDL_Rect body, int flags, SpriteMap* spriteMap);

        virtual void update(double deltaTime, World& world) override;

    private:
        inline SpriteMap* getTexture() { return static_cast<SpriteMap*>(tex); };

        const float speed = 0.2f;
        const float jump = 0.2f;
};

#endif