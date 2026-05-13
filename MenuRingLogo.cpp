#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>

// 渐变空心圆环悬浮球 - 桃粉→灰蓝→柔粉，柔光边缘
void DrawRingLogo(ImDrawList* dl, ImVec2 center, float r, float ui_scale, float alpha, ImU32 center_bg, float aspect_ratio, float rotation) {
    const float ring_w = r * 0.28f;          // 环宽
    const float outer_r = r;
    const float inner_r = r - ring_w;
    const int   segs = 144;                   // 分段数
    const float asp = aspect_ratio;           // Y轴缩放比（<1=扁椭圆, 1=正圆）

    // 柔光外晕：多层半透明圆环
    for (int i = 0; i < 3; ++i) {
        float glow_r = outer_r + (i + 1) * 2.5f * ui_scale;
        float glow_a = alpha * (0.12f - i * 0.035f);
        if (fabsf(asp - 1.0f) < 0.002f) {
            dl->AddCircle(center, glow_r, IM_COL32(200, 195, 210, (int)(glow_a * 255)), 64, 1.8f * ui_scale);
        } else {
            const int gsegs = 48;
            ImVec2 pts[gsegs + 1];
            for (int j = 0; j <= gsegs; ++j) {
                float ga = (float)j / gsegs * 2.0f * IM_PI + rotation;
                pts[j] = ImVec2(center.x + cosf(ga) * glow_r, center.y + sinf(ga) * glow_r * asp);
            }
            dl->AddPolyline(pts, gsegs + 1, IM_COL32(200, 195, 210, (int)(glow_a * 255)), ImDrawFlags_None, 1.8f * ui_scale);
        }
    }

    // 渐变环主体：用分段四边形条带模拟平滑渐变
    // 关键颜色：225°(左上)=桃粉  0°(右)=灰蓝  135°(左下)=柔粉
    auto GradientColor = [](float angle_deg) -> ImU32 {
        float a = fmodf(angle_deg + 360.0f, 360.0f);
        ImU32 c_peach   = IM_COL32(253, 208, 195, 255);  // 桃粉
        ImU32 c_blue    = IM_COL32(175, 195, 218, 255);  // 灰蓝
        ImU32 c_pink    = IM_COL32(242, 188, 198, 255);  // 柔粉
        float t;
        if (a <= 135.0f) {
            t = a / 135.0f;
            return IM_COL32((int)(175 + (242-175)*t), (int)(195 + (188-195)*t), (int)(218 + (198-218)*t), 255);
        } else if (a <= 225.0f) {
            t = (a - 135.0f) / 90.0f;
            return IM_COL32((int)(242 + (253-242)*t), (int)(188 + (208-188)*t), (int)(198 + (195-198)*t), 255);
        } else {
            t = (a - 225.0f) / 135.0f;
            return IM_COL32((int)(253 + (175-253)*t), (int)(208 + (195-208)*t), (int)(195 + (218-195)*t), 255);
        }
    };

    // 用 AddQuadFilled 构建环形条带
    for (int i = 0; i < segs; ++i) {
        float a0 = (float)i / segs * 2.0f * IM_PI + rotation;
        float a1 = (float)(i + 1.08f) / segs * 2.0f * IM_PI + rotation;  // +8%重叠覆盖缝隙
        float deg0 = a0 * 180.0f / IM_PI;
        float deg1 = a1 * 180.0f / IM_PI;
        ImVec2 p0o(center.x + cosf(a0) * outer_r, center.y + sinf(a0) * outer_r * asp);
        ImVec2 p1o(center.x + cosf(a1) * outer_r, center.y + sinf(a1) * outer_r * asp);
        ImVec2 p0i(center.x + cosf(a0) * inner_r, center.y + sinf(a0) * inner_r * asp);
        ImVec2 p1i(center.x + cosf(a1) * inner_r, center.y + sinf(a1) * inner_r * asp);
        ImU32 c0 = GradientColor(deg0);
        ImU32 c1 = GradientColor(deg1);
        // 用平均色填充整个梯形
        ImU32 cavg = IM_COL32(
            (int)((((c0>>IM_COL32_R_SHIFT)&0xFF) + ((c1>>IM_COL32_R_SHIFT)&0xFF)) / 2),
            (int)((((c0>>IM_COL32_G_SHIFT)&0xFF) + ((c1>>IM_COL32_G_SHIFT)&0xFF)) / 2),
            (int)((((c0>>IM_COL32_B_SHIFT)&0xFF) + ((c1>>IM_COL32_B_SHIFT)&0xFF)) / 2), 255);
        dl->AddQuadFilled(p0o, p1o, p1i, p0i, cavg);
    }

    // 中心圆盘（椭圆时用分段多边形）
    const float fill_r = inner_r + 0.5f * ui_scale;  // 略微覆盖环内边，消除间隙
    if (fabsf(asp - 1.0f) < 0.002f) {
        dl->AddCircleFilled(center, fill_r, center_bg, 64);
    } else {
        const int fsegs = 48;
        dl->PathClear();
        dl->PathLineTo(center);
        for (int j = 0; j <= fsegs; ++j) {
            float fa = (float)j / fsegs * 2.0f * IM_PI + rotation;
            dl->PathLineTo(ImVec2(center.x + cosf(fa) * fill_r, center.y + sinf(fa) * fill_r * asp));
        }
        dl->PathFillConvex(center_bg);
    }

    // 内边缘微光
    if (fabsf(asp - 1.0f) < 0.002f) {
        dl->AddCircle(center, inner_r, IM_COL32(200, 195, 210, (int)(alpha * 55)), 0, 2.8f * ui_scale);
        dl->AddCircle(center, outer_r, IM_COL32(200, 195, 210, (int)(alpha * 50)), 0, 2.8f * ui_scale);
        dl->AddCircle(center, inner_r, IM_COL32(255, 255, 255, (int)(alpha * 45)), 64, 1.2f * ui_scale);
    }
    // 外边缘微光
    if (fabsf(asp - 1.0f) < 0.002f) {
        dl->AddCircle(center, outer_r, IM_COL32(255, 255, 255, (int)(alpha * 35)), 64, 1.0f * ui_scale);
    }
}
