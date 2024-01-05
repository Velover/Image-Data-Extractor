#pragma once
#include <iostream>;
#include <string>
#include <fstream>;
#include <sstream>
#include <filesystem>;

extern "C" {
#include "Core/Lua54/include/lua.h"
#include "Core/Lua54/include/lauxlib.h"
#include "Core/Lua54/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "Core/Lua54/lua54.lib")
#endif

#include "BaseTools.h";

lua_State* lua_state = luaL_newstate();

bool CheckLua(lua_State* lua_state, int success) {
	//checks if the function worked successfully
	if (success == LUA_OK) {
		return true;
	}

	std::string error_message = lua_tostring(lua_state, -1);
	Log(error_message);

	return false;
}

bool InitializeLua() {
	luaL_openlibs(lua_state);
	if (CheckLua(lua_state, luaL_dofile(lua_state, "Core/main.lua"))) {
		return true;
	}
	return false;
}

void CloseLua() {
	//finishes the job of lua
	lua_close(lua_state);
}

std::string CompressString(std::string string) {
	lua_getglobal(lua_state, "Compress");
		
	lua_pushstring(lua_state, string.data());
	if (CheckLua(lua_state, lua_pcall(lua_state, 1, 1, 0))) {
		std::string return_string = lua_tostring(lua_state, -1);
		//cleans everything from stack
		lua_pop(lua_state, -1);
		lua_pop(lua_state, -1);
		lua_pop(lua_state, -1);
		return return_string;
	}
	Log("Success");

	return "Error";
}