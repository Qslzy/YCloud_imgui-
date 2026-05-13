#pragma once
#ifndef INJECTION_ANIM_H
#define INJECTION_ANIM_H
#include "imgui.h"

extern float g_ui_scale;
extern const float g_ball_radius;
extern void DrawRingLogo(ImDrawList* dl, ImVec2 center, float r, float ui_scale,
                          float alpha, ImU32 center_bg, float aspect_ratio, float rotation);

struct InjectionAnim {
    // 动画状态
    bool     playing   = false;   // 是否正在播放
    bool     done      = false;   // 是否已完成
    bool     triggered = false;   // 是否已触发
    float    elapsed   = 0.0f;    // 已经过时间
    float    duration  = 9.0f;    // 总时长（秒）

    // 缓动函数
    static float EaseOutElastic(float t);   // 弹性缓动
    static float EaseOutCubic(float t);     // 三次方缓出
    static float EaseInOutCubic(float t);   // 三次方缓入缓出
    static float EaseOutBack(float t);      // 回弹缓出
    static float EaseOutBounce(float t);    // 弹跳缓出
    static float EaseOutQuint(float t);     // 五次方缓出
    static float EaseInQuint(float t);      // 五次方缓入

    void Fire();    // 触发动画
    void Render();  // 渲染动画

private:
    float _particle_phase = 0.0f;       // 粒子相位
    float _prev_total_scale = 0.0f;     // 上一帧总缩放
    float _ring_rotation = 0.0f;        // 圆环旋转角度
};

extern InjectionAnim g_injection_anim;
#endif
