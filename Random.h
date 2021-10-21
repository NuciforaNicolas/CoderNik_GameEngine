#pragma once

#include <random>
#include "Math.h"

class Random {
public:
	static void Init();

	// Seed the random generator
	static void Seed(unsigned int seed);

	// Return a float between 0.0 and 1.0
	static float GetFloat();

	// Return a float between the specified range
	static float GetFloatRange(float min, float max);

	// Get a int between the specified range
	static int GetIntRange(int min, int max);

	// Get a random vector given the min/max bounds
	static Vector2 GetVector(const Vector2& min, const Vector2& max);
	static Vector3 GetVector(const Vector3& min, const Vector3& max);

private:
	static std::mt19937 sGenerator;
};