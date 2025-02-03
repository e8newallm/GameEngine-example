#include "player.h"
#include "keystate.h"
#include "spritemap.h"
#include "world.h"

Player::Player(SDL_Rect body, int flags, std::string spriteMap)
{
    Player(body, flags, new SpriteMap(SpriteMap::get(spriteMap)));
};

Player::Player(SDL_Rect body, int flags, SpriteMap* spriteMap) :
PhysicsObject(body, flags, spriteMap)
{
    getTexture()->setSprite("sprite01");
};

void Player::update(double deltaTime, World& world)
{
    if(onGround(world) && KeyState::key(SDL_SCANCODE_SPACE) == SDL_EVENT_KEY_DOWN)
    {
        SDL_FPoint curVel = getVelocity();
        velocity(curVel.x, -jump);
    }

    if(!getTexture()->animationRunning())
    {
        getTexture()->startAnimation("explosion");
    }
    
    SDL_FPoint vel = getVelocity();
    if(KeyState::key(SDL_SCANCODE_A) == SDL_EVENT_KEY_DOWN)
    {
        velocity(std::min(vel.x, -speed), vel.y);
    }
    else if(KeyState::key(SDL_SCANCODE_D) == SDL_EVENT_KEY_DOWN)
    {
        velocity(std::max(vel.x, speed), vel.y);
    }
    else
    {
        velocity(vel.x/2.0f, vel.y);
    }
    
    PhysicsObject::update(deltaTime, world);
}