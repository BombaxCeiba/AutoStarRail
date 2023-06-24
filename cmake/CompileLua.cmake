cmake_minimum_required(VERSION 3.6)

project(AsrLua544 LANGUAGES C)

# message(${PROJECT_NAME})
aux_source_directory(. ALL_SRC)

# remove the lua.c/luac.c from teh all file lists
set(LIB_SRC ${ALL_SRC})
list(REMOVE_ITEM LIB_SRC lua.c luac.c)

add_compile_definitions(-DLUA_COMPAT_5_3)

set(ASR_LUA_TARGET AsrLua546)
add_library(${ASR_LUA_TARGET} ${LIB_SRC})
target_include_directories(${ASR_LUA_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(Asr::Lua ALIAS ${ASR_LUA_TARGET})

# ADD_EXECUTABLE(luaexec lua.c)
# target_link_libraries(luaexec lua)
# set_target_properties(luaexec PROPERTIES OUTPUT_NAME lua)

# add_executable(luac luac.c)
# target_link_libraries(luac lua)
