#pragma once
#include "Actor.h"

class Cube : public Actor {
public:
	Cube(class Game* game);
	~Cube();

	void UpdateActor(float deltatime) override;

private:
	class MoveComponent* mMoveComp;
	class MeshComponent* mMeshComp;
	float mForwardSpeed;
	float mAngularSpeed;
};