#include "Cube.h"
#include "Math.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Random.h"
#include "MoveComponent.h"

Cube::Cube(Game* game) :
	Actor(game),
	mForwardSpeed(100.f),
	mAngularSpeed(1.f),
	mMeshComp(nullptr),
	mMoveComp(nullptr)
{

	SetActorPosition(Vector3(Random::GetFloatRange(-500.f, 500.f), Random::GetFloatRange(-500.f, 500.f), Random::GetFloatRange(0.f, 100.f)));
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.f));
	SetActorRotation(q);
	SetActorScale(Random::GetFloatRange(10.f, 100.f));

	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));

	mMoveComp = new MoveComponent(this);
}

Cube::~Cube(){}

void Cube::UpdateActor(float deltatime) {
	Actor::UpdateActor(deltatime);

	mMoveComp->SetForwardSpeed(mForwardSpeed);
	if (mPosition.z >= 150.f)
		mForwardSpeed = -100.f;
	if (mPosition.z < 10.f)
		mForwardSpeed = 100.f;

	mMoveComp->SetAngularSpeed(mAngularSpeed);
}