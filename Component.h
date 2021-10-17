#pragma once

class Component {
public:
	// Costructor. Specifies the actor who the component belongs and the order which the component will be updated
	// the lower the update component, the earlier will be updated
	Component(class Actor* owner, int updateOrder = 100);
	// destructor
	virtual ~Component();
	// Update the component by deltatime
	virtual void Update(float deltatime);

	int GetUpdateOrder() const { return mUpdateOrder; }

protected:
	// update order of component
	int mUpdateOrder;
	// Owning Actor
	Actor* mOwner;
};