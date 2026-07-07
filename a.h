#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

// Ultra obfuscated memory - senior dev nightmare mode
class m {
private:
    HANDLE h;
    uintptr_t b;
    
    inline DWORD p(const wchar_t* n) {
        HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (s == INVALID_HANDLE_VALUE) return 0;
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(s, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, n) == 0) {
                    CloseHandle(s);
                    return pe.th32ProcessID;
                }
            } while (Process32NextW(s, &pe));
        }
        CloseHandle(s);
        return 0;
    }
    
    inline uintptr_t g(DWORD pid) {
        HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (s == INVALID_HANDLE_VALUE) return 0;
        MODULEENTRY32W me;
        me.dwSize = sizeof(me);
        if (Module32FirstW(s, &me)) {
            CloseHandle(s);
            return (uintptr_t)me.modBaseAddr;
        }
        CloseHandle(s);
        return 0;
    }
    
public:
    m() : h(nullptr), b(0) {}
    
    inline bool a(const wchar_t* n) {
        DWORD pid = p(n);
        if (!pid) return false;
        h = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
        if (!h) return false;
        b = g(pid);
        return b != 0;
    }
    
    template<typename T>
    inline T r(uintptr_t addr) {
        T v{};
        ReadProcessMemory(h, (LPCVOID)addr, &v, sizeof(T), nullptr);
        return v;
    }
    
    template<typename T>
    inline bool w(uintptr_t addr, T val) {
        return WriteProcessMemory(h, (LPVOID)addr, &val, sizeof(T), nullptr);
    }
    
    inline void rb(uintptr_t addr, void* buf, size_t sz) {
        ReadProcessMemory(h, (LPCVOID)addr, buf, sz, nullptr);
    }
    
    inline bool v() const { return h != nullptr; }
    inline uintptr_t gb() const { return b; }
};
