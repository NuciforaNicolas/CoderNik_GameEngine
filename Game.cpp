#include <vector>
#include "Game.h"
#include "Actor.h"
#include "SpriteComponent.h"

#define WINWIDTH 1280
#define WINHEIGHT 720

Game::Game() : 
	mWindow(nullptr),
	mRenderer(nullptr),
	mWinHeight(0),
	mWinWidth(0),
	mTicksCount(0),
	mIsRunning(true){}

void Game::SetWindowWidthHeight(int width, int height) {
	mWinWidth = width;
	mWinHeight = height;
}

bool Game::Initialize() {
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create a window
	mWindow = SDL_CreateWindow("CoderNik Game Engine", 100, 100, mWinWidth != 0 ? mWinWidth : WINWIDTH, mWinHeight != 0 ? mWinHeight : WINHEIGHT, SDL_WINDOW_OPENGL);
	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Initialize Renderer. Use GPU to render graphic and use VSYNC
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED /*Use graphic card*/ | SDL_RENDERER_PRESENTVSYNC /*Use VSync*/);
	if (!mRenderer) {
		SDL_Log("Error on creating Renderer: %s", SDL_GetError());
		return false;
	}

	// Game initialization succeeds
	return true;
}

void Game::ShutDown() {
	UnloadData();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
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
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If player press Escape key, close the game
	if (state[SDL_SCANCODE_ESCAPE]) mIsRunning = false;
}

void Game::UpdateGame() {
	// wait until 16ms has elapsed since last frame. Guarantee fixed 60FPS
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	// calculate deltatime in milliseconds
	float deltatime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// update tick counts
	mTicksCount = SDL_GetTicks();
	// clamp deltatime to fixed value
	if (deltatime > 0.05f) deltatime = 0.05f;

	// update all active actors
	for(auto actor : mActors){
		actor->Update(deltatime);
	}
	// put pending actors in active actors
	for (auto actor : mPendingActors) {
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
	// Set the back buffer color
	SDL_SetRenderDrawColor(
		mRenderer,
		255,
		0,
		120,
		255
	);
	// Clear the back buffer
	SDL_RenderClear(mRenderer);

	// Swap back and front buffer
	SDL_RenderPresent(mRenderer);

}

void Game::AddActor(Actor* actor) {
	// if updating actors, then add to pending actors
	if (!mUpdatingActors)
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
		return;
	}

	// if not in pending actors, check if in active actors
	it = std::find(mActors.begin(), mActors.end(), actor);
	if (it != mActors.end()) {
		std::iter_swap(it, mActors.end() - 1);
		mActors.pop_back();
		return;
	}
}

void Game::UnloadData() {
	// delete any residual actors
	while (!mActors.empty()) delete mActors.back();
}

void Game::AddSprite(SpriteComponent* sprite) {
	//Find an insertion point in the sorted vector
	//The vector is already sorted befor inserting new sprite. We can use insertion sort to preseres order
	int spriteOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); iter++) {
		if (spriteOrder < (*iter)->GetDrawOrder()) break;
	}
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite) {
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end()) {
		std::iter_swap(iter, mSprites.end() - 1);
		mSprites.pop_back();
	}
}