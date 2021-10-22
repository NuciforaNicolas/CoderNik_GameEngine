#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "Renderer.h"

class Game {
public:
	// Constructor
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void ShutDown();
	// Set Window Width and Height
	void SetWindowWidthHeight(int width, int height);
	// add new actor in mActors. Called from Actor costructor
	void AddActor(class Actor* actor);
	// remove actor from mActors. Called from Actor destructor
	void RemoveActor(class Actor* actor);
	// Get window width and height
	int GetWidth() const { return mWinWidth; }
	int GetHeight() const { return mWinHeight; }

	class Renderer* GetRenderer() const { return mRenderer; }

private:
	// Helper function for the game loop. Main Game steps for each frame: Process Inputs, update the game world, generate any output
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	// Load game stuff
	void LoadData();
	// Delete all game's stuff
	void UnloadData();
	
	int mWinWidth, mWinHeight;
	//Game should continue to run?
	bool mIsRunning;
	// Game is currently updating any actor?
	bool mUpdatingActors;
	//Ticks: number of milliseconds elapsed since SDL_Init function call. Used to calculate deltaTime
	Uint32 mTicksCount;
	// list of active actors
	std::vector<class Actor*> mActors;
	// list of pending actor. We need this list to insert into it new actors created while iterating mActors (we can't add item on a list being iterated)
	// after iterating mActors, add all pending actors inside mActors.
	std::vector<class Actor*> mPendingActors;
	// Renderer
	class Renderer* mRenderer;
	// Camera actor
	class CameraActor* mCameraActor;
};
