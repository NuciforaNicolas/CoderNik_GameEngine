#include "Asteroid.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Random.h"

Asteroid::Asteroid(Game* game) :
	Actor(game)
{
	auto screenX = game->GetWidth() / 2;
	auto screenY = game->GetHeight() / 2;
	Vector3 pos = Random::GetVector(Vector3(-screenX, -screenY, 0.0f),
		Vector3(screenX, screenY, 0.0f));
	SetActorPosition(pos);

	float rot = Random::GetFloatRange(0.0f, Math::TwoPi);
	SetActorRotation(Quaternion::Identity);

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetRenderer()->GetTexture("Assets/Sprites/Asteroid.png"));

	MoveComponent* mc = new MoveComponent(this);
	//mc->SetAngularSpeed(Random::GetFloatRange(1.f, 10.f));
	mc->SetForwardSpeed(Random::GetFloatRange(50.f, 150.f));

	game->AddAsteroid(this);
}

Asteroid::~Asteroid(){
	mGame->RemoveAsteroid(this);
}