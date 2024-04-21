//
// Created by ty on 4/15/24.
//

#ifndef HOTRELOADLIBRARY_HPP
#define HOTRELOADLIBRARY_HPP

#include <string>
#include <filesystem>

#ifndef _WIN32
#include <dlfcn.h> // dynamic library functions
typedef void* libhandle_t; // alt. #define LIBHANDLE void* (just a type to hold a handle)
#define libopen(lib) dlopen(lib, RTLD_LAZY | RTLD_LOCAL) // open a library
#define libbind(lib, fn) dlsym(lib, fn) // get a function address
#define libexit(lib) dlclose(lib) // close a library
#else // Note: Windows has NOT been tested (yet)
#include <Windows.h>
typedef HMODULE libhandle_t;
#define libopen(lib) LoadLibraryA(lib)
#define libbind(lib, fn) GetProcAddress(lib, fn)
#define libexit(lib) FreeLibraryAndExitThread(lib, 0)
#endif

typedef void(*LIBMAIN_T)(void*, size_t); // Just a type to hold the libmain function

class HotReloadLibrary {
public:
    HotReloadLibrary(std::string&& libPath) {
        m_libpath = std::filesystem::absolute({libPath}); // On Linux, must be absolute path
        if(!std::filesystem::exists(m_libpath)) throw std::runtime_error("Failed initial library load. File does not exist.");
        if(update() != UpdateReturnCodes::UPDATE_SUCCESSFUL) throw std::runtime_error("Failed initial library load."); // Something went wrong?
    }

    ~HotReloadLibrary() {
        if(m_libHandle) libexit(m_libHandle); // Clean up
    }

    void call(void* args, size_t count) {
        if(m_libmain) m_libmain(args, count); // If libmain exists, call it with specified arguments and count
    }

    enum class UpdateReturnCodes {
        UPDATE_NOT_REQUIRED,
        UPDATE_SUCCESSFUL,
        UPDATE_NO_LIBMAIN,
        UPDATE_SAME_LIBMAIN
    };

    bool canUpdate() {
        // If file is available and the write time is different, else no you can't update
        if(std::filesystem::exists(m_libpath) && m_lastwrite != std::filesystem::last_write_time(m_libpath))
            return true;
        else return false;
    }

    UpdateReturnCodes update() {
        uintptr_t libmainaddy = reinterpret_cast<uintptr_t>(m_libmain); // cache old libmain for checks
        if(canUpdate()) { // libmain update available?
            if(m_libHandle) libexit(m_libHandle); // close old handle

            m_libHandle = nullptr;
            m_libmain = nullptr;

            do (m_libHandle = libopen(m_libpath.c_str())); while(!m_libHandle); // keep trying to load library until successful
            m_lastwrite = std::filesystem::last_write_time(m_libpath); // set last write time for future checks

            m_libmain = (LIBMAIN_T)libbind(m_libHandle, "libmain"); // set the libmain function address
            if(!m_libmain) return UpdateReturnCodes::UPDATE_NO_LIBMAIN; // libmain was not found?! did you forget to add it or export it?

            if(libmainaddy == reinterpret_cast<uintptr_t>(m_libmain)) return UpdateReturnCodes::UPDATE_SAME_LIBMAIN; // libmain was the same address?! weird.

            return UpdateReturnCodes::UPDATE_SUCCESSFUL; // everything worked swimmingly :)
        }
        return UpdateReturnCodes::UPDATE_NOT_REQUIRED; // Nothing had to change.
    }

    std::string fileName() {
        return m_libpath.filename().string(); // Just get the library name, (e.g., "libyourlib.so")
    }
private:
    std::filesystem::file_time_type m_lastwrite{};
    std::filesystem::path m_libpath{};
    mutable libhandle_t m_libHandle = nullptr;
    mutable LIBMAIN_T m_libmain = nullptr;
};



#endif //HOTRELOADLIBRARY_HPP
