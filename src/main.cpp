//
// Created by ty on 4/15/24.
//

// Because Windows.h and raylib.h conflict, instead of using a header file, use a simple function declaration
// and define elsewhere. IIRC as long as it's in the same translation unit, the symbols should match and it should be fine.
// (It works on my machine.)
void start();

#ifdef _WIN32
// On Windows, to build without the pop up console, you need -mwindows and to use WinMain in place of C's main
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    start();
    return 0;
}
#else
// For any normal operating system...
int main(int argc, char* argv[]) {
    start();
    return 0;
}
#endif