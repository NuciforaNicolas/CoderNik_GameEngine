#include "Actor.h"
#include "Component.h"

Component::Component(Actor* owner, int updateOrder) :
	mOwner(owner){}

Component::~Component(){}

void Component::Update(float deltatime){}