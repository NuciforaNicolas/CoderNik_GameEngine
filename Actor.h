#pragma once
#include <vector>
#include "Game.h"
#include "Math.h"

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

	//Getters and setters
	State GetActorState() const { return mState; }
	void SetActorState(State state) { mState; }
	Vector3 GetActorPosition() const { return mPosition; }
	void SetActorPosition(const Vector3& position){ mPosition = position; }
	Vector3 GetActorRotation() const { return mRotation; }
	void SetActorRotation(const Vector3& rotation){ mRotation = rotation; }
	Vector3 GetActorScale() const { return mScale; }
	void SetActorScale(const Vector3& scale) { mScale = scale; }
	Game* GetGame() const { return mGame; }

private:
	// Actor's state
	State mState;
	// Transform
	Vector3 mPosition;
	Vector3 mRotation;
	Vector3 mScale;

	// List of Actor's components
	std::vector<class Component*> mComponents;
	// Game pointer. Used to call specific Game functions
	Game* mGame;
};