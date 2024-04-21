//
// Created by ty on 4/15/24.
//

#include <iostream>
#include <chrono>
#include <utility>
#include <raylib.h>

#include "hotreloadlibrary.hpp"

void start() {
    // Create window and init OpenGL context
    InitWindow(1280, 720, "Video Game :)");

#ifndef NDEBUG // Build for Debug
    SetTraceLogLevel(LOG_DEBUG);
#endif

    // Cache: Last time checked for DLL/SO updates
    std::chrono::time_point<std::chrono::system_clock> lastUpdated = std::chrono::system_clock::now();

    // HotReloadLibrary instance (see hotreloadlibrary.hpp)
    HotReloadLibrary mainLib("Game/libgamelib.so");
    TraceLog(LOG_INFO, "LIBRARY: Library loaded successfully (%s)", mainLib.fileName().c_str());

    while(!WindowShouldClose()) {
        // START WINDOW MESSAGE LOOP
        BeginDrawing();
        ClearBackground(WHITE);
        try {
            // Note, for arguments, you can do this:
            // const size_t argumentCount = 4;
            // void* arguments[argumentCount] = { &object1, &object2, &object3, ... };
            // mainLib.call(arguments, argumentCount);
            // But you will have to hardcode the indices in "Game" (gamelib)
            // This can be done for fully redundant data (otherwise should be static)
            mainLib.call(nullptr, 0U); // call "libmain" symbol/function
        } catch (const std::exception& e) {
            TraceLog(LOG_ERROR, "%s", e.what());
        }
        EndDrawing();
        // END WINDOW MESSAGE LOOP

        // Every 200 or so milliseconds, check if the library can be updated
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdated).count() > 200U) {
            if(mainLib.canUpdate()) {
                TraceLog(LOG_DEBUG, "LIBRARY: Hot reloading library (%s)", mainLib.fileName().c_str());
                switch(mainLib.update()) {
                    case HotReloadLibrary::UpdateReturnCodes::UPDATE_NOT_REQUIRED:
                        // File currently unavailable or has not been written to
                        break;
                    case HotReloadLibrary::UpdateReturnCodes::UPDATE_SUCCESSFUL:
                        // Everything went swimmingly
                        TraceLog(LOG_INFO, "LIBRARY: Library hot reloaded successfully (%s)", mainLib.fileName().c_str());
                    break;
                    case HotReloadLibrary::UpdateReturnCodes::UPDATE_NO_LIBMAIN:
                        // The library was loaded but no "libmain" symbol (function) was found.
                            // If you have it defined, make sure you export it with EXPORT_LIB define
                        TraceLog(LOG_ERROR, "LIBRARY: Library missing libmain(void*, size_t) (%s)", mainLib.fileName().c_str());
                    break;
                    case HotReloadLibrary::UpdateReturnCodes::UPDATE_SAME_LIBMAIN:
                        // The library was loaded and a "libmain" symbol (function) was found, but for some reason it's the same address as the previous one
                            // Not a direct cause for worry, but it's not expected behavior
                        TraceLog(LOG_WARNING, "LIBRARY: Library libmain(void*, size_t) is the same after hot reload (%s)", mainLib.fileName().c_str());
                    break;
                    default:
                        // Covered all conditions, should never reach this point
                        std::unreachable();
                }
            }

            // Update the time cache
            lastUpdated = std::chrono::system_clock::now();
        }
    }

    // Clean up
    CloseWindow();
}
