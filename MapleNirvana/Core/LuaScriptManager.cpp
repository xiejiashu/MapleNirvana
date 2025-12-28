#include "Core/LuaScriptManager.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <system_error>

#ifndef MAPLENIRVANA_USE_LUA
#define MAPLENIRVANA_USE_LUA 0
#endif

#if MAPLENIRVANA_USE_LUA
extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace
{
std::filesystem::path get_exe_dir()
{
    const char *base = SDL_GetBasePath();
    if (!base)
    {
        return std::filesystem::current_path();
    }

    std::filesystem::path p = base;
    return p;
}

std::string to_utf8(const std::filesystem::path &p)
{
#if defined(_WIN32)
    const std::u8string u8 = p.u8string();
    return std::string(u8.begin(), u8.end());
#else
    return p.string();
#endif
}

int push_traceback(lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg)
    {
        luaL_traceback(L, L, msg, 1);
    }
    else
    {
        lua_pushliteral(L, "(error object is not a string)");
    }
    return 1;
}

bool run_chunk_with_traceback(lua_State *L, int status, const char *context)
{
    if (status == LUA_OK)
    {
        return true;
    }

    const char *err = lua_tostring(L, -1);
    if (!err)
    {
        err = "(unknown lua error)";
    }

    SDL_Log("[Lua] %s failed: %s", context ? context : "chunk", err);
    lua_pop(L, 1);
    return false;
}

void append_package_path(lua_State *L, const std::filesystem::path &scriptRoot)
{
    lua_getglobal(L, "package");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return;
    }

    lua_getfield(L, -1, "path");
    const char *cur = lua_tostring(L, -1);

    std::string newPath = cur ? cur : "";

    const auto root = to_utf8(scriptRoot);
    if (!newPath.empty() && newPath.back() != ';')
    {
        newPath.push_back(';');
    }

    // <root>/?.lua and <root>/?/init.lua
    newPath += root;
    if (!newPath.empty() && newPath.back() != '/' && newPath.back() != '\\')
    {
        newPath.push_back('/');
    }
    newPath += "?.lua;";

    newPath += root;
    if (!newPath.empty() && newPath.back() != '/' && newPath.back() != '\\')
    {
        newPath.push_back('/');
    }
    newPath += "?/init.lua";

    lua_pop(L, 1); // old path
    lua_pushlstring(L, newPath.c_str(), newPath.size());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1); // package
}

// -------- Example bindings --------

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

constexpr const char *kVec2Meta = "MapleNirvana.Vec2";

Vec2 *check_vec2(lua_State *L, int idx)
{
    return static_cast<Vec2 *>(luaL_checkudata(L, idx, kVec2Meta));
}

int vec2_new(lua_State *L)
{
    float x = static_cast<float>(luaL_optnumber(L, 1, 0.0));
    float y = static_cast<float>(luaL_optnumber(L, 2, 0.0));

    void *mem = lua_newuserdatauv(L, sizeof(Vec2), 0);
    new (mem) Vec2{ x, y };

    luaL_getmetatable(L, kVec2Meta);
    lua_setmetatable(L, -2);
    return 1;
}

int vec2_gc(lua_State *L)
{
    auto *v = check_vec2(L, 1);
    v->~Vec2();
    return 0;
}

int vec2_tostring(lua_State *L)
{
    const auto *v = check_vec2(L, 1);
    char buf[128];
    SDL_snprintf(buf, sizeof(buf), "Vec2(%.3f, %.3f)", v->x, v->y);
    lua_pushstring(L, buf);
    return 1;
}

int vec2_length(lua_State *L)
{
    const auto *v = check_vec2(L, 1);
    const float len = SDL_sqrtf(v->x * v->x + v->y * v->y);
    lua_pushnumber(L, len);
    return 1;
}

int vec2_add(lua_State *L)
{
    const auto *a = check_vec2(L, 1);
    const auto *b = check_vec2(L, 2);

    void *mem = lua_newuserdatauv(L, sizeof(Vec2), 0);
    new (mem) Vec2{ a->x + b->x, a->y + b->y };

    luaL_getmetatable(L, kVec2Meta);
    lua_setmetatable(L, -2);
    return 1;
}

int vec2_index(lua_State *L)
{
    auto *v = check_vec2(L, 1);
    const char *key = luaL_checkstring(L, 2);

    if (SDL_strcmp(key, "x") == 0)
    {
        lua_pushnumber(L, v->x);
        return 1;
    }
    if (SDL_strcmp(key, "y") == 0)
    {
        lua_pushnumber(L, v->y);
        return 1;
    }

    // Methods
    luaL_getmetatable(L, kVec2Meta);
    lua_getfield(L, -1, key);
    return 1;
}

