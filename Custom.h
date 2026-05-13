#include <map>
#include <cmath>
#include <string>
#include <algorithm>
#include <chrono>
#include <vector>

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline ImVec2 operator*(const ImVec2& lhs, float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
inline ImVec2 operator/(const ImVec2& lhs, float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline ImVec2& operator*=(ImVec2& lhs, float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
inline ImVec4 operator*(const ImVec4& lhs, float rhs) { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
inline float ImLength(const ImVec2& vec) { return sqrtf(vec.x * vec.x + vec.y * vec.y); }
inline bool operator<(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z && lhs.w < rhs.w; }
inline bool operator>(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z && lhs.w > rhs.w; }
inline float ImClamp(float v, float mn, float mx) { return v < mn ? mn : (v > mx ? mx : v); }
inline ImVec4 ImClamp(const ImVec4& v, const ImVec4& mn, const ImVec4& mx) { return ImVec4(ImClamp(v.x, mn.x, mx.x), ImClamp(v.y, mn.y, mx.y), ImClamp(v.z, mn.z, mx.z), ImClamp(v.w, mn.w, mx.w)); }

using namespace ImGui;

extern float g_custom_draw_alpha;

namespace Custom {
    namespace Colors {
        static ImVec4 accent = ImVec4(74.0f / 255.0f, 141.0f / 255.0f, 247.0f / 255.0f, 1.0f);
        inline bool dark_mode = false;
        inline float rounding = 22;
        inline ImVec4 background = ImColor(15, 15, 17);
        inline ImVec4 background_widget = ImColor(21, 23, 26);
        inline ImVec4 background_switch = { 209/255.f, 213/255.f, 219/255.f, 1.f };
        inline ImVec4 switch_on = { 74/255.f, 141/255.f, 247/255.f, 1.f };
        inline ImVec4 switch_knob = { 1.0f, 1.0f, 1.0f, 1.0f };
        inline ImVec4 element_layout = ImColor(21, 21, 29);
        inline ImVec4 element_circle = ImColor(41, 41, 53);
        inline ImVec4 dropdown_selection_layout = ImColor(31, 31, 41);
        inline ImVec4 panel_background = ImColor(20, 20, 20);
        inline ImVec4 panel_header = ImColor(25, 25, 25);
        inline ImVec4 panel_border = ImColor(35, 35, 35);
        inline ImVec4 module_rect = ImColor(30, 30, 30);
        inline ImVec4 module_rect_hovered = ImColor(35, 35, 35);
        inline ImVec4 module_rect_active = ImColor(40, 40, 40);
        inline ImVec4 category_header = ImColor(255, 255, 255, 255);
        namespace backgroundM {
            inline ImVec4 filling = ImColor(12, 12, 12);
            inline ImVec4 stroke = ImColor(24, 26, 36);
            inline float rounding = 6;
        }
        namespace text {
            inline ImVec4 text_active = ImColor(255, 255, 255, 255);
            inline ImVec4 text_hov = ImColor(86, 88, 95, 255);
            inline ImVec4 text = ImColor(100, 100, 100, 255);
        }
    }

    inline ImU32 GetColorU32Ex(const ImVec4& col, float alpha = 1.0f) {
        return IM_COL32((int)(col.x * 255), (int)(col.y * 255), (int)(col.z * 255), (int)(col.w * 255 * alpha * ::g_custom_draw_alpha));
    }

    inline float fixed_speed(float speed) {
        return speed * ImGui::GetIO().DeltaTime;
    }

    void GlowText(ImFont* font, float size, const char* label, ImVec2 pos, ImU32 text_color) {
        ImGui::PushFont(font);
        float glow_size = 15.f;
        for (int i = 0; i < glow_size; i++) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f / i / 10);
            ImGui::GetWindowDrawList()->AddText(font, size, pos + ImVec2(i, i), ImGui::GetColorU32(text_color), label);
            ImGui::PopStyleVar();
        }
        ImGui::PopFont();
    }

    struct tab_state {
        float highlight_width = 2.0f;
        ImVec4 text_color = Colors::text::text;
        float bg_width = 0.0f;
        float bg_alpha = 0.30f;
        float corner_radius = 6.0f;
        float highlight_padding_vertical = 6.0f;
        float highlight_padding_horizontal = 8.0f;
        float text_padding_left = 24.0f;
    };

    bool Tab(const char* label, bool selected) {
        if (!label) return false;
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        const ImVec2 pos = window->DC.CursorPos;
        const float total_width = ImGui::GetContentRegionAvail().x;
        const float height = label_size.y + 24.0f;
        const ImRect total_bb(pos, pos + ImVec2(total_width, height));
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id)) return false;
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        bool new_selected = selected;
        if (pressed) {
            new_selected = !selected;
        }
        static std::map<ImGuiID, tab_state> anim_map;
        auto it = anim_map.find(id);
        if (it == anim_map.end()) {
            tab_state init_state;
            init_state.highlight_width = selected ? 4.0f : 2.0f;
            init_state.text_color = selected ? Colors::text::text_active : (hovered ? Colors::text::text_hov : Colors::text::text);
            init_state.bg_width = selected ? total_width : 0.0f;
            anim_map[id] = init_state;
            it = anim_map.find(id);
        }
        tab_state& state = it->second;
        float target_width = selected ? 4.0f : 2.0f;
        float target_bg_width = selected ? total_width : 0.0f;
        ImVec4 target_color;
        if (selected) {
            target_color = Colors::text::text_active;
        } else if (hovered) {
            target_color = Colors::text::text_hov;
        } else {
            target_color = Colors::text::text;
        }
        float speed = 6.0f;
        float delta = g.IO.DeltaTime * speed;
        state.highlight_width = ImLerp(state.highlight_width, target_width, delta);
        state.bg_width = ImLerp(state.bg_width, target_bg_width, delta);
        state.text_color.x = ImLerp(state.text_color.x, target_color.x, delta);
        state.text_color.y = ImLerp(state.text_color.y, target_color.y, delta);
        state.text_color.z = ImLerp(state.text_color.z, target_color.z, delta);
        state.text_color.w = ImLerp(state.text_color.w, target_color.w, delta);
        if (state.bg_width > 0.01f) {
            ImDrawList* draw_list = window->DrawList;
            int segments = 32;
            float segment_width = state.bg_width / segments;
            for (int i = 0; i < segments; i++) {
                float t = (float)i / segments;
                float alpha = state.bg_alpha * (1.0f - t);
                if (alpha <= 0.01f) continue;
                ImU32 segment_color = ImGui::GetColorU32(ImVec4(Colors::accent.x, Colors::accent.y, Colors::accent.z, alpha));
                float x1 = total_bb.Min.x + i * segment_width;
                float x2 = (i == segments - 1) ? total_bb.Min.x + state.bg_width : x1 + segment_width;
                ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
                if (i == 0 && state.bg_width >= total_width) {
                    flags = ImDrawFlags_RoundCornersLeft;
                } else if (i == segments - 1 && state.bg_width >= total_width) {
                    flags = ImDrawFlags_RoundCornersRight;
                } else if (i == 0 && state.bg_width < total_width) {
                    flags = ImDrawFlags_RoundCornersLeft;
                }
                if (state.bg_width >= total_width) {
                    if (i == 0) flags = ImDrawFlags_RoundCornersLeft;
                    else if (i == segments - 1) flags = ImDrawFlags_RoundCornersRight;
                } else {
                    if (i == 0) flags = ImDrawFlags_RoundCornersLeft;
                }
                draw_list->AddRectFilled(ImVec2(x1, total_bb.Min.y), ImVec2(x2, total_bb.Max.y), segment_color, state.corner_radius, flags);
            }
        }
        float highlight_left = total_bb.Min.x + state.highlight_padding_horizontal;
        float highlight_top = total_bb.Min.y + state.highlight_padding_vertical;
        float highlight_bottom = total_bb.Max.y - state.highlight_padding_vertical;
        float highlight_height = highlight_bottom - highlight_top;
        if (highlight_height > 0) {
            ImRect line_bb(ImVec2(highlight_left, highlight_top), ImVec2(highlight_left + state.highlight_width, highlight_bottom));
            ImU32 line_color = selected ? ImGui::GetColorU32(Colors::accent) : ImGui::GetColorU32(ImVec4(Colors::text::text.x, Colors::text::text.y, Colors::text::text.z, 0.5f));
            window->DrawList->AddRectFilled(line_bb.Min, line_bb.Max, line_color);
        }
        float text_offset_x = (state.highlight_width - 2.0f) * 2.0f;
        ImVec2 text_pos = pos + ImVec2(state.text_padding_left + text_offset_x, (height - label_size.y) * 0.5f);
        window->DrawList->AddText(text_pos, ImGui::GetColorU32(state.text_color), label);
        return new_selected;
    }

    struct ButtonState {
        float bg_progress = 0.0f;
        float text_progress = 0.0f;
    };

    inline bool Button(const char* label, const ImVec2& size = ImVec2(0, 0)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        float label_w = ImGui::CalcTextSize(label, NULL, true).x;
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 final_size = size;
        if (final_size.y <= 0.0f) final_size.y = 34.0f;
        if (final_size.x <= 0.0f) {
            if (size.x < 0.0f) {
                final_size.x = ImGui::GetContentRegionAvail().x;
            } else {
                final_size.x = label_w + style.FramePadding.x * 2.0f + 24.0f;
                if (final_size.x < 70.0f) final_size.x = 70.0f;
                float rem = ImGui::GetContentRegionAvail().x;
                if (final_size.x > rem) final_size.x = rem;
            }
        }
        ImRect rect(pos, pos + final_size);
        ImGui::ItemSize(rect, 0);
        if (!ImGui::ItemAdd(rect, id)) return false;
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held);
        if (pressed) ImGui::MarkItemEdited(id);

        static std::map<ImGuiID, ButtonState> anim;
        ButtonState& state = anim[id];
        float dt = g.IO.DeltaTime;
        float target_bg = (hovered || held) ? 1.0f : 0.0f;
        state.bg_progress = ImLerp(state.bg_progress, target_bg, dt * 10.0f);
        state.text_progress = ImLerp(state.text_progress, target_bg, dt * 10.0f);

        ImDrawList* dl = window->DrawList;
        ImVec4 bg_col = ImLerp(Colors::dark_mode ? ImVec4(0.075f,0.070f,0.100f,1.0f) : ImVec4(0.98f, 0.98f, 0.98f, 1.0f), Colors::accent, state.bg_progress);
        ImU32 bg = GetColorU32Ex(bg_col);
        float r = 8.0f;
        dl->AddRectFilled(rect.Min, rect.Max, bg, r);
        if (state.bg_progress < 0.5f) {
            dl->AddRect(rect.Min, rect.Max, IM_COL32(0,0,0,(int)(30*(1.0f-state.bg_progress*2.0f))), r, 0, 1.0f);
        }
        ImVec4 txt_col = ImLerp(Colors::dark_mode ? ImVec4(0.88f,0.84f,0.94f,1.0f) : ImVec4(0.23f, 0.27f, 0.33f, 1.0f), ImVec4(1,1,1,1), state.text_progress);
        dl->AddText(rect.Min + (rect.GetSize() - ImGui::CalcTextSize(label)) * 0.5f,
                    GetColorU32Ex(txt_col), label);
        return pressed;
    }

    struct switch_state {
        float knob_x = 0.0f;
        float track_r = 0.0f;
        float track_g = 0.0f;
        float track_b = 0.0f;
        ImVec4 text_col = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    };

    bool Switch(const char* label, bool* v, bool align_to_right = false) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const char* label_end = ImGui::FindRenderedTextEnd(label);
        bool hide_label = (label[0] == '#' && label[1] == '#' && label[2] != '#');
        const char* display_label = hide_label ? label_end : label;
        const ImVec2 label_size = hide_label ? ImVec2(0, 0) : ImGui::CalcTextSize(display_label, NULL, true);

        const float track_w = 75.0f;
        const float track_h = 30.0f;
        const float knob_r = 13.0f;
        const float knob_pad = 5.0f;
        const float row_h = track_h;
        const float spacing = 10.0f;

        const ImVec2 pos = window->DC.CursorPos;
        float switch_x;
        if (hide_label) {
            switch_x = pos.x;
        } else if (align_to_right) {
            switch_x = window->WorkRect.Max.x - track_w;
        } else {
            switch_x = pos.x + label_size.x + spacing;
        }
        const float total_width = hide_label ? track_w : (switch_x - pos.x + track_w);
        const ImRect total_bb(pos, pos + ImVec2(total_width, row_h));
        ImGui::ItemSize(total_bb, 0.0f);
        if (!ImGui::ItemAdd(total_bb, id)) return false;
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

        static std::map<ImGuiID, switch_state> anim;
        auto it_anim = anim.find(id);
        if (it_anim == anim.end()) {
            switch_state init;
            init.knob_x = *v ? (track_w - knob_r * 2.0f - knob_pad) : knob_pad;
            init.track_r = *v ? 0.29f : 0.82f;
            init.track_g = *v ? 0.55f : 0.84f;
            init.track_b = *v ? 0.97f : 0.86f;
            anim[id] = init;
            it_anim = anim.find(id);
        }
        switch_state& st = it_anim->second;

        if (pressed) {
            *v = !(*v);
            ImGui::MarkItemEdited(id);
        }

        float dt = g.IO.DeltaTime;
        float target_knob = *v ? (track_w - knob_r * 2.0f - knob_pad) : knob_pad;
        st.knob_x = ImLerp(st.knob_x, target_knob, dt * 12.0f);

        float tr = Colors::dark_mode ? 0.145f : 0.82f, tg = Colors::dark_mode ? 0.125f : 0.84f, tb = Colors::dark_mode ? 0.190f : 0.86f;
        float tr_on = Colors::accent.x, tg_on = Colors::accent.y, tb_on = Colors::accent.z;
        float t = (st.knob_x - knob_pad) / (track_w - knob_r * 2.0f - knob_pad * 2.0f);
        t = ImClamp(t, 0.0f, 1.0f);
        st.track_r = ImLerp(tr, tr_on, t);
        st.track_g = ImLerp(tg, tg_on, t);
        st.track_b = ImLerp(tb, tb_on, t);

        ImVec4 target_text = *v ? ImVec4(0.12f, 0.16f, 0.23f, 1.0f)
                                : (hovered ? ImVec4(0.3f, 0.3f, 0.3f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        st.text_col = ImLerp(st.text_col, target_text, dt * 8.0f);

        ImDrawList* dl = window->DrawList;
        ImVec2 track_min(switch_x, pos.y + (row_h - track_h) * 0.5f);
        ImVec2 track_max(switch_x + track_w, track_min.y + track_h);
        float pill_r = track_h * 0.5f;

        float alpha = g_custom_draw_alpha;
        ImU32 track_col = IM_COL32(
            (int)(st.track_r * 255), (int)(st.track_g * 255), (int)(st.track_b * 255), (int)(255 * alpha));
        dl->AddRectFilled(track_min, track_max, track_col, pill_r);

        if (t > 0.01f) {
            float glow_a = t * 0.25f * alpha;
            dl->AddRectFilled(
                ImVec2(track_min.x - 3.0f, track_min.y - 3.0f),
                ImVec2(track_max.x + 3.0f, track_max.y + 3.0f),
                IM_COL32((int)(Colors::accent.x*255), (int)(Colors::accent.y*255), (int)(Colors::accent.z*255), (int)(glow_a*255)),
                pill_r + 3.0f);
        }

        float kn_cx = track_min.x + knob_pad + knob_r + st.knob_x - knob_pad;
        float kn_cy = track_min.y + track_h * 0.5f;
        dl->AddCircleFilled(ImVec2(kn_cx + 1.0f, kn_cy + 2.0f), knob_r,
                            IM_COL32(0, 0, 0, (int)(40 * alpha)), 24);
        dl->AddCircleFilled(ImVec2(kn_cx, kn_cy), knob_r,
                            Colors::dark_mode ? IM_COL32(232,222,255,(int)(255*alpha)) : IM_COL32(255, 255, 255, (int)(255 * alpha)), 24);

        if (!hide_label) {
            dl->AddText(pos + ImVec2(0.0f, (row_h - label_size.y) / 2.0f),
                        GetColorU32Ex(st.text_col), display_label);
        }
        return pressed;
    }

    struct ChildState {
        bool initialized = false;
        ImVec2 header_size;
    };

    bool BeginChild(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags) {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* parent_window = g.CurrentWindow;
        flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
        flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);
        const ImVec2 content_avail = ImGui::GetContentRegionAvail();
        ImVec2 size = ImFloor(size_arg);
        if (size.x <= 0.0f) size.x = ImMax(content_avail.x + size.x, 4.0f);
        if (size.y <= 0.0f) size.y = ImMax(content_avail.y + size.y, 4.0f);
        const float header_height = 32.0f;
        const float bottom_padding = 4.0f;
        ImVec2 content_size = ImVec2(size.x, size.y - header_height - bottom_padding);
        ImVec2 window_pos = parent_window->DC.CursorPos;
        ImGui::SetNextWindowPos(window_pos);
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowContentSize(content_size);
        const char* temp_window_name;
        if (name) ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
        else ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);
        const float backup_border_size = g.Style.ChildBorderSize;
        if (!border) g.Style.ChildBorderSize = 0.0f;
        ImGuiChildFlags child_flags = 0;
        if (border) child_flags |= ImGuiChildFlags_Borders;
        bool ret = ImGui::BeginChild(temp_window_name, size, child_flags, flags);
        g.Style.ChildBorderSize = backup_border_size;
        ImGuiWindow* child_window = g.CurrentWindow;
        child_window->ChildId = id;
        ImDrawList* draw_list = child_window->DrawList;
        ImVec2 win_pos = child_window->Pos;
        ImVec2 win_size = child_window->Size;
        ImU32 accent_color = ImGui::GetColorU32(Colors::accent);
        ImU32 accent_color_50 = ImGui::GetColorU32(ImVec4(Colors::accent.x, Colors::accent.y, Colors::accent.z, 0.5f));
        ImU32 accent_color_0 = ImGui::GetColorU32(ImVec4(Colors::accent.x, Colors::accent.y, Colors::accent.z, 0.0f));
        draw_list->AddRectFilled(win_pos, win_pos + win_size, ImColor(24, 24, 26), 4.0f, ImDrawFlags_RoundCornersAll);
        draw_list->AddRect(win_pos, win_pos + win_size, ImColor(1.0f, 1.0f, 1.0f, 0.03f), 4.0f, ImDrawFlags_RoundCornersAll);
        draw_list->AddRectFilled(win_pos, win_pos + ImVec2(win_size.x, header_height), ImColor(24, 24, 26), 4.0f, ImDrawFlags_RoundCornersTop);
        draw_list->AddLine(win_pos + ImVec2(1, header_height), win_pos + ImVec2(win_size.x - 1, header_height), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
        if (name) {
            draw_list->AddText(ImGui::GetFont(), 24.0f, win_pos + ImVec2(16, 4), accent_color, name);
        }
        if (g.NavActivateId == id && !(child_flags & ImGuiChildFlags_NavFlattened) && 
            (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY)) {
            ImGui::FocusWindow(child_window);
            ImGui::NavInitWindow(child_window, false);
            ImGui::SetActiveID(id + 1, child_window);
            g.ActiveIdSource = ImGuiInputSource_Keyboard;
        }
        ImGui::Dummy(ImVec2(0, header_height + 4.0f));
        return ret;
    }
    bool begin_child(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImGuiChildFlags child_flags = ImGuiChildFlags_None;
        if (border) child_flags |= ImGuiChildFlags_Borders;
        if (size_arg.x == 0.0f) child_flags |= ImGuiChildFlags_AutoResizeX;
        if (size_arg.y == 0.0f) child_flags |= ImGuiChildFlags_AutoResizeY;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        bool ret = BeginChild(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_NoMove);
        if (ret) {
            ImGui::Indent(8.0f);
        } else {
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
        return ret;
    }

    bool begin_child(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags) {
        IM_ASSERT(id != 0);
        return BeginChild(NULL, id, size_arg, border, extra_flags);
    }

    void EndChild() {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (!(window->Flags & ImGuiWindowFlags_ChildWindow))
            return;
        ImGui::Unindent(8.0f);
        ImGui::EndChild();
        ImGui::PopStyleVar();
        g.LogLinePosY = -FLT_MAX;
    }

    struct Notification {
        int id;
        std::string message;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        int type;
    };

    class NotificationSystem {
    public:
        NotificationSystem() : notificationIdCounter(0) {}

        void AddNotification(const std::string& message, int durationMs, int type = 0) {
            auto now = std::chrono::steady_clock::now();
            auto endTime = now + std::chrono::milliseconds(durationMs);
            notifications.push_back({ notificationIdCounter++, message, now, endTime, type });
        }

        void DrawNotifications() {
            auto now = std::chrono::steady_clock::now();
            std::sort(notifications.begin(), notifications.end(),
                [now](const Notification& a, const Notification& b) -> bool {
                    float durationA = std::chrono::duration_cast<std::chrono::milliseconds>(a.endTime - a.startTime).count();
                    float elapsedA = std::chrono::duration_cast<std::chrono::milliseconds>(now - a.startTime).count();
                    float percentageA = (durationA - elapsedA) / durationA;
                    float durationB = std::chrono::duration_cast<std::chrono::milliseconds>(b.endTime - b.startTime).count();
                    float elapsedB = std::chrono::duration_cast<std::chrono::milliseconds>(now - b.startTime).count();
                    float percentageB = (durationB - elapsedB) / durationB;
                    return percentageA < percentageB;
                }
            );
            int currentNotificationPosition = 0;
            for (auto& notification : notifications) {
                if (now < notification.endTime) {
                    float duration = std::chrono::duration_cast<std::chrono::milliseconds>(notification.endTime - notification.startTime).count();
                    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - notification.startTime).count();
                    float percentage = (duration - elapsed) / duration * 100.0f;
                    ShowNotification(currentNotificationPosition, notification.message, percentage, notification.type);
                    currentNotificationPosition++;
                }
            }
            notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
                [now](const Notification& notification) { return now >= notification.endTime; }),
                notifications.end());
        }

    private:
        std::vector<Notification> notifications;
        int notificationIdCounter;

        void ShowNotification(int position, const std::string& message, float percentage, int type = 0) {
            float duePercentage = 100.0f - percentage;
            float alpha = percentage > 10.0f ? 1.0f : percentage / 10.0f;
            ImGui::GetStyle().WindowPadding = ImVec2(15, 10);
            ImGui::SetNextWindowPos(ImVec2(duePercentage < 15.f ? duePercentage : 15.f, 15 + position * 90));
            ImGui::Begin(("##NOTIFY" + std::to_string(position)).c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 region = ImGui::GetContentRegionMax();
            ImVec2 window_size = ImGui::GetWindowSize();
            ImGui::GetWindowDrawList()->AddRectFilled(pos, pos + window_size, GetColorU32Ex(Colors::backgroundM::filling, 0.4f), Colors::rounding);
            ImGui::GetWindowDrawList()->AddRect(pos, pos + window_size, GetColorU32Ex(Colors::backgroundM::stroke, alpha), Colors::rounding);
            ImGui::TextColored(ImColor(GetColorU32Ex(Colors::accent, alpha)), "%s", "[Notification]");
            ImGui::TextColored(ImColor(GetColorU32Ex(Colors::text::text_active, alpha)), "%s", message.c_str());
            ImGui::Dummy(ImVec2(ImGui::CalcTextSize(message.c_str()).x + 15, 5));
            ImGui::End();
        }
    };

    inline NotificationSystem g_NotificationSystem;

    struct CapsuleRadioAnimState {
        float select_x = 0.0f;
    };

    inline bool CapsuleRadio(const char* str_id, const char* const items[], int count, int* current, float width = 0.0f, float height = 42.0f) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        ImDrawList* dl = window->DrawList;
        const ImGuiID id = window->GetID(str_id);

        count = ImClamp(count, 2, 3);
        if (!current) return false;
        if (*current < 0 || *current >= count) *current = 0;

        const float avail_w = ImGui::GetContentRegionAvail().x;
        const float w = width > 0.0f ? width : avail_w;
        const float h = ImMax(34.0f, height);
        const float pad = 4.0f;
        const float seg_w = (w - pad * 2.0f) / (float)count;
        const float round = h * 0.5f;
        const ImVec2 pos = window->DC.CursorPos;
        const ImVec2 size(w, h);

        static std::map<ImGuiID, CapsuleRadioAnimState> anim;
        auto it = anim.find(id);
        if (it == anim.end()) {
            CapsuleRadioAnimState init;
            init.select_x = pad + seg_w * (float)(*current);
            anim[id] = init;
            it = anim.find(id);
        }
        CapsuleRadioAnimState& st = it->second;

        ImGui::InvisibleButton(str_id, size);
        bool changed = false;
        const bool hovered = ImGui::IsItemHovered();
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            float local_x = g.IO.MousePos.x - pos.x - pad;
            int next = (int)(local_x / seg_w);
            next = ImClamp(next, 0, count - 1);
            if (next != *current) {
                *current = next;
                changed = true;
            }
        }

        const float dt = g.IO.DeltaTime;
        const float target_x = pad + seg_w * (float)(*current);
        st.select_x = ImLerp(st.select_x, target_x, dt * 12.0f);

        ImVec4 outer_bg = Colors::dark_mode ? ImVec4(0.075f,0.070f,0.100f, hovered ? 0.98f : 0.90f) : ImVec4(0.96f, 0.98f, 1.00f, hovered ? 0.96f : 0.88f);
        ImVec4 outer_border = Colors::dark_mode ? ImVec4(0.31f,0.24f,0.44f,0.92f) : ImVec4(0.82f, 0.88f, 0.98f, 0.90f);
        ImVec4 active_bg = Colors::accent;
        ImVec4 active_bg2 = ImVec4(0.40f, 0.64f, 1.00f, 1.00f);

        dl->AddRectFilled(pos + ImVec2(0.0f, 3.0f), pos + size + ImVec2(0.0f, 4.0f), IM_COL32(15, 23, 42, 14), round);
        dl->AddRectFilled(pos, pos + size, GetColorU32Ex(outer_bg), round);
        dl->AddRect(pos, pos + size, GetColorU32Ex(outer_border), round, 0, 1.0f);

        for (int i = 1; i < count; ++i) {
            float x = pos.x + pad + seg_w * i;
            dl->AddLine(ImVec2(x, pos.y + 9.0f), ImVec2(x, pos.y + h - 9.0f), IM_COL32(148, 163, 184, 46), 1.0f);
        }

        ImVec2 sel_min = pos + ImVec2(st.select_x, pad);
        ImVec2 sel_max = sel_min + ImVec2(seg_w, h - pad * 2.0f);
        float sel_round = (h - pad * 2.0f) * 0.5f;
        dl->AddRectFilled(sel_min + ImVec2(0.0f, 2.0f), sel_max + ImVec2(0.0f, 2.0f), IM_COL32(37, 99, 235, 35), sel_round);
        dl->AddRectFilled(sel_min, sel_max, GetColorU32Ex(active_bg), sel_round);
        dl->AddRectFilled(sel_min + ImVec2(1.0f, 1.0f), sel_max - ImVec2(1.0f, 1.0f), GetColorU32Ex(active_bg2, 0.35f), sel_round - 1.0f);
        dl->AddRect(sel_min, sel_max, IM_COL32(255, 255, 255, 90), sel_round, 0, 1.0f);

        for (int i = 0; i < count; ++i) {
            const char* label = items[i] ? items[i] : "";
            ImVec2 ts = ImGui::CalcTextSize(label, NULL, true);
            float cx = pos.x + pad + seg_w * (i + 0.5f);
            ImVec2 tp(cx - ts.x * 0.5f, pos.y + (h - ts.y) * 0.5f);
            ImU32 text_col = (i == *current) ? IM_COL32(255, 255, 255, 255) : IM_COL32(148, 163, 184, 200);
            dl->AddText(tp, text_col, label);
        }

        return changed;
    }

    // ... 文件剩余部分（SliderMD3, FeatureCard等）保持原有代码结构 ...
}

