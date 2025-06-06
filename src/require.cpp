#include "export.hpp"
#include "Luau/ReplRequirer.h"
#include "Luau/Coverage.h"
constexpr auto context_key = "__REQUIRE_CONTEXT";

// yanked from Repl.h
static auto create_require_context(lua_State* L) -> void*
{
    void* ctx = lua_newuserdatadtor(
        L,
        sizeof(ReplRequirer),
        [](void* ptr)
        {
            static_cast<ReplRequirer*>(ptr)->~ReplRequirer();
        }
    );

    if (!ctx)
        luaL_error(L, "unable to allocate ReplRequirer");

    ctx = new (ctx) ReplRequirer{
        compile_options<Luau::CompileOptions>,
        coverageActive,
        []{return true;},
        coverageTrack,
    };

    // Store ReplRequirer in the registry to keep it alive for the lifetime of
    // this lua_State. Memory address is used as a key to avoid collisions.
    lua_pushstring(L, context_key);
    lua_insert(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    return ctx;
}
void open_require(lua_State* L) {
    luaopen_require(L, requireConfigInit, create_require_context(L));
}
