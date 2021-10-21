#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game) :
	mGame(game),
	mState(EActive),
	mPosition(Vector3::Zero),
	mRotation(Quaternion::Identity),
	mScale(1.0f),
	mRecomputeWorldTransform(true){
	// add itself to active actors using game
	mGame->AddActor(this);
}

Actor::~Actor(){
	// remove itself from actor list
	mGame->RemoveActor(this);
	while (!mComponents.empty())
		delete mComponents.back();
}

void Actor::Update(float deltatime){
	if (mState == State::EActive) {
		// compute world trasform matrix
		ComputeWorldTransform();

		UpdateComponents(deltatime);
		UpdateActor(deltatime);

		//Re-compute world trasform in case the actor state change while updating
		ComputeWorldTransform();
	}
}

void Actor::UpdateComponents(float deltatime){
	for (Component* comp : mComponents) {
		comp->Update(deltatime);
	}
}

void Actor::UpdateActor(float deltatime) {}

void Actor::ProcessInput(const uint8_t* keyState) {
	if (mState == EActive) {
		for (Component* comp : mComponents)
			comp->ProcessInput(keyState);
	}
	ProcessActorInput(keyState);
}

void Actor::ProcessActorInput(const uint8_t* keyState){}

void Actor::AddComponent(Component* comp) {
	// find insertion point
	int compOrder = comp->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
		if (compOrder < (*iter)->GetUpdateOrder())
			break;
	mComponents.insert(iter, comp);
}

void Actor::RemoveComponent(Component* comp) {
	auto iter = std::find(mComponents.begin(), mComponents.end(), comp);
	if (iter != mComponents.end()) {
		mComponents.erase(iter);
	}
}

void Actor::ComputeWorldTransform() {
	// We need to recompute world transform?
	if (mRecomputeWorldTransform) {
		mRecomputeWorldTransform = false;
		// Scale -> Rotation -> Translation
		mWorldTransform = Matrix4::CreateScale(mScale);					// Create a uniform scale matrix
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);	// create arotation matrix about z-axis
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);		// create a translation matrix
	}

	// Inform components that world transform updated
	for (auto comp : mComponents) comp->OnUpdateWorldTransform();
}