// ==================== Material Design 3 风格 SliderFloat ====================
struct SliderMD3AnimState {
    float visual_t = 0.0f;
    float hover_t = 0.0f;
    float active_t = 0.0f;
};

inline bool SliderMD3(const char* label, float* value, float min, float max, float width = 0.0f) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    
    float track_w = width > 0.0f ? width : ImGui::GetContentRegionAvail().x;
    const float track_h = 6.0f;      // 轨道高度
    const float thumb_w_base = 4.0f; // 滑块基础宽度
    const float thumb_h_base = 16.0f;// 滑块基础高度
    const float track_r = 3.0f;      // 轨道圆角
    const float value_w = 40.0f;     // 数值显示宽度
    
    ImVec2 pos = window->DC.CursorPos;
    const float total_w = track_w + value_w + 10.0f;
    const float total_h = ImMax(label_size.y, thumb_h_base) + 8.0f;
    ImRect total_bb(pos, pos + ImVec2(total_w, total_h));
    ImGui::ItemSize(total_bb, 0.0f);
    if (!ImGui::ItemAdd(total_bb, id)) return false;
    
    float normalized = (*value - min) / (max - min);
    normalized = ImClamp(normalized, 0.0f, 1.0f);
    
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    bool changed = false;
    
    static std::map<ImGuiID, SliderMD3AnimState> anim;
    SliderMD3AnimState& st = anim[id];
    float dt = g.IO.DeltaTime;
    
    if (held) {
        float mx = g.IO.MousePos.x;
        float track_x1 = pos.x;
        float track_x2 = pos.x + track_w;
        float t = (mx - track_x1) / (track_x2 - track_x1);
        t = ImClamp(t, 0.0f, 1.0f);
        float new_val = min + t * (max - min);
        if (new_val != *value) {
            *value = new_val;
            changed = true;
        }
        normalized = t;
    }
    
    st.visual_t = ImLerp(st.visual_t, normalized, dt * 12.0f);
    st.hover_t = ImLerp(st.hover_t, hovered ? 1.0f : 0.0f, dt * 10.0f);
    st.active_t = ImLerp(st.active_t, held ? 1.0f : 0.0f, dt * 8.0f);
    
    float track_x1 = pos.x;
    float track_x2 = pos.x + track_w;
    float track_y = pos.y + total_h * 0.5f;
    const float fill_x = ImLerp(track_x1, track_x2, st.visual_t);
    
    // 轨道背景
    const ImVec4 track_bg = Colors::dark_mode ? ImVec4(0.145f,0.125f,0.190f, hovered ? 1.0f : 0.94f) : ImVec4(0.82f, 0.84f, 0.86f, hovered ? 1.00f : 0.92f);
    const ImVec4 fill_bg  = Colors::accent;
    const ImVec4 thumb_bg = Colors::accent;
    const ImVec4 value_col = Colors::dark_mode ? ImVec4(0.74f,0.68f,0.84f,0.96f) : ImVec4(0.29f, 0.31f, 0.43f, 0.94f);
    
    ImVec2 track_min(track_x1, track_y - track_h * 0.5f);
    ImVec2 track_max(track_x2, track_y + track_h * 0.5f);
    
    // 轨道阴影
    dl->AddRectFilled(track_min + ImVec2(0.0f, 1.2f), track_max + ImVec2(0.0f, 1.2f),
                      IM_COL32(30, 41, 59, 14), track_r);
    dl->AddRectFilled(track_min, track_max, GetColorU32Ex(track_bg), track_r);
    
    // 已选择区域
    if (fill_x > track_x1 + 0.5f) {
        ImVec2 fill_min(track_x1, track_y - track_h * 0.5f);
        ImVec2 fill_max(fill_x, track_y + track_h * 0.5f);
        dl->AddRectFilled(fill_min, fill_max, GetColorU32Ex(fill_bg), track_r);
    }
    
    // 滑块
    const float thumb_w = thumb_w_base + st.active_t * 4.0f + st.hover_t * 1.5f;
    const float thumb_h = thumb_h_base + st.active_t * 4.0f;
    const float thumb_round = 6.0f;
    ImVec2 thumb_min(fill_x - thumb_w * 0.5f, track_y - thumb_h * 0.5f);
    ImVec2 thumb_max(fill_x + thumb_w * 0.5f, track_y + thumb_h * 0.5f);
    dl->AddRectFilled(thumb_min + ImVec2(0.0f, 1.0f), thumb_max + ImVec2(0.0f, 1.0f),
                      IM_COL32(30, 41, 59, 22), thumb_round);
    dl->AddRectFilled(thumb_min, thumb_max, GetColorU32Ex(thumb_bg), thumb_round);
    
    // 数值文本
    char value_buf[64];
    snprintf(value_buf, sizeof(value_buf), "%.2f", *value);
    ImVec2 value_sz = ImGui::CalcTextSize(value_buf);
    float value_x = track_x2 + 10.0f;
    ImVec2 value_pos(value_x, track_y - value_sz.y * 0.5f);
    dl->AddText(value_pos, GetColorU32Ex(value_col), value_buf);
    
    return pressed || changed;
}

