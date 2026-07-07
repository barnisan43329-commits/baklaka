#pragma once
#include "a.h"
#include "t.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <windows.h>

// Ultra obfuscated aimbot engine
class s {
private:
    m* mm;
    uintptr_t bs;
    v2 sc;      // screen center
    float fov;  // aim fov
    float sm;   // smoothness
    float sens; // mouse sensitivity multiplier
    
    inline uintptr_t gc() {
        if (!mm->v()) return 0;
        
        // Read ClientEngine pointer
        uintptr_t ce = mm->r<uintptr_t>(bs + x::ce);
        if (!ce) return 0;
        
        // IGameplay
        uintptr_t gp = mm->r<uintptr_t>(ce + x::a1);
        if (!gp) return 0;
        
        // ClientPlayer
        uintptr_t cp = mm->r<uintptr_t>(gp + x::b1);
        if (!cp) return 0;
        
        // Camera
        return mm->r<uintptr_t>(cp + x::b3);
    }
    
    inline uintptr_t gl() {
        if (!mm->v()) return 0;
        
        // Read ClientEngine pointer
        uintptr_t ce = mm->r<uintptr_t>(bs + x::ce);
        if (!ce) return 0;
        
        // IGameplay
        uintptr_t gp = mm->r<uintptr_t>(ce + x::a1);
        if (!gp) return 0;
        
        // ClientPlayer
        uintptr_t cp = mm->r<uintptr_t>(gp + x::b1);
        if (!cp) return 0;
        
        // LocalActor
        return mm->r<uintptr_t>(cp + x::b4);
    }
    
    inline v3 rh(uintptr_t et, uintptr_t cm) {
        v3 wp{0, 0, 0};
        
        m3 tr{};
        mm->rb(et + x::d5, &tr, sizeof(m3));
        
        uintptr_t ai = mm->r<uintptr_t>(et - 0x10);
        if (!ai) return wp;
        
        uintptr_t ps = mm->r<uintptr_t>(ai + x::e1);
        if (!ps) return wp;
        
        uintptr_t bp = mm->r<uintptr_t>(ps + x::e2) + x::e3;
        uintptr_t bs = mm->r<uintptr_t>(bp + (x::head * 0x8));
        if (!bs) return wp;
        
        m3 bm{};
        mm->rb(bs + x::e4, &bm, sizeof(m3));
        
        // Transform bone to world
        wp.x = (tr.m[0][0] * bm.m[2][1]) + (tr.m[0][3] * bm.m[2][2]) + (tr.m[1][2] * bm.m[2][3]) + tr.m[2][1];
        wp.y = (tr.m[0][1] * bm.m[2][1]) + (tr.m[1][0] * bm.m[2][2]) + (tr.m[1][3] * bm.m[2][3]) + tr.m[2][2];
        wp.z = (tr.m[0][2] * bm.m[2][1]) + (tr.m[1][1] * bm.m[2][2]) + (tr.m[2][0] * bm.m[2][3]) + tr.m[2][3];
        
        return wp;
    }
    
    inline bool ws(uintptr_t c, const v3& w, v2& o) {
        if (!c) return false;
        float cx = mm->r<float>(c + x::f1);
        float cy = mm->r<float>(c + x::f2);
        float cz = mm->r<float>(c + x::f3);
        float rx = w.x - cx, ry = w.y - cy, rz = w.z - cz;
        float vm[9];
        for (int i = 0; i < 9; i++) vm[i] = mm->r<float>(c + x::f7[i]);
        float px = rx * vm[0] + ry * vm[3] + rz * vm[6];
        float py = rx * vm[1] + ry * vm[4] + rz * vm[7];
        float pz = rx * vm[2] + ry * vm[5] + rz * vm[8];
        if (pz >= -0.01f) return false;
        pz = -pz;
        float fv = mm->r<float>(c + x::f4);
        float f = 1.0f / tanf((fv * 0.017453292f) * 0.5f);
        uint16_t sw = mm->r<uint16_t>(c + x::f5);
        uint16_t sh = mm->r<uint16_t>(c + x::f6);
        float iz = 1.0f / fmaxf(fabsf(pz), 0.000001f);
        o.x = roundf(((px * iz) * f * sh + sw) * 0.5f * 10.0f) * 0.1f;
        o.y = roundf(((sh - ((py * iz) * f * sh)) * 0.5f) * 10.0f) * 0.1f;
        return true;
    }
    
    inline void ca(const v3& cp, const v3& tp, float& p, float& y) {
        float dx = tp.x - cp.x;
        float dy = tp.y - cp.y;
        float dz = tp.z - cp.z;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);
        if (dist < 0.01f) return;
        p = -asinf(dz / dist) * u::r2d;
        y = atan2f(dy, dx) * u::r2d;
    }
    
