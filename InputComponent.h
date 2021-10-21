#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent {
public:
	InputComponent(class Actor* owner);

	void ProcessInput(const uint8_t* keyState) override;

	// Getter and Setter
	void SetMaxForwardSpeed(float maxForwardSpeed) { mMaxForwardSpeed = maxForwardSpeed; }
	float GetMaxForwardSpeed() const { return mMaxForwardSpeed; }
	void SetMaxAngularSpeed(float maxAngularSpeed) { mMaxAngularSpeed = maxAngularSpeed; }
	float GetMaxAngularSpeed() const { return mMaxAngularSpeed; }
	void SetForwardKey(int key) { mForwardKey = key; }
	int GetForwardKey() const { return mForwardKey; }
	void SetBackKey(int key) { mBackKey = key; }
	int GetBackKey() const { return mBackKey; }
	int GetClockwiseKey() const { return mClockwiseKey; }
	void SetClockwiseKey(int key) { mClockwiseKey = key; }
	int GetCounterClockwiseKey() const { return mCounterClockwiseKey; }
	void SetCounterClockwiseKey(int key) { mCounterClockwiseKey = key; }

private:
	// Maximum forward and angular speed
	float mMaxForwardSpeed, mMaxAngularSpeed;
	// Keys for forward/back movement
	int mForwardKey, mBackKey;
	// Keys for angular movement
	int mClockwiseKey, mCounterClockwiseKey;

};
