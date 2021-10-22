#include "Cube.h"
#include "Math.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Random.h"

Cube::Cube(Game* game) :
	Actor(game) {

	SetActorPosition(Vector3(Random::GetFloatRange(0.f, 50.f), Random::GetFloatRange(0.f, 10.f), Random::GetFloatRange(0.f, 10.f)));
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.f));
	SetActorRotation(q);
	SetActorScale(50.f);

	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh"));
}

Cube::~Cube(){}