public:
    s(m* mp, float fv = 80.0f, float smooth = 5.0f, float sensitivity = 100.0f) : mm(mp), bs(mp->gb()), fov(fv), sm(smooth), sens(sensitivity) {
        sc.x = 960.0f; // 1920/2
        sc.y = 540.0f; // 1080/2
    }
    
    inline bool aim() {
        if (!mm->v()) {
            std::cout << "[DEBUG] Memory not valid" << std::endl;
            return false;
        }
        
        uintptr_t cm = gc();
        if (!cm) {
            std::cout << "[DEBUG] Camera is null" << std::endl;
            return false;
        }
        
        uintptr_t la = gl();
        
        // Get camera position
        v3 cpos{
            mm->r<float>(cm + x::f1),
            mm->r<float>(cm + x::f2),
            mm->r<float>(cm + x::f3)
        };
        
        // Find entities - Match ESP code exactly!
        uintptr_t el_ptr = mm->r<uintptr_t>(bs + x::el);
        if (!el_ptr) {
            std::cout << "[DEBUG] EntityList pointer is null" << std::endl;
            return false;
        }
        uintptr_t hd = mm->r<uintptr_t>(el_ptr + 0x8);  // Head at +0x8, not +0x0!
        if (!hd) return false;
        uintptr_t cu = mm->r<uintptr_t>(hd);
        
        float best_dist = 999999.0f;
        v3 best_target{0, 0, 0};
        bool found = false;
        int total_enemies = 0;
        int in_fov = 0;
        
        int ct = 0;
        while (cu && cu != hd && ct++ < 100) {
            uintptr_t ai = mm->r<uintptr_t>(cu + 0x18);  // ActorInstance at +0x18
            if (ai) {
                uintptr_t ap = mm->r<uintptr_t>(ai + 0x278);  // ActorProps at +0x278
                if (ap) {
                    uintptr_t ac = mm->r<uintptr_t>(ap + 0x18);  // ActorComponent at +0x18
                    if (ac) {
                        uintptr_t et = mm->r<uintptr_t>(ac + 0x40);  // IEntity at +0x40
                        if (et && et != la) {  // Skip local player
                            uint64_t mk = mm->r<uint64_t>(et + 0x2e0);  // EntityMask at +0x2e0
                            if (mk == 2) {  // Only enemies (mask == 2)
                                total_enemies++;
                                
                                // Get head position
                                v3 head = rh(et, cm);
                                
                                // World to screen
                                v2 sp{};
                                if (ws(cm, head, sp)) {
                                    // Additional check: Skip if entity is at exact screen center (likely self)
                                    float dx_check = fabsf(sp.x - sc.x);
                                    float dy_check = fabsf(sp.y - sc.y);
                                    if (dx_check < 5.0f && dy_check < 5.0f) {
                                        continue;  // Skip entities too close to center (self or glitched)
                                    }
                                    
                                    // Check FOV
                                    float d = u::dist2d(sc, sp);
                                    
                                    static int pos_debug = 0;
                                    if (++pos_debug >= 60) {
                                        std::cout << "[DEBUG] Enemy screen pos: (" << sp.x << ", " << sp.y << ") | Center: (" << sc.x << ", " << sc.y << ") | Dist: " << d << std::endl;
                                        pos_debug = 0;
                                    }
                                    
                                    if (d < fov) {
                                        in_fov++;
                                        if (d < best_dist) {
                                            best_dist = d;
                                            best_target = head;
                                            found = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            cu = mm->r<uintptr_t>(cu);
        }
        
        static int debug_tick = 0;
        if (++debug_tick >= 60) {
            std::cout << "[DEBUG] Enemies: " << total_enemies << " | In FOV: " << in_fov << std::endl;
            debug_tick = 0;
        }
        
        if (!found) {
            std::cout << "[AIM] No target in FOV" << std::endl;
            return false;
        }
        
        std::cout << "[AIM] Locked target!" << std::endl;
        
        // Calculate angles to target using 3D math
        float dx = best_target.x - cpos.x;
        float dy = best_target.y - cpos.y;
        float dz = best_target.z - cpos.z;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);
        
        if (dist < 0.01f) return false;
        
        // Calculate target angles
        float target_pitch = -asinf(dz / dist) * u::r2d;
        float target_yaw = atan2f(dy, dx) * u::r2d;
        
        // Get current angles
        float current_pitch = mm->r<float>(cm + x::va_pitch);
        float current_yaw = mm->r<float>(cm + x::va_yaw);
        
        // Calculate delta with angle normalization
        float delta_pitch = u::norm(target_pitch - current_pitch);
        float delta_yaw = u::norm(target_yaw - current_yaw);
        
        // Apply smoothing
        float new_pitch = current_pitch + (delta_pitch / sm);
        float new_yaw = current_yaw + (delta_yaw / sm);
        
        // Clamp pitch
        if (new_pitch > 89.0f) new_pitch = 89.0f;
        if (new_pitch < -89.0f) new_pitch = -89.0f;
        
        static int aim_debug = 0;
        if (++aim_debug >= 30) {
            std::cout << "[AIM] Current: (" << current_pitch << ", " << current_yaw << ") | Target: (" << target_pitch << ", " << target_yaw << ") | New: (" << new_pitch << ", " << new_yaw << ")" << std::endl;
            aim_debug = 0;
        }
        
        // Write ViewAngles
        mm->w<float>(cm + x::va_pitch, new_pitch);
        mm->w<float>(cm + x::va_yaw, new_yaw);
        
        return true;
    }
};
