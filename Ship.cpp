#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include <SDL.h>

Ship::Ship(Game* game) :
	Actor(game)
{
	SetActorPosition(Vector3::Zero);
	SetActorRotation(Quaternion::Identity);
	SetActorScale(1.0f);

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetRenderer()->GetTexture("Assets/Sprites/Ship.png"));
	
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(150.f);
	ic->SetMaxAngularSpeed(10.f);

}

Ship::~Ship(){}