// ==================== FeatureCard 组件 ====================
struct FeatureCardAnimState {
    float expand_progress = 0.0f;
    float switch_knob_x = 0.0f;
    float bg_bottom_y = 0.0f;
    ImVec4 title_color = ImVec4(0.12f,0.16f,0.23f,1);
    ImVec4 desc_color = ImVec4(0.4f,0.4f,0.4f,1);
    int child_index = 0;
};

struct FeatureCardLayoutState {
    ImVec2 card_pos;
    float card_h;
    float content_h;
};

inline std::map<ImGuiID, FeatureCardAnimState> g_feature_card_anim;
inline std::map<ImGuiID, FeatureCardLayoutState> g_feature_card_layout;
inline std::vector<ImGuiID> g_feature_card_stack;

typedef void (*FeatureCardSwitchNotifyCallback)(const char* title, bool old_enabled, bool new_enabled);
inline FeatureCardSwitchNotifyCallback g_feature_card_switch_notify_callback = nullptr;

inline void SetFeatureCardSwitchNotifyCallback(FeatureCardSwitchNotifyCallback callback) {
    g_feature_card_switch_notify_callback = callback;
}

inline void NotifyFeatureCardSwitch(const char* title, bool old_enabled, bool new_enabled) {
    if (g_feature_card_switch_notify_callback && old_enabled != new_enabled) {
        g_feature_card_switch_notify_callback(title, old_enabled, new_enabled);
    }
}

