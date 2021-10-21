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

		// Screen wrapping for asteroids
		auto screenX = mOwner->GetGame()->GetWidth() / 2;
		auto screenY = mOwner->GetGame()->GetHeight() / 2;
		if (position.x < -screenX) position.x = screenX;
		else if (position.x > screenX) position.x = -screenX;
		if (position.y < -screenY) position.y = screenY;
		else if (position.y > screenY) position.y = -screenY;
		mOwner->SetActorPosition(position);
	}
}
