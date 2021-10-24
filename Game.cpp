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
#include "Sphere.h"

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
	if (!mRenderer->Initialize(mWinWidth, mWinHeight)) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	Random::Init();

	// Load all objects and lights
	LoadData();

	mTicksCount = SDL_GetTicks();

	// Game initialization succeeds
	return true;
}

void Game::ShutDown() {
	UnloadData();
	if (mRenderer) mRenderer->ShutDown();
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
	for (Actor* actor : mActors)
		actor->ProcessInput(keyState);
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
	const int numCubes = 10;
	for (int i = 0; i < numCubes; i++)
		new Cube(this);

	Sphere* sphere = new Sphere(this);

	Actor* a;
	Quaternion q;

	// Setup floor
	const float start = -1250.0f;
	const float size = 250.0f;
	const int wallNum = 10;
	for (int i = 0; i < wallNum; i++)
	{
		for (int j = 0; j < wallNum; j++)
		{
			a = new PlaneActor(this);
			a->SetActorPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// Left/right walls
	q = Quaternion(Vector3::UnitX, Math::PiOver2);
	for (int i = 0; i < wallNum; i++)
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
	for (int i = 0; i < wallNum; i++)
	{
		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(start - size, start + i * size, 0.0f));
		a->SetActorRotation(q);

		a = new PlaneActor(this);
		a->SetActorPosition(Vector3(-start + size, start + i * size, 0.0f));
		a->SetActorRotation(q);
	}

	// Setup light
	mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
	DirectionaLight& dirLight = mRenderer->GetDirectionalLight();
	dirLight.mDirection = Vector3(Random::GetFloatRange(-800.f, 800.f), Random::GetFloatRange(-800.f, 800.f), 10.f);
	dirLight.mDiffuseColor = Vector3(Random::GetFloat(), Random::GetFloat(), Random::GetFloat());
	dirLight.mSpecularColor = Vector3(Random::GetFloat(), Random::GetFloat(), Random::GetFloat());
	PointLight* pointLights = mRenderer->GetPointLights();
	const unsigned int numPointLights = 4;
	for (int i = 0; i < numPointLights; i++) {
		pointLights[i].mPosition = Vector3(Random::GetFloatRange(-800.f, 800.f), Random::GetFloatRange(-800.f, 800.f), 10.f);
		pointLights[i].mDiffuseColor = Vector3(Random::GetFloat(), Random::GetFloat(), Random::GetFloat());
		pointLights[i].mSpecularColor = Vector3(Random::GetFloat(), Random::GetFloat(), Random::GetFloat());;
		pointLights[i].mSpecPower = 1.f;
		pointLights[i].mRadius = 250.f;
	}
	
	mCameraActor = new CameraActor(this);

	// UI elements
	a = new Actor(this);
	SpriteComponent* sc = new SpriteComponent(a);
	Texture* tex = new Texture();
	tex = mRenderer->GetTexture("Assets/HealthBar.png");
	a->SetActorPosition(Vector3(-(mWinWidth / 2) + tex->GetWidth() / 2 + 10.f, -(mWinHeight / 2) + tex->GetHeight() / 2 + 20.f, 0.0f));
	sc->SetTexture(tex);

	a = new Actor(this);
	sc = new SpriteComponent(a);
	tex = new Texture();
	tex = mRenderer->GetTexture("Assets/Radar.png");
	a->SetActorPosition(Vector3((mWinWidth / 2) - (tex->GetWidth() / 2) - 10.f, -(mWinHeight / 2) + (tex->GetHeight() / 2) + 20.f, 0.0f));
	a->SetActorScale(0.75f);
	sc->SetTexture(tex);

}

void Game::UnloadData() {
	// delete any residual actors
	while (!mActors.empty()) delete mActors.back();
	if (mRenderer) mRenderer->UnloadData();
}
