#include "MoveComponent.h"
#include "Actor.h"
#include "Game.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder) :
	Component(owner, updateOrder),
	mAngularSpeed(0.0f),
	mForwardSpeed(0.0f)
{}

void MoveComponent::Update(float deltatime) {
	if (!Math::NearZero(mAngularSpeed)) {
		Quaternion rotation = mOwner->GetActorRotation();
		float angle = mAngularSpeed * deltatime;
		// Create Quaternion for incremental rotaion. Rotate about Z-axis
		Quaternion inc(Vector3::UnitZ, angle);
		// Concatenate old with new quaternion
		rotation = Quaternion::Concatenate(rotation, inc);
		mOwner->SetActorRotation(rotation);
	}
	if (!Math::NearZero(mForwardSpeed)) {
		Vector3 position = mOwner->GetActorPosition();
		position += mOwner->GetForward() * mForwardSpeed * deltatime;
		mOwner->SetActorPosition(position);
	}
}
