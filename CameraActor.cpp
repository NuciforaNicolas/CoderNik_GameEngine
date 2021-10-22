#include "CameraActor.h"
#include "Game.h"
#include "MoveComponent.h"
#include "Math.h"

CameraActor::CameraActor(Game* game) :
	Actor(game) {
	mMoveComp = new MoveComponent(this);
}

void CameraActor::UpdateActor(float deltatime) {
	Actor::UpdateActor(deltatime);

	// Compute new camera from this actor
	Vector3 cameraPos = GetActorPosition();
	Vector3 targetPos = GetActorPosition() + GetForward() * 100.f;
	Vector3 up = Vector3::UnitZ;

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, targetPos, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraActor::ProcessActorInput(const uint8_t* keys) {
	float angularSpeed = 0.f;
	float forwardSpeed = 0.f;
	if (keys[SDL_SCANCODE_W]) {
		forwardSpeed += 300.f;
	}
	if (keys[SDL_SCANCODE_S]) {
		forwardSpeed -= 300.f;
	}
	if (keys[SDL_SCANCODE_A]) {
		angularSpeed -= Math::TwoPi;
	}
	if (keys[SDL_SCANCODE_D]) {
		angularSpeed += Math::TwoPi;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}