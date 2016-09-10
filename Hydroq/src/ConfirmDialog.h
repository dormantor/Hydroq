#pragma once

#include "Behavior.h"
using namespace Cog;

/**
* Behavior for yes-no confirmation dialog
*/
class ConfirmDialog : public Behavior {
	
public:
	ConfirmDialog() {

	}

	void OnInit();

	void OnMessage(Msg& msg);

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};