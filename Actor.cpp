#include "Actor.h"
#include "Game.h"
#include "Math.h"

Actor::Actor(Game* game) :
	mGame(game),
	mState(State::EActive),
	mPosition(Vector3::Zero),
	mRotation(Vector3::Zero),
	mScale(Vector3::One) {
	// add itself to active actors using game
	mGame->AddActor(this);
}

Actor::~Actor(){
	// remove itself from actor list
	mGame->RemoveActor(this);
}

void Actor::Update(float deltatime){}

void Actor::UpdateComponents(float deltatime){}

void Actor::UpdateActor(float deltatime){}
