#pragma once
#include <cstdint>
#include <cmath>

// Ultra obfuscated types
struct v2 { float x, y; };
struct v3 { float x, y, z; };
struct m3 { float m[3][4]; };

// Obfuscated offsets namespace
namespace x {
    // Base pointers
    constexpr uintptr_t ce = 0x65F7AD0;  // ClientEngine
    constexpr uintptr_t el = 0x6E4D0D8;  // EntityList
    
    // ClientEngine chain
    constexpr uintptr_t a1 = 0x58;  // IGameplay
    constexpr uintptr_t b1 = 0x58;  // ClientPlayer
    constexpr uintptr_t b2 = 0x58;  // ClientPlayer
    constexpr uintptr_t b3 = 0x238; // Camera
    constexpr uintptr_t b4 = 0x288; // LocalActor
    
    // EntityList offsets
    constexpr uintptr_t c1 = 0x0;   // Head
    constexpr uintptr_t c2 = 0x10;  // ActorInstance
    
    // Actor offsets
    constexpr uintptr_t d1 = 0x0;   // ActorPtr
    constexpr uintptr_t d2 = 0x8;   // ActorComponent
    constexpr uintptr_t d3 = 0x0;   // Entity
    constexpr uintptr_t d4 = 0x8;   // Mask
    constexpr uintptr_t d5 = 0x58;  // Transform
    
    // Skeleton offsets
    constexpr uintptr_t e1 = 0x18;  // SkeletonComponent
    constexpr uintptr_t e2 = 0x90;  // BipedPose
    constexpr uintptr_t e3 = 0x8;   // BoneArray
    constexpr uintptr_t e4 = 0x30;  // BoneMatrix
    
    // Camera offsets
    constexpr uintptr_t f1 = 124;  // CameraX (0x7C)
    constexpr uintptr_t f2 = 128;  // CameraY (0x80)
    constexpr uintptr_t f3 = 132;  // CameraZ (0x84)
    constexpr uintptr_t f4 = 824;  // FOV (0x338)
    constexpr uintptr_t f5 = 752;  // ScreenWidth (0x2F0)
    constexpr uintptr_t f6 = 754;  // ScreenHeight (0x2F2)
    
    // ViewMatrix offsets - MUST match ESP exactly!
    constexpr uintptr_t f7[9] = {
        772, 776, 780,  // Row 1 (0x304, 0x308, 0x30C)
        784, 788, 792,  // Row 2 (0x310, 0x314, 0x318)
        796, 800, 804   // Row 3 (0x31C, 0x320, 0x324)
    };
    
    // ViewAngles offsets - GHIDRA VERIFIED!
    constexpr uintptr_t va_pitch = 0x110;  // Pitch
    constexpr uintptr_t va_yaw = 0x114;    // Yaw
    
    // Bone indices
    constexpr int g[15] = {7, 6, 4, 3, 2, 1, 0, 8, 9, 10, 11, 12, 13, 14, 5};
    constexpr int head = 7; // biped Head
}

// Ultra-fast math helpers
namespace u {
    constexpr float pi = 3.14159265f;
    constexpr float d2r = pi / 180.0f;
    constexpr float r2d = 180.0f / pi;
    
    inline float norm(float a) {
        while (a > 180.0f) a -= 360.0f;
        while (a < -180.0f) a += 360.0f;
        return a;
    }
    
    inline float dist(const v3& a, const v3& b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float dz = b.z - a.z;
        return sqrtf(dx*dx + dy*dy + dz*dz);
    }
    
    inline float dist2d(const v2& a, const v2& b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return sqrtf(dx*dx + dy*dy);
    }
}
