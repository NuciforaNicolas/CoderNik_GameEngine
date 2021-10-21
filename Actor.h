#pragma once
#include <vector>
#include "Math.h"
#include<cstdint>

class Actor {
public:
	// track the actor state
	enum State {
		EActive,
		EPaused,
		EDead
	};

	// Costructor/Destructor
	Actor(class Game* game);
	virtual ~Actor();

	//Update actor and all components. Called by game (NOT OVERRIDABLE)
	void Update(float deltatime);
	// Update all the actor's components
	void UpdateComponents(float deltatime);
	// Update actor
	virtual void UpdateActor(float deltatime);
	// Process input called by game (not overidable)
	void ProcessInput(const uint8_t* keyState);
	// Process specific actor input (overridable)
	virtual void ProcessActorInput(const uint8_t* keyState);

	//Getters and setters
	State GetActorState() const { return mState; }
	void SetActorState(State state) { mState; }
	Vector3 GetActorPosition() const { return mPosition; }
	void SetActorPosition(const Vector3& position) { mPosition = position; mRecomputeWorldTransform = true; }
	Quaternion GetActorRotation() const { return mRotation; }
	void SetActorRotation(const Quaternion& rotation) { mRotation = rotation; mRecomputeWorldTransform = true; }
	float GetActorScale() const { return mScale; }
	void SetActorScale(const float& scale) { mScale = scale; mRecomputeWorldTransform = true; }
	Vector3 GetForward() const { return Vector3::Transform(Vector3::UnitX, mRotation); }
	class Game* GetGame() const { return mGame; }
	Matrix4 GetWorldTransform() const { return mWorldTransform; }

	// Create world transform matrix
	void ComputeWorldTransform();

	// Add/Remove Components
	void AddComponent(class Component* comp);
	void RemoveComponent(class Component* comp);

protected:
	// Actor's state
	State mState;
	// Transform
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
	// Store the world transformation matrix
	Matrix4 mWorldTransform; // components (x, y, z, w)
	// we need to recalculate world transformation? For example, when actor change position, scale or rotation
	bool mRecomputeWorldTransform;
	// List of Actor's components
	std::vector<class Component*> mComponents;
	// Game pointer. Used to call specific Game functions
	class Game* mGame;
};