//
// Created by ty on 4/15/24.
//

#include <raylib.h>

#include "../../include/lib.hpp"

class GameState {
public:
    void count() {
        m_count++;
    }
int m_count = 0;
};

static GameState state; // This *should* be redundant data, but idk lol

// This is the libmain called every window message loop iteration
EXPORT_LIB void libmain(void* args, size_t argc) {
    state.count();

    TraceLog(LOG_INFO, "Count: %d", state.m_count);

}