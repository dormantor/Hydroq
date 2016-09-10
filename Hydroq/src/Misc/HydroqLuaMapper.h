#pragma once

#include "ofUtils.h"
#include "Component.h"

#include <LuaBridge.h>
#include <iostream>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include "PlayerModelLua.h"

using namespace luabridge;
using namespace Cog;
class BehaviorLua;
class NodeLua;

/**
* Component that extends LuaScripting component with new methods
*/
class HydroqLuaMapper : public Component {

public:

	HydroqLuaMapper() {
		this->initPriority = InitPriority::LOW;
	}

	void OnInit();

	virtual void Update(const uint64 delta, const uint64 absolute) {

	}

	/**
	* Gets wrapper of player model
	*/
	static PlayerModelLua HydroqGetPlayerModel();
};
