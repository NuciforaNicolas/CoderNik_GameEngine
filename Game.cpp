#include "Game.h"
#include <SDL.h>
#include <algorithm>
#include "Actor.h"
#include "Asteroid.h"
#include "Random.h"
#include "Ship.h"

Game::Game() : 
	mWinHeight(0),
	mWinWidth(0),
	mTicksCount(0),
	mIsRunning(true),
	mUpdatingActors(false),
	mRenderer(nullptr)
{}

void Game::SetWindowWidthHeight(int width, int height) {
	mWinWidth = width;
	mWinHeight = height;
}

bool Game::Initialize() {
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	mRenderer->Initialize(mWinWidth, mWinHeight);

	LoadData();

	mTicksCount = SDL_GetTicks();

	// Game initialization succeeds
	return true;
}

void Game::ShutDown() {
	UnloadData();
	SDL_Quit();
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	// Store a input event
	SDL_Event event;
	// iterate the event's queue and manage them
	while (SDL_PollEvent(&event)) {
		// Check the type of event
		switch (event.type)
		{
			// The player close the window
			case SDL_QUIT : 
				mIsRunning = false;
				break;
		}
	}

	// Store the keyboard input by the player
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	// If player press Escape key, close the game
	if (keyState[SDL_SCANCODE_ESCAPE]) mIsRunning = false;
	
	// Pass the key pressed to all actor. If one of them have this key as input, process it
	mUpdatingActors = true;
	for (Actor* actor : mActors)
		actor->ProcessInput(keyState);
	mUpdatingActors = false;
}

void Game::UpdateGame() {
	// wait until 16ms has elapsed since last frame. Guarantee fixed 60FPS
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	// calculate deltatime in milliseconds
	float deltatime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// clamp deltatime to fixed value
	if (deltatime > 0.05f) deltatime = 0.05f;

	// update tick counts
	mTicksCount = SDL_GetTicks();

	// update all active actors
	mUpdatingActors = true;
	for(auto actor : mActors){
		actor->Update(deltatime);
	}
	mUpdatingActors = false;

	// put pending actors in active actors
	for (auto actor : mPendingActors) {
		actor->ComputeWorldTransform();
		mActors.emplace_back(actor);
	}
	//clear pending actor list
	mPendingActors.clear();

	// if any dead actor, put them in a temp vector and delete them
	std::vector<Actor*> deadActors;
	for (auto actor : mActors) {
		if (actor->GetActorState() == Actor::EDead) {
			deadActors.emplace_back(actor);
		}
	}
	// delete all dead actors
	for (auto actor : deadActors) delete actor;
}

void Game::GenerateOutput() {
	mRenderer->Draw();
}

void Game::AddActor(Actor* actor) {
	// if updating actors, then add to pending actors
	if (mUpdatingActors)
		mPendingActors.emplace_back(actor);
	else
		mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor) {
	// first, check if actor is in pending actors list
	auto it = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (it != mPendingActors.end()) {
		// put the actor at the end of the list
		std::iter_swap(it, mPendingActors.end() - 1);
		//pop actor from the back of the vector
		mPendingActors.pop_back();
	}

	// if not in pending actors, check if in active actors
	it = std::find(mActors.begin(), mActors.end(), actor);
	if (it != mActors.end()) {
		std::iter_swap(it, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::LoadData() {
	Ship* ship = new Ship(this);

	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
		new Asteroid(this);
}

void Game::UnloadData() {
	// delete any residual actors
	while (!mActors.empty()) delete mActors.back();
}

void Game::AddAsteroid(Asteroid* ast) {
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast) {
	auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
		mAsteroids.erase(iter);
}
