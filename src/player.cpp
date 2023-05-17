#include <iostream>
#include <algorithm>

#include "player.h"
#include "keystate.h"
#include "spritemap.h"

void Player::update(double deltaTime, PhysicsContext* context)
{
    KeyState& keyState = KeyState::get();

    if(body.y < MAXHEIGHT)
    {
        MAXHEIGHT = body.y;
    }
    
    if(onGround() && keyState[SDL_SCANCODE_SPACE] == SDL_KEYDOWN)
    {
        SDL_FPoint curVel = getVelocity();
        velocity(curVel.x, -jump);
        //((SpriteMap*)tex)->startAnimation("explosion");
    }
    
    if(!((SpriteMap*)tex)->animationRunning())
    {
        ((SpriteMap*)tex)->startAnimation("explosion");
    }
    
    SDL_FPoint vel = getVelocity();
    if(keyState[SDL_SCANCODE_A] == SDL_KEYDOWN)
    {
        velocity(std::min(vel.x, -speed), vel.y);
    }
    else if(keyState[SDL_SCANCODE_D] == SDL_KEYDOWN)
    {
        velocity(std::max(vel.x, speed), vel.y);
    }
    else
    {
        velocity(vel.x/2.0f, vel.y);
    }
    
    PhysicsObject::update(deltaTime, context);
}