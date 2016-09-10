#include "HydroqLuaMapper.h"
#include "ComponentStorage.h"
#include "Definitions.h"
#include "LuaScripting.h"
#include "PlayerModel.h"

void HydroqLuaMapper::OnInit() {
	auto luaScripting = GETCOMPONENT(LuaScripting);
	auto L = luaScripting->GetLua();

	getGlobalNamespace(L)
		.beginClass<PlayerModelLua>("PlayerModel")
		.addFunction("AddRigs", &PlayerModelLua::AddRigs)
		.addFunction("AddUnit", &PlayerModelLua::AddUnit)
		.addFunction("GameEnded", &PlayerModelLua::GameEnded)
		.addFunction("GetFaction", &PlayerModelLua::GetFaction)
		.addFunction("GetMap", &PlayerModelLua::GetMap)
		.addFunction("GetRigs", &PlayerModelLua::GetRigs)
		.addFunction("GetUnits", &PlayerModelLua::GetUnits)
		.addFunction("IsMultiplayer", &PlayerModelLua::IsMultiplayer)
		.addFunction("PlayerWin", &PlayerModelLua::PlayerWin)
		.addFunction("RemoveRigs", &PlayerModelLua::RemoveRigs)
		.addFunction("RemoveUnit", &PlayerModelLua::RemoveUnit)
		.addFunction("SetFaction", &PlayerModelLua::SetFaction)
		.addFunction("SetGameEnded", &PlayerModelLua::SetGameEnded)
		.addFunction("SetPlayerWin", &PlayerModelLua::SetPlayerWin)
		.endClass();

	getGlobalNamespace(L)
		.addFunction("HydroqGetPlayerModel", &HydroqGetPlayerModel);
}

PlayerModelLua HydroqLuaMapper::HydroqGetPlayerModel() {
	auto playerModel = GETCOMPONENT(PlayerModel);
	return PlayerModelLua(playerModel);
}