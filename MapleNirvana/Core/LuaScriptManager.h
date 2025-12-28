#pragma once

#include <filesystem>
#include <string>

struct lua_State;

class LuaScriptManager
{
public:
    static LuaScriptManager &instance();

    // If scriptRoot is empty, defaults to: <exeDir>/Script
    bool init(std::filesystem::path scriptRoot = {});
    void shutdown();

    bool doFile(const std::string &relativePath);
    bool doFileIfExists(const std::string &relativePath);

    lua_State *state();
    const std::filesystem::path &scriptRoot() const;

private:
    LuaScriptManager() = default;
    ~LuaScriptManager() = default;
    LuaScriptManager(const LuaScriptManager &) = delete;
    LuaScriptManager &operator=(const LuaScriptManager &) = delete;

    bool ensureInitialized_();

    lua_State *L_ = nullptr;
    std::filesystem::path scriptRoot_;
};
