#include "Game.h"
#include <SDL.h>
#include <algorithm>
#include "Actor.h"
#include "Random.h"
#include "Cube.h"
#include "CameraActor.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "PlaneActor.h"

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
	Actor* a = new Actor(this);
	a->SetActorPosition(Vector3(200.f, 75.f, 0.f));
	a->SetActorScale(100.f);
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.f));
	a->SetActorRotation(q);
	MeshComponent* mc = new MeshComponent(a);
	mc->SetMesh(mRenderer->GetMesh("Assets/Meshes/Cube.gpmesh"));

	a = new Actor(this);
	a->SetActorPosition(Vector3(200.f, -75.f, 0.f));
	a->SetActorScale(3.0f);
	mc = new MeshComponent(a);
	mc->SetMesh(mRenderer->GetMesh("Assets/Meshes/Sphere.gpmesh"));

	// Setup floor
	const float start = -1250.0f;
	const float size = 250.0f;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			a = new PlaneActor(this);
			a->SetActorPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// Left/right walls
	q = Quaternion(Vector3::UnitX, Math::PiOver2);
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(start + i * size, start - size, 0.0f));
		a->SetActorRotation(q);

		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(start + i * size, -start + size, 0.0f));
		a->SetActorRotation(q);
	}

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	// Forward/back walls
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(start - size, start + i * size, 0.0f));
		a->SetActorRotation(q);

		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(-start + size, start + i * size, 0.0f));
		a->SetActorRotation(q);
	}

	// Setup light
	mRenderer->SetAmbientLight(Vector3(0.5f, 0.5f, 0.5f));
	DirectionaLight& dirLight = mRenderer->GetDirectionalLight();
	dirLight.mDirection = Vector3(0.f, -0.7f, -0.7f);
	dirLight.mDiffuseColor = Color::White;
	dirLight.mSpecularColor = Vector3(1.f, 1.f, 1.f);

	mCameraActor = new CameraActor(this);

	// UI elements
	a = new Actor(this);
	a->SetActorPosition(Vector3(-350.0f, -350.0f, 0.0f));
	SpriteComponent* sc = new SpriteComponent(a);
	sc->SetTexture(mRenderer->GetTexture("Assets/Sprites/HealthBar.png"));

	a = new Actor(this);
	a->SetActorPosition(Vector3(375.0f, -275.0f, 0.0f));
	a->SetActorScale(0.75f);
	sc = new SpriteComponent(a);
	sc->SetTexture(mRenderer->GetTexture("Assets/Sprites/Radar.png"));

}

void Game::UnloadData() {
	// delete any residual actors
	while (!mActors.empty()) delete mActors.back();
}
