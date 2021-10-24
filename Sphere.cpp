#include "Sphere.h"
#include "Game.h"
#include "MeshComponent.h"
#include "InputComponent.h"

Sphere::Sphere(Game* game) :
	Actor(game)
{

	SetActorPosition(Vector3(200.f, -75.f, 0.f));
	SetActorRotation(Quaternion::Identity);
	SetActorScale(3.0f);
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));

	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_UP);
	ic->SetBackKey(SDL_SCANCODE_DOWN);
	ic->SetClockwiseKey(SDL_SCANCODE_RIGHT);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_LEFT);
	ic->SetMaxForwardSpeed(100.f);
	ic->SetMaxAngularSpeed(5.f);
}

Sphere::~Sphere(){}