int vec2_newindex(lua_State *L)
{
    auto *v = check_vec2(L, 1);
    const char *key = luaL_checkstring(L, 2);

    if (SDL_strcmp(key, "x") == 0)
    {
        v->x = static_cast<float>(luaL_checknumber(L, 3));
        return 0;
    }
    if (SDL_strcmp(key, "y") == 0)
    {
        v->y = static_cast<float>(luaL_checknumber(L, 3));
        return 0;
    }

    return luaL_error(L, "Vec2 has no field '%s'", key);
}

int engine_log(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    SDL_Log("[Lua] %s", msg);
    return 0;
}

void register_vec2(lua_State *L)
{
    if (luaL_newmetatable(L, kVec2Meta))
    {
        lua_pushcfunction(L, vec2_gc);
        lua_setfield(L, -2, "__gc");

        lua_pushcfunction(L, vec2_tostring);
        lua_setfield(L, -2, "__tostring");

        lua_pushcfunction(L, vec2_index);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, vec2_newindex);
        lua_setfield(L, -2, "__newindex");

        lua_pushcfunction(L, vec2_length);
        lua_setfield(L, -2, "length");

        lua_pushcfunction(L, vec2_add);
        lua_setfield(L, -2, "add");
    }
    lua_pop(L, 1); // metatable

    // Engine.Vec2 = { new = vec2_new }
    lua_getglobal(L, "Engine");
    lua_newtable(L);
    lua_pushcfunction(L, vec2_new);
    lua_setfield(L, -2, "new");
    lua_setfield(L, -2, "Vec2");
    lua_pop(L, 1);
}

void register_engine(lua_State *L, const std::filesystem::path &scriptRoot)
{
    lua_newtable(L);

    lua_pushcfunction(L, engine_log);
    lua_setfield(L, -2, "log");

    const auto root = to_utf8(scriptRoot);
    lua_pushlstring(L, root.c_str(), root.size());
    lua_setfield(L, -2, "script_root");

    lua_setglobal(L, "Engine");

    register_vec2(L);
}
} // namespace

#endif // MAPLENIRVANA_USE_LUA

LuaScriptManager &LuaScriptManager::instance()
{
    static LuaScriptManager inst;
    return inst;
}

bool LuaScriptManager::init(std::filesystem::path scriptRoot)
{
#if !MAPLENIRVANA_USE_LUA
    (void)scriptRoot;
    SDL_Log("[Lua] MAPLENIRVANA_USE_LUA=0, scripting disabled");
    return false;
#else
    shutdown();

    if (scriptRoot.empty())
    {
        scriptRoot_ = get_exe_dir() / "Script";
    }
    else
    {
        scriptRoot_ = std::move(scriptRoot);
    }

    L_ = luaL_newstate();
    if (!L_)
    {
        SDL_Log("[Lua] Failed to create lua_State");
        return false;
    }

    luaL_openlibs(L_);
    append_package_path(L_, scriptRoot_);
    register_engine(L_, scriptRoot_);

    SDL_Log("[Lua] Initialized. Script root: %s", to_utf8(scriptRoot_).c_str());
    return true;
#endif
}

void LuaScriptManager::shutdown()
{
#if MAPLENIRVANA_USE_LUA
    if (L_)
    {
        lua_close(L_);
        L_ = nullptr;
    }
#endif
}

bool LuaScriptManager::ensureInitialized_()
{
    if (L_)
    {
        return true;
    }

    return init({});
}

bool LuaScriptManager::doFile(const std::string &relativePath)
{
#if !MAPLENIRVANA_USE_LUA
    (void)relativePath;
    return false;
#else
    if (!ensureInitialized_())
    {
        return false;
    }

    const std::filesystem::path full = scriptRoot_ / relativePath;

    lua_pushcfunction(L_, push_traceback);
    const int errFuncIndex = lua_gettop(L_);

    const int loadStatus = luaL_loadfile(L_, to_utf8(full).c_str());
    if (loadStatus != LUA_OK)
    {
        lua_remove(L_, errFuncIndex);
        return run_chunk_with_traceback(L_, loadStatus, "luaL_loadfile");
    }

    const int callStatus = lua_pcall(L_, 0, LUA_MULTRET, errFuncIndex);
    lua_remove(L_, errFuncIndex);
    return run_chunk_with_traceback(L_, callStatus, "lua_pcall");
#endif
}

bool LuaScriptManager::doFileIfExists(const std::string &relativePath)
{
#if !MAPLENIRVANA_USE_LUA
    (void)relativePath;
    return false;
#else
    const std::filesystem::path full = scriptRoot_ / relativePath;
    std::error_code ec;
    if (!std::filesystem::exists(full, ec))
    {
        return false;
    }
    return doFile(relativePath);
#endif
}

lua_State *LuaScriptManager::state()
{
    return L_;
}

const std::filesystem::path &LuaScriptManager::scriptRoot() const
{
    return scriptRoot_;
}