inline bool FeatureCardHeader(const char* title, const char* desc, bool* enabled, bool* expanded) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;
    ImGuiContext& g = *GImGui;
    ImGuiID id = window->GetID(title);
    
    const float sw_w = 64.0f;
    const float sw_h = 34.0f;
    const float sw_travel = sw_w - sw_h;
    const float card_pad = 12.0f;
    const float round = 10.0f;
    
    const float avail_w = ImGui::GetContentRegionAvail().x;
    const float desc_h = desc ? ImGui::CalcTextSize(desc).y : 0.0f;
    const float title_h = ImGui::CalcTextSize(title).y;
    const float card_h = desc_h > 0 ? title_h + desc_h + 8.0f : title_h;
    ImVec2 card_pos = window->DC.CursorPos;
    
    static std::map<ImGuiID, FeatureCardAnimState> anim;
    FeatureCardAnimState& st = anim[id];
    
    bool toggled = false;
    if (ImGui::InvisibleButton(title, ImVec2(avail_w, card_h))) {
        *expanded = !(*expanded);
    }
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        bool old_enabled = *enabled;
        *enabled = !(*enabled);
        if (old_enabled != *enabled) {
            NotifyFeatureCardSwitch(title, old_enabled, *enabled);
            toggled = true;
        }
    }
    
    float dt = g.IO.DeltaTime;
    st.switch_knob_x = ImLerp(st.switch_knob_x, *enabled ? sw_travel :0.0f, dt *10.0f);
    float switch_t = sw_travel > 0.0f ? st.switch_knob_x / sw_travel : 0.0f;
    
    ImDrawList* dl = window->DrawList;
    
    float extra_h = ImMax(0.0f, st.bg_bottom_y);
    float visual_bottom_y = card_pos.y + card_h + extra_h * st.expand_progress;
    ImVec2 card_end = ImVec2(card_pos.x + avail_w, visual_bottom_y);
    
    ImVec4 bg = Colors::dark_mode ? ImVec4(0.08f,0.07f,0.11f,0.95f) : 
                (*enabled ? ImVec4(0.92f,0.96f,1.0f,0.96f) : ImVec4(0.98f,0.98f,0.99f,0.95f));
    ImVec4 border = Colors::dark_mode ? ImVec4(0.22f,0.20f,0.28f,0.80f) : 
                    ImVec4(0.88f,0.90f,0.94f,0.90f);
    
    dl->AddRectFilled(card_pos, card_end, GetColorU32Ex(bg), round);
    dl->AddRect(card_pos, card_end, GetColorU32Ex(border), round, 0, 1.0f);
    
    float tx = card_pos.x + card_pad;
    float ty = card_pos.y + (card_h - text_h) * 0.5f;
    dl->AddText(ImVec2(tx, ty), GetColorU32Ex(st.title_color), title);
    
    if (desc && desc_sz.y > 0) {
        dl->AddText(ImVec2(tx, ty + title_h + 4.0f), GetColorU32Ex(st.desc_color), desc);
    }
    
    // 绘制Switch
    ImVec2 sw_pos(card_pos.x + avail_w - sw_w - card_pad, card_pos.y + (card_h - sw_h) * 0.5f);
    ImVec2 sw_min(sw_pos.x, sw_pos.y);
    ImVec2 sw_max(sw_pos.x + sw_w, sw_pos.y + sw_h);
    float pill_r = sw_h * 0.5f;
    
    float tr = Colors::dark_mode ? 0.20f : 0.82f, tg = Colors::dark_mode ? 0.18f : 0.84f, tb = Colors::dark_mode ? 0.25f : 0.86f;
    float tr_on = Colors::accent.x, tg_on = Colors::accent.y, tb_on = Colors::accent.z;
    float sw_track_r = ImLerp(tr, tr_on, switch_t);
    float sw_track_g = ImLerp(tg, tg_on, switch_t);
    float sw_track_b = ImLerp(tb, tb_on, switch_t);
    
    float sw_alpha = g_custom_draw_alpha;
    ImU32 sw_track_col = IM_COL32((int)(sw_track_r * 255), (int)(sw_track_g * 255), (int)(sw_track_b * 255), (int)(255 * sw_alpha));
    dl->AddRectFilled(sw_min, sw_max, sw_track_col, pill_r);
    
    float kn_cx = sw_min.x + st.switch_knob_x + sw_h * 0.5f;
    float kn_cy = sw_min.y + sw_h * 0.5f;
    float kn_r = sw_h * 0.4f;
    
    dl->AddCircleFilled(ImVec2(kn_cx + 1.0f, kn_cy + 1.5f), kn_r, IM_COL32(0, 0, 0, (int)(30 * sw_alpha)), 16);
    dl->AddCircleFilled(ImVec2(kn_cx, kn_cy), kn_r, 
                        Colors::dark_mode ? IM_COL32(232,222,255,(int)(255*sw_alpha)) : IM_COL32(255,255,255,(int)(255*sw_alpha)), 16);
    
    ImGui::SetCursorPosY(card_pos.y + card_h + extra_h * st.expand_progress + 8.0f);
    
    return toggled;
}

inline void FeatureCardSeparator() {
    ImGuiWindow* window = GetCurrentWindow();
    ImDrawList* dl = window->DrawList;
    ImVec2 pos = window->DC.CursorPos;
    float width = ImGui::GetContentRegionAvail().x;
    
    dl->AddLine(pos + ImVec2(12.0f, 0.0f), pos + ImVec2(width - 12.0f, 0.0f), 
                IM_COL32(148, 163, 184, 46), 1.0f);
    ImGui::Dummy(ImVec2(0, 10));
}

inline bool FeatureCard(const char* title, const char* desc, bool* enabled, bool* expanded, bool* shortcut_enabled = nullptr) {
    bool toggled = FeatureCardHeader(title, desc, enabled, expanded);
    
    if (*expanded) {
        ImGui::Dummy(ImVec2(0.0f, 14.0f));
        ImGui::Indent(18.0f);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    }
    
    return toggled;
}

inline void FeatureCardEnd(bool* expanded) {
    if (*expanded) {
        ImGui::PopItemWidth();
        ImGui::Unindent(18.0f);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
    }
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
}
