#include "a.h"
#include "s.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA("svc");
    
    std::cout << "[*] Starting..." << std::endl;
    
    m mm;
    std::cout << "[*] Waiting for game..." << std::endl;
    while (!mm.a(L"BloodStrike.exe")) 
        std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "[+] Game found!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Create aimbot with FOV 300, smoothness 5, sensitivity 100
    s ab(&mm, 300.0f, 5.0f, 100.0f);
    
    std::cout << "[+] Ready!" << std::endl;
    std::cout << "[*] Hold LEFT SHIFT to aim" << std::endl;
    std::cout << "[*] Press END to exit" << std::endl;
    
    // Ultra optimized loop - 240Hz
    auto lt = std::chrono::high_resolution_clock::now();
    const auto dt = std::chrono::microseconds(4166);  // 240 Hz
    
    int debug_counter = 0;
    
    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) break;
        
        auto ct = std::chrono::high_resolution_clock::now();
        if (ct - lt < dt) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }
        lt = ct;
        
        // Check if LEFT SHIFT is held
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
            bool result = ab.aim();
            
            // Debug output every 60 frames (quarter second)
            if (++debug_counter >= 60) {
                if (result) {
                    std::cout << "[AIM] Locked target!" << std::endl;
                } else {
                    std::cout << "[AIM] No target in FOV" << std::endl;
                }
                debug_counter = 0;
            }
        } else {
            debug_counter = 0;
        }
    }
    
    std::cout << "[*] Exiting..." << std::endl;
    return 0;
}
