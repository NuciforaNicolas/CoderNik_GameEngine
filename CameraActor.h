#pragma once
#include "Actor.h"

class CameraActor : public Actor {
public:
	CameraActor(class Game* game);
	
	void UpdateActor(float deltatime) override;
	void ProcessActorInput(const uint8_t* keys) override;
private:
	class MoveComponent* mMoveComp;
};