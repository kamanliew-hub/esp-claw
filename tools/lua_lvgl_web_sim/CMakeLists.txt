cmake_minimum_required(VERSION 3.12)
project (lv_emscripten)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s USE_SDL=2")
get_filename_component(ESP_CLAW_ROOT "${PROJECT_SOURCE_DIR}/../.." ABSOLUTE)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(lvgl)
file(GLOB MY_SOURCES "./*.c")
set(SOURCES ${MY_SOURCES})

add_executable(index ${SOURCES} ${INCLUDES})

if(NOT LVGL_CHOSEN_DEMO)
    set(LVGL_CHOSEN_DEMO lv_demo_widgets)
endif()
set_source_files_properties(main.c PROPERTIES COMPILE_FLAGS -DCHOSEN_DEMO=${LVGL_CHOSEN_DEMO})

set(CMAKE_EXECUTABLE_SUFFIX ".html")
target_link_libraries(index
    lvgl
    lvgl_examples
    lvgl_demos
    lvgl_thorvg
    SDL2
)
set_target_properties(index PROPERTIES LINK_FLAGS "--shell-file ${PROJECT_SOURCE_DIR}/lvgl_shell.html -s INITIAL_MEMORY=33554432")

set(LUA_SRC_DIR ${PROJECT_SOURCE_DIR}/third_party/lua-5.4.8/src)
set(LUA_SOURCES
    ${LUA_SRC_DIR}/lapi.c
    ${LUA_SRC_DIR}/lcode.c
    ${LUA_SRC_DIR}/lctype.c
    ${LUA_SRC_DIR}/ldebug.c
    ${LUA_SRC_DIR}/ldo.c
    ${LUA_SRC_DIR}/ldump.c
    ${LUA_SRC_DIR}/lfunc.c
    ${LUA_SRC_DIR}/lgc.c
    ${LUA_SRC_DIR}/llex.c
    ${LUA_SRC_DIR}/lmem.c
    ${LUA_SRC_DIR}/lobject.c
    ${LUA_SRC_DIR}/lopcodes.c
    ${LUA_SRC_DIR}/lparser.c
    ${LUA_SRC_DIR}/lstate.c
    ${LUA_SRC_DIR}/lstring.c
    ${LUA_SRC_DIR}/ltable.c
    ${LUA_SRC_DIR}/ltm.c
    ${LUA_SRC_DIR}/lundump.c
    ${LUA_SRC_DIR}/lvm.c
    ${LUA_SRC_DIR}/lzio.c
    ${LUA_SRC_DIR}/lauxlib.c
    ${LUA_SRC_DIR}/lbaselib.c
    ${LUA_SRC_DIR}/lcorolib.c
    ${LUA_SRC_DIR}/ldblib.c
    ${LUA_SRC_DIR}/liolib.c
    ${LUA_SRC_DIR}/lmathlib.c
    ${LUA_SRC_DIR}/loadlib.c
    ${LUA_SRC_DIR}/loslib.c
    ${LUA_SRC_DIR}/lstrlib.c
    ${LUA_SRC_DIR}/ltablib.c
    ${LUA_SRC_DIR}/lutf8lib.c
    ${LUA_SRC_DIR}/linit.c
)

set(ESP_CLAW_SIM_SOURCES
    sim/esp_claw_sim.c
    sim/sim_esp_compat.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_module_lvgl.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_core_widgets.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_complex_widgets.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_demos.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_events.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_font.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_extra_widgets.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_indev.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_layout.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_methods.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_object.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_parse.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_runtime.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_style.c
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src/lua_lvgl_value.c
    ${LUA_SOURCES}
)

set(ESP_CLAW_SIM_COMMON_LINK_FLAGS "-O3 --shell-file ${PROJECT_SOURCE_DIR}/esp_claw_shell.html -s ASYNCIFY=1 -s ASYNCIFY_STACK_SIZE=262144 -s EXPORTED_RUNTIME_METHODS=['ccall','FS']")
set(ESP_CLAW_SIM_PRELOAD_FLAGS "--preload-file ${PROJECT_SOURCE_DIR}/sim/lvgl_web_shim.lua@/sim/lvgl_web_shim.lua --preload-file ${PROJECT_SOURCE_DIR}/sim_assets/fonts@/fonts --preload-file ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_display/test@/scripts/display --preload-file ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/test@/scripts/lvgl")
set(ESP_CLAW_SIM_EMBED_FLAGS "--embed-file ${PROJECT_SOURCE_DIR}/sim/lvgl_web_shim.lua@/sim/lvgl_web_shim.lua --embed-file ${PROJECT_SOURCE_DIR}/sim_assets/fonts@/fonts --embed-file ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_display/test@/scripts/display --embed-file ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/test@/scripts/lvgl")

function(configure_esp_claw_sim_target target_name)
target_include_directories(${target_name} PRIVATE
    ${LUA_SRC_DIR}
    ${PROJECT_SOURCE_DIR}/sim/compat
    ${PROJECT_SOURCE_DIR}/lvgl
    ${ESP_CLAW_ROOT}/components/lua_modules/lua_module_lvgl/src
)
target_compile_definitions(${target_name} PRIVATE LUA_USE_C89)
target_compile_options(${target_name} PRIVATE -O3)
target_link_libraries(${target_name}
    lvgl
    lvgl_examples
    lvgl_demos
    lvgl_thorvg
)
endfunction()

add_executable(esp_claw_sim ${ESP_CLAW_SIM_SOURCES})
configure_esp_claw_sim_target(esp_claw_sim)
set_target_properties(esp_claw_sim PROPERTIES
    SUFFIX ".html"
    LINK_FLAGS "${ESP_CLAW_SIM_COMMON_LINK_FLAGS} ${ESP_CLAW_SIM_PRELOAD_FLAGS}"
)

add_executable(esp_claw_sim_release ${ESP_CLAW_SIM_SOURCES})
configure_esp_claw_sim_target(esp_claw_sim_release)
set_target_properties(esp_claw_sim_release PROPERTIES
    SUFFIX ".html"
    LINK_FLAGS "${ESP_CLAW_SIM_COMMON_LINK_FLAGS} ${ESP_CLAW_SIM_EMBED_FLAGS} -s SINGLE_FILE=1"
)
