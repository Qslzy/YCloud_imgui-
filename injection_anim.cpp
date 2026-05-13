#include "injection_anim.h"
#include "imgui_internal.h"
#include <cmath>
#include <cstdio>
float InjectionAnim::EaseOutElastic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    const float c = 2.0f * IM_PI / 3.0f;
    return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c) + 1.0f;
}
float InjectionAnim::EaseOutCubic(float t)   { return 1.0f - powf(1.0f - t, 3.0f); }
float InjectionAnim::EaseInOutCubic(float t) {
    return t < 0.5f ? 4.0f*t*t*t : 1.0f - powf(-2.0f*t+2.0f, 3.0f)/2.0f;
}
float InjectionAnim::EaseOutBack(float t) {
    const float c1=1.70158f, c3=c1+1.0f;
    return 1.0f + c3*powf(t-1.0f,3.0f) + c1*powf(t-1.0f,2.0f);
}
float InjectionAnim::EaseOutBounce(float t) {
    const float n1=7.5625f, d1=2.75f;
    if (t<1.0f/d1) return n1*t*t;
    if (t<2.0f/d1) { t-=1.5f/d1; return n1*t*t+0.75f; }
    if (t<2.5f/d1) { t-=2.25f/d1; return n1*t*t+0.9375f; }
    t-=2.625f/d1; return n1*t*t+0.984375f;
}
float InjectionAnim::EaseOutQuint(float t)  { return 1.0f - powf(1.0f-t, 5.0f); }
float InjectionAnim::EaseInQuint(float t)   { return powf(t, 5.0f); }
static inline float Lerp(float a, float b, float t) { return a + (b-a)*t; }
void InjectionAnim::Fire() {
    if (triggered) return;
    triggered = true; playing = true; done = false;
    elapsed = 0.0f; _particle_phase = 0.0f;
    _prev_total_scale = 0.0f; _ring_rotation = 0.0f;
}
void InjectionAnim::Render() {
    if (!playing) return;
    auto& io   = ImGui::GetIO();
    auto* dl   = ImGui::GetForegroundDrawList();
    const float us = g_ui_scale;
    const ImVec2 scr(io.DisplaySize.x, io.DisplaySize.y);
    const ImVec2 cnt(scr.x * 0.5f, scr.y * 0.5f);
    const float diag = sqrtf(scr.x*scr.x + scr.y*scr.y);
    const float ring_r = scr.y * 0.25f;
    const float ball_r = g_ball_radius * us;
    const float text_fs = scr.y * 0.07f;
    elapsed += io.DeltaTime;
    const float T_SQUARE_BEG = 0.00f, T_SQUARE_END = 1.34f;
    const float T_BG_DARK    = 1.34f;
    const float T_RING_BEG   = 0.88f, T_RING_END   = 1.68f;  
    const float T_TEXT_BEG   = 2.08f, T_TEXT_END   = 3.00f;
    const float T_BREATH_BEG = 2.78f, T_BREATH_END = 5.08f;
    const float T_SHRINK_BEG = 4.62f, T_SHRINK_END = 5.42f;
    const float T_RING2BALL_BEG = 4.98f, T_RING2BALL_END = 5.78f;
    const float T_PULSE_BEG  = 5.44f, T_PULSE_END  = 6.02f;
    const float T_BURST_BEG  = 5.92f, T_BURST_END  = 7.08f;
    const float T_FADE_BEG   = 6.68f, T_FADE_END   = 9.00f;
    if (elapsed >= T_FADE_END) { playing = false; done = true; return; }
    const int   SQ_N = 10;
    const float sq_radius  = ring_r * 0.82f;  
    float sq_size = text_fs * 0.85f;           
    float sq_phase2 = 0.0f; 
    float sq_phase3 = 0.0f; 
    float sq_alpha  = 0.0f;
    if (elapsed < 0.35f) {
        sq_alpha = elapsed / 0.35f;
        sq_phase2 = 0.0f;
    } else if (elapsed < 1.05f) {
        sq_alpha = 1.0f;
        sq_phase2 = (elapsed - 0.35f) / 0.70f;
    } else {
        sq_alpha = 1.0f - (elapsed - 1.05f) / 0.45f;
        sq_phase2 = 1.0f;
        sq_phase3 = (elapsed - 0.85f) / 0.65f;
        if (sq_phase3 > 1.0f) sq_phase3 = 1.0f;
    }
    if (sq_alpha < 0.0f) sq_alpha = 0.0f;
    const int STARS_N = 360;
    float star_alpha[STARS_N] = {}, star_x[STARS_N] = {}, star_y[STARS_N] = {};
    float star_px[STARS_N] = {}, star_py[STARS_N] = {};  
    if (elapsed >= 0.24f && elapsed < 2.05f) {
        float slt = (elapsed - 0.24f) / 1.81f;
        if (slt > 1.0f) slt = 1.0f;
        for (int i = 0; i < STARS_N; ++i) {
            star_px[i] = star_x[i]; star_py[i] = star_y[i];
            float s1 = (float)i * 97.13f + (float)(i * i) * 0.021f;
            float s2 = (float)i * 173.71f + (float)(i * i * i) * 0.00091f;
            int cols = 30;
            int gx_i = i % cols;
            int gy_i = i / cols;
            float cell_w = scr.x / (float)cols;
            float cell_h = scr.y / 13.0f;
            float jitter_x = sinf(s1 * 0.61f) * cell_w * 0.28f;
            float jitter_y = cosf(s2 * 0.47f) * cell_h * 0.22f;
            float sx = gx_i * cell_w + cell_w * 0.5f + jitter_x;
            float sy = fmodf((float)gy_i * cell_h + cell_h * 0.5f + jitter_y + scr.y * 0.17f, scr.y);
            float band = floorf((sy / scr.y) * 18.0f);
            float scan = sinf(elapsed * 18.0f + band * 1.7f) * (18.0f + 12.0f * sinf(s1));
            sx += scan * us;
            float ta = (float)(i * 1543 % STARS_N) / (float)STARS_N * 2.0f * IM_PI;
            float lane = ((i % 5) - 2) * 0.018f;
            float target_r = ring_r * (0.86f + lane);
            float tx = cnt.x + cosf(ta) * target_r;
            float ty = cnt.y + sinf(ta) * target_r;
            float delay = fmodf(fabsf(sinf(s1 * 0.37f)) * 0.28f + (float)(gx_i % 6) * 0.012f, 0.34f);
            float cr = (slt - delay) / (0.58f - delay * 0.35f);
            float conv = (cr < 0.0f) ? 0.0f : ((cr > 1.0f) ? 1.0f : cr);
            float ec = EaseInOutCubic(conv);
            float midx = sx + (tx - sx) * EaseOutCubic(conv);
            float midy = sy;
            float turn = conv < 0.55f ? (conv / 0.55f) : 1.0f;
            float drop = conv > 0.35f ? ((conv - 0.35f) / 0.65f) : 0.0f;
            if (drop > 1.0f) drop = 1.0f;
            star_x[i] = sx + (midx - sx) * turn;
            star_y[i] = sy + (ty - sy) * EaseOutCubic(drop);
            star_x[i] = star_x[i] + (tx - star_x[i]) * (ec * 0.35f);
            float ed = fmodf(fabsf(cosf(s2 * 0.29f)) * 0.10f, 0.10f);
            float er = (slt - 0.62f - ed) / 0.34f;
            float wipe = (er < 0.0f) ? 0.0f : ((er > 1.0f) ? 1.0f : er);
            float dir = (i & 1) ? 1.0f : -1.0f;
            star_x[i] += dir * wipe * wipe * ring_r * (0.30f + 0.45f * fabsf(sinf(s2)));
            star_y[i] += sinf(s2 + elapsed * 9.0f) * wipe * 5.0f * us;
            float fi_d = (float)(gx_i % 9) * 0.010f + fmodf(fabsf(sinf(s1 * 0.91f)) * 0.06f, 0.06f);
            float fi = (slt < fi_d) ? 0.0f : ((slt - fi_d) / 0.10f);
            if (fi > 1.0f) fi = 1.0f;
            float fo = (slt > 0.64f) ? (1.0f - (slt - 0.64f) / 0.36f) : 1.0f;
            if (fo < 0.0f) fo = 0.0f;
            float glitch = (sinf(elapsed * 13.0f + s1) > 0.94f && slt > 0.24f) ? 0.68f : 1.0f;
            star_alpha[i] = fi * fo * glitch * 0.86f;
        }
    }
    float bg_alpha = 0.0f;
    if (elapsed >= 0.50f && elapsed < 0.86f) {
        bg_alpha = EaseInOutCubic((elapsed - 0.50f) / 0.36f) * 0.76f;
    } else if (elapsed >= 0.86f && elapsed < 1.10f) {
        float lt = (elapsed - 0.86f) / 0.24f;
        if (lt > 1.0f) lt = 1.0f;
        bg_alpha = Lerp(0.76f, 0.84f, EaseOutCubic(lt));
    } else if (elapsed >= 1.10f && elapsed < T_RING_END) {
        float lt = (elapsed - 1.10f) / (T_RING_END - 1.10f);
        if (lt > 1.0f) lt = 1.0f;
        bg_alpha = Lerp(0.84f, 0.78f, EaseOutCubic(lt));
    } else if (elapsed >= T_RING_END && elapsed < T_SHRINK_BEG) {
        float pulse = 0.5f + 0.5f * sinf(elapsed * 2.55f);
        bg_alpha = 0.80f + pulse * 0.075f;
    } else if (elapsed >= T_SHRINK_BEG && elapsed < T_BURST_END) {
        float lt = (elapsed - T_SHRINK_BEG) / (T_BURST_END - T_SHRINK_BEG);
        if (lt > 1.0f) lt = 1.0f;
        bg_alpha = Lerp(0.78f, 0.0f, EaseInOutCubic(lt));
    } else if (elapsed >= T_BURST_END) {
        bg_alpha = 0.0f;
    }
    float ring_scale=0.0f, ring_aspect=0.28f, ring_alpha=0.0f;
    float ring_pop_flash=0.0f, ring_pop_wave=0.0f, ring_chroma=0.0f, ring_slash=0.0f;
    if (elapsed >= T_RING_BEG && elapsed < T_RING_END) {
        float lt = (elapsed - T_RING_BEG) / (T_RING_END - T_RING_BEG);
        if (lt > 1.0f) lt = 1.0f;
        float pre = lt / 0.18f;
        if (pre < 0.0f) pre = 0.0f; if (pre > 1.0f) pre = 1.0f;
        float pop_t = (lt - 0.10f) / 0.90f;
        if (pop_t < 0.0f) pop_t = 0.0f; if (pop_t > 1.0f) pop_t = 1.0f;
        float base = EaseInOutCubic(pop_t);
        float overshoot = sinf(pop_t * 2.2f * IM_PI) * expf(-5.6f * pop_t) * 0.32f;
        float recoil = sinf(pop_t * 4.0f * IM_PI) * expf(-6.8f * pop_t) * 0.22f;
        ring_scale  = 0.06f + 0.98f * base + 0.11f * overshoot;
        ring_aspect = 0.20f + 0.80f * EaseInOutCubic(pre) + 0.05f * recoil;
        ring_alpha  = (lt < 0.12f) ? (lt / 0.12f) : 1.0f;
        ring_pop_flash = (lt < 0.20f) ? (1.0f - lt / 0.20f) : 0.0f;
        ring_pop_wave  = (lt > 0.08f && lt < 0.72f) ? ((lt - 0.08f) / 0.64f) : 0.0f;
        ring_chroma    = (1.0f - EaseOutCubic(lt)) * 1.15f;
        ring_slash     = (lt < 0.55f) ? (1.0f - lt / 0.55f) : 0.0f;
        if (ring_scale < 0.02f) ring_scale = 0.02f;
        if (ring_aspect < 0.18f) ring_aspect = 0.18f;
        if (ring_aspect > 1.22f) ring_aspect = 1.22f;
    } else if (elapsed >= T_RING_END) { ring_scale = 1.0f; ring_aspect = 1.0f; ring_alpha = 1.0f; }
    float bounce_glow=0.0f, breath_scale=1.0f, breath_glow=0.0f;
    if (elapsed >= T_RING_END && elapsed < T_BREATH_BEG) {
        float lt = (elapsed - T_RING_END) / (T_BREATH_BEG - T_RING_END);
        if (lt > 1.0f) lt = 1.0f;
        bounce_glow = (1.0f - EaseOutCubic(lt)) * 0.40f;  
    }
    if(elapsed>=T_BREATH_BEG && elapsed<T_BREATH_END){
        float phase=(elapsed-T_BREATH_BEG)*3.7f;
        breath_scale=1.0f+sinf(phase)*0.09f;
        breath_glow=(1.0f-sinf(phase))*0.35f;
        float in=(elapsed-T_BREATH_BEG)/0.25f, out=(T_BREATH_END-elapsed)/0.30f;
        float fade=(in<1.0f)?in:((out<1.0f)?out:1.0f); if(fade<0.0f)fade=0.0f;
        breath_scale=1.0f+(breath_scale-1.0f)*fade; breath_glow*=fade;
    }
    float text_alpha=0.0f, text_scale=0.3f, text_flash=0.0f, text_burst=0.0f;
    if(elapsed>=T_TEXT_BEG && elapsed<T_TEXT_END){
        float lt=(elapsed-T_TEXT_BEG)/(T_TEXT_END-T_TEXT_BEG);
        if(lt>1.0f)lt=1.0f;
        float raw=EaseInOutCubic(lt);
        text_alpha=EaseOutCubic(lt);
        text_scale=0.88f+0.12f*raw;
        text_flash=(lt<0.14f)?(1.0f-EaseOutCubic(lt/0.14f))*0.24f:0.0f;
        text_burst=(lt>0.72f)?EaseInOutCubic((lt-0.72f)/0.28f)*0.24f:0.0f;
    } else if(elapsed>=T_TEXT_END && elapsed<T_SHRINK_BEG){
        text_alpha=1.0f; text_scale=1.0f;
        float bt=(elapsed-T_TEXT_END)/0.70f; if(bt>1.0f)bt=1.0f;
        text_burst=(1.0f-EaseOutCubic(bt))*0.18f;
    }
    const int TEXTP_N=36;
    float textp_alpha[TEXTP_N]={}, textp_x[TEXTP_N]={}, textp_y[TEXTP_N]={};
    const float textp_arrival=T_TEXT_END-0.06f;
    for(int i=0;i<TEXTP_N;++i){
        float start_angle=(float)i/TEXTP_N*2.0f*IM_PI;
        if(i<24){ 
            float start_r=diag*0.55f;
            float sx=cnt.x+cosf(start_angle)*start_r;
            float sy=cnt.y+sinf(start_angle)*start_r;
            float delay=(float)i*0.010f;
            float dur=(textp_arrival-T_TEXT_BEG-delay); if(dur<0.02f)dur=0.02f;
            float lt=(elapsed-T_TEXT_BEG-delay)/dur;
            if(lt<0.0f)lt=0.0f; if(lt>1.0f)lt=1.0f;
            float e=lt*lt; 
            float cp_dist=start_r*0.35f*(1.0f-e);
            float cp_angle=start_angle+1.2f*(1.0f-e)*((i%3==0)?1.0f:-1.0f);
            float cpx=cnt.x+cosf(cp_angle)*cp_dist;
            float cpy=cnt.y+sinf(cp_angle)*cp_dist;
            float t_q=e;
            float u=1.0f-t_q;
            textp_x[i]=u*u*sx+2*u*t_q*cpx+t_q*t_q*cnt.x;
            textp_y[i]=u*u*sy+2*u*t_q*cpy+t_q*t_q*cnt.y;
            float fi=(lt<0.18f)?(lt/0.18f):1.0f;
            float fo=(lt>0.75f)?(1.0f-(lt-0.75f)/0.25f):1.0f;
            textp_alpha[i]=fi*fo;
            if(elapsed>=T_TEXT_END+0.1f)textp_alpha[i]=0.0f;
        } else { 
            int si=i-24;
            float sa=start_angle+(float)(si*47)*0.11f;
            float sd=T_TEXT_END+(float)si*0.022f;
            if(elapsed>=sd && elapsed<sd+0.60f){
                float sl=(elapsed-sd)/0.60f; if(sl>1.0f)sl=1.0f;
                float se=sl*sl, dist=ring_r*(0.25f+se*1.8f);
                textp_x[i]=cnt.x+cosf(sa)*dist;
                textp_y[i]=cnt.y+sinf(sa)*dist;
                textp_alpha[i]=1.0f-sl*sl;
            }
        }
    }
    if(elapsed>=T_SHRINK_BEG && elapsed<T_SHRINK_END){
        float lt=(elapsed-T_SHRINK_BEG)/(T_SHRINK_END-T_SHRINK_BEG);
        if(lt>1.0f)lt=1.0f;
        float ei=lt*lt*lt;
        text_alpha=1.0f-ei; text_scale=1.0f-ei*0.85f;
    } else if(elapsed>=T_SHRINK_END) text_alpha=0.0f;
    float ring_shrink=1.0f;
    if(elapsed>=T_RING2BALL_BEG && elapsed<T_RING2BALL_END){
        float lt=(elapsed-T_RING2BALL_BEG)/(T_RING2BALL_END-T_RING2BALL_BEG);
        if(lt>1.0f)lt=1.0f;
        ring_shrink=1.0f+(ball_r/ring_r-1.0f)*EaseInOutCubic(lt);
    } else if(elapsed>=T_RING2BALL_END) ring_shrink=ball_r/ring_r;
    float current_ring_r=ring_r*ring_scale*breath_scale*ring_shrink;
    float converge_intensity = 0.0f;   
    float ball_glow = 0.0f;           
    const int CV_N = 30;
    float cv_alpha[CV_N] = {}, cv_x[CV_N] = {}, cv_y[CV_N] = {};
    if (elapsed >= T_PULSE_BEG && elapsed < T_PULSE_END) {
        float lt = (elapsed - T_PULSE_BEG) / (T_PULSE_END - T_PULSE_BEG);
        if (lt > 1.0f) lt = 1.0f;
        converge_intensity = EaseInQuint(lt);
        ball_glow = lt * 1.2f; if (ball_glow > 1.0f) ball_glow = 1.0f;
        for (int i = 0; i < CV_N; ++i) {
            float ba = (float)i / CV_N * 2.0f * IM_PI;
            float start_r = diag * 0.55f;
            float sx = cnt.x + cosf(ba) * start_r;
            float sy = cnt.y + sinf(ba) * start_r;
            float delay = (float)i * 0.006f;
            float dur = (T_PULSE_END - T_PULSE_BEG - delay);
            if (dur < 0.02f) dur = 0.02f;
            float pt = (elapsed - T_PULSE_BEG - delay) / dur;
            if (pt < 0.0f) pt = 0.0f; if (pt > 1.0f) pt = 1.0f;
            float e = pt * pt;
            float cp_dist = start_r * 0.30f * (1.0f - e);
            float cp_angle = ba + 1.5f * (1.0f - e) * ((i & 1) ? 1.0f : -1.0f);
            float cpx = cnt.x + cosf(cp_angle) * cp_dist;
            float cpy = cnt.y + sinf(cp_angle) * cp_dist;
            float u = 1.0f - e;
            cv_x[i] = u*u*sx + 2*u*e*cpx + e*e*cnt.x;
            cv_y[i] = u*u*sy + 2*u*e*cpy + e*e*cnt.y;
            float fi = (pt < 0.15f) ? (pt / 0.15f) : 1.0f;
            cv_alpha[i] = fi * (1.0f - e * 0.8f);
        }
    }
    const int BWAVES = 5;  
    float bwave_r[BWAVES] = {}, bwave_a[BWAVES] = {}, bwave_t[BWAVES] = {};
    float burst_flash = 0.0f;   
    float burst_particles_phase = 0.0f;
    if (elapsed >= T_BURST_BEG && elapsed < T_BURST_END) {
        float lt = (elapsed - T_BURST_BEG) / (T_BURST_END - T_BURST_BEG);
        if (lt > 1.0f) lt = 1.0f;
        burst_flash = (lt < 0.12f) ? (1.0f - lt / 0.12f) * 1.00f : 0.0f;
        for (int w = 0; w < BWAVES; ++w) {
            float w_delay = w * 0.075f;
            float w_lt = (lt - w_delay) / (1.0f - w_delay);
            if (w_lt < 0.0f) w_lt = 0.0f;
            if (w_lt > 1.0f) w_lt = 1.0f;
            float we = EaseInOutCubic(w_lt);
            bwave_r[w] = ball_r * 0.20f + we * diag * (1.34f + w * 0.035f);
            float peak_a = 1.18f - w * 0.115f;
            if (peak_a < 0.55f) peak_a = 0.55f;
            bwave_a[w] = peak_a * (1.0f - we * 0.58f) * (1.0f - w_lt * 0.18f);
            bwave_t[w] = Lerp(10.5f - w * 0.55f, 2.2f, we) * us;  
        }
        burst_particles_phase = lt * 4.0f;
    }
    float fade_out=1.0f;
    if(elapsed>=T_FADE_BEG){
        float lt=(elapsed-T_FADE_BEG)/(T_FADE_END-T_FADE_BEG);
        if(lt>1.0f)lt=1.0f;
        fade_out=1.0f-EaseOutCubic(lt);
        if(fade_out<0.0f)fade_out=0.0f;
    }
    _particle_phase+=io.DeltaTime*2.8f;
    dl->PushClipRectFullScreen();
    float global_alpha=fade_out;
    if(global_alpha<0.001f){dl->PopClipRect(); return;}
    if (sq_alpha > 0.005f && elapsed < T_SQUARE_END) {
        float ease2 = EaseOutBack(sq_phase2);
        float ease3 = sq_phase3 * sq_phase3;
        for (int i = 0; i < SQ_N; ++i) {
            float base_angle = (float)i / SQ_N * 2.0f * IM_PI;
            float rot_angle = base_angle + ease3 * 2.5f;
            float dist = sq_radius * ease2;
            float sx = cnt.x + cosf(rot_angle) * dist;
            float sy = cnt.y + sinf(rot_angle) * dist;
            float sq_rot = 0.785f + base_angle + ease3 * 1.8f;  
            float hs = sq_size * 0.5f * (1.0f - ease3 * 0.6f);
            float c = cosf(sq_rot), s = sinf(sq_rot);
            ImVec2 corners[4] = {
                {sx + (-hs*c - -hs*s), sy + (-hs*s + -hs*c)},
                {sx + ( hs*c - -hs*s), sy + ( hs*s + -hs*c)},
                {sx + ( hs*c -  hs*s), sy + ( hs*s +  hs*c)},
                {sx + (-hs*c -  hs*s), sy + (-hs*s +  hs*c)},
            };
            float color_t = ease2 * (1.0f - ease3 * 0.7f);
            int gray = (int)(240.0f - color_t * 210.0f);
            if (gray < 30) gray = 30;
            ImU32 col = IM_COL32(gray, gray, gray, (int)(sq_alpha * 255.0f));
            for (int j = 0; j < 4; ++j)
                dl->AddLine(corners[j], corners[(j+1)%4], col, 1.8f * us);
        }
    }
    float bg_final_alpha = bg_alpha;
    if (elapsed >= T_FADE_BEG && bg_final_alpha > 0.0f) {
        float lt = (elapsed - T_FADE_BEG) / (T_FADE_END - T_FADE_BEG);
        if (lt > 1.0f) lt = 1.0f;
        bg_final_alpha *= (1.0f - EaseOutCubic(lt));
    }
    const float T_FOLD_BG_BEG = T_SHRINK_BEG + 0.04f;
    const float T_FOLD_BG_END = T_BURST_BEG + 0.10f;
    bool fold_backplate = (elapsed >= T_FOLD_BG_BEG && elapsed < T_FOLD_BG_END);
    if (fold_backplate) {
        float fold_in = (elapsed - T_FOLD_BG_BEG) / 0.18f;
        if (fold_in < 0.0f) fold_in = 0.0f; if (fold_in > 1.0f) fold_in = 1.0f;
        float fold_out = (T_FOLD_BG_END - elapsed) / 0.34f;
        if (fold_out < 0.0f) fold_out = 0.0f; if (fold_out > 1.0f) fold_out = 1.0f;
        float fold_gate = EaseOutCubic(fold_in) * EaseOutCubic(fold_out);
        float fold_panel_alpha = 0.68f * fold_gate * global_alpha;
        if (bg_final_alpha < fold_panel_alpha) bg_final_alpha = fold_panel_alpha;
    }
    if (bg_final_alpha > 0.001f) {
        float base_alpha = bg_final_alpha;
        bool gravity_mask = (elapsed >= 0.78f && elapsed < T_PULSE_BEG);
        if (gravity_mask && !fold_backplate) base_alpha *= 0.82f;
        dl->AddRectFilled(ImVec2(0,0), scr, IM_COL32(0,0,0,(int)(base_alpha*255.0f)));
        float radial_gate = 0.0f;
        if (elapsed >= 0.70f && elapsed < T_SHRINK_BEG) {
            float ri = (elapsed - 0.70f) / 0.28f;
            if (ri < 0.0f) ri = 0.0f; if (ri > 1.0f) ri = 1.0f;
            radial_gate = EaseOutCubic(ri);
        } else if (elapsed >= T_SHRINK_BEG && elapsed < T_FOLD_BG_END) {
            radial_gate = fold_backplate ? 1.0f : 0.75f;
        }
        if (radial_gate > 0.001f) {
            float breathe = 0.5f + 0.5f * sinf(elapsed * 2.45f + 0.6f);
            float vortex_a = radial_gate * global_alpha;
            const int VORTEX = 42;
            for (int i = VORTEX; i >= 0; --i) {
                float k = (float)i / (float)VORTEX;
                float inv = 1.0f - k;
                float soft = inv * inv * (3.0f - 2.0f * inv);
                float rr = ring_r * (0.70f + k * 5.25f + breathe * 0.15f);
                float aa = vortex_a * bg_final_alpha * (0.018f + soft * 0.082f);
                if (elapsed >= 0.80f && elapsed < 3.00f) aa *= 1.24f;
                if (fold_backplate) aa *= 1.16f;
                if (i < 8) aa *= 1.22f;
                if (aa > 0.001f)
                    dl->AddCircleFilled(cnt, rr, IM_COL32(0, 0, 0, (int)(aa * 255.0f)), 192);
            }
            float core_a = vortex_a * bg_final_alpha * 0.22f;
            if (elapsed >= 0.80f && elapsed < 3.00f) core_a *= 1.32f;
            if (fold_backplate) core_a *= 1.18f;
            if (core_a > 0.001f)
                dl->AddCircleFilled(cnt, ring_r * (1.32f + breathe * 0.055f), IM_COL32(0, 0, 0, (int)(core_a * 255.0f)), 192);
        }
    }
    if (elapsed >= 7.72f && elapsed < T_FADE_END) {
        float mt = (elapsed - 7.72f) / (T_FADE_END - 7.72f);
        if (mt < 0.0f) mt = 0.0f; if (mt > 1.0f) mt = 1.0f;
        float me = EaseInOutCubic(mt);
        float mask_alive = 1.0f - EaseOutCubic(mt * 0.72f);
        if (mask_alive < 0.0f) mask_alive = 0.0f;
        float outer_r = Lerp(diag * 0.66f, ring_r * 1.62f, me);
        float inner_bias = 0.5f + 0.5f * sinf(elapsed * 2.10f);
        const int FINAL_MASK_LAYERS = 34;
        for (int i = FINAL_MASK_LAYERS; i >= 0; --i) {
            float k = (float)i / (float)FINAL_MASK_LAYERS;
            float inv = 1.0f - k;
            float near_core = inv * inv * (3.0f - 2.0f * inv);
            float rr = Lerp(ring_r * (1.02f + inner_bias * 0.025f), outer_r, k);
            float aa = global_alpha * mask_alive * (0.010f + near_core * 0.105f);
            if (i < 7) aa *= 1.28f;
            if (aa > 0.001f)
                dl->AddCircleFilled(cnt, rr, IM_COL32(0, 0, 0, (int)(aa * 255.0f)), 192);
        }
        float core_mask_a = global_alpha * mask_alive * (0.16f + 0.06f * me);
        if (core_mask_a > 0.001f)
            dl->AddCircleFilled(cnt, ring_r * (1.16f + inner_bias * 0.035f), IM_COL32(0, 0, 0, (int)(core_mask_a * 255.0f)), 192);
    }
    if (elapsed >= 0.34f && elapsed < 1.54f) {
        float ot = (elapsed - 0.34f) / 1.20f;
        if (ot < 0.0f) ot = 0.0f; if (ot > 1.0f) ot = 1.0f;
        float in_a = ot < 0.18f ? EaseOutCubic(ot / 0.18f) : 1.0f;
        float out_a = ot > 0.76f ? (1.0f - EaseOutCubic((ot - 0.76f) / 0.24f)) : 1.0f;
        float oa = in_a * out_a * global_alpha;
        const int OLINES = 8;
        const int OSEG = 18;
        for (int l = 0; l < OLINES; ++l) {
            float seed = l * 1.973f + 0.41f;
            float side = (l & 1) ? 1.0f : -1.0f;
            float ybias = (((float)(l % 4) - 1.5f) / 1.5f) * ring_r * 0.70f;
            ImVec2 p0(cnt.x + side * scr.x * (0.47f + 0.035f * sinf(seed)), cnt.y + ybias + sinf(seed) * 18.0f * us);
            ImVec2 p1(cnt.x + side * ring_r * (1.95f + 0.18f * cosf(seed)), cnt.y + ybias * 0.62f - ring_r * 0.42f * side);
            ImVec2 p2(cnt.x - side * ring_r * (0.70f + 0.10f * sinf(seed)), cnt.y - ybias * 0.24f + ring_r * 0.34f * side);
            float lock = EaseInOutCubic(ot);
            ImVec2 p3(cnt.x + cosf(seed + ot * 2.0f) * ring_r * (0.42f - 0.18f * lock),
                      cnt.y + sinf(seed + ot * 2.0f) * ring_r * (0.30f - 0.12f * lock));
            ImVec2 pts[OSEG + 1];
            float head = EaseInOutCubic(ot);
            float tail = head - 0.44f;
            if (tail < 0.0f) tail = 0.0f;
            for (int s = 0; s <= OSEG; ++s) {
                float t = tail + (head - tail) * (float)s / (float)OSEG;
                float u = 1.0f - t;
                pts[s] = ImVec2(u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
                                u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y);
            }
            float la = oa * (0.11f + 0.035f * (l % 3));
            if (la > 0.004f) {
                dl->AddPolyline(pts, OSEG + 1, IM_COL32(92, 165, 235, (int)(la * 255.0f)), ImDrawFlags_None, 1.0f * us);
                ImVec2 hp = pts[OSEG];
                dl->AddCircleFilled(hp, 1.8f * us, IM_COL32(135, 205, 250, (int)(la * 240.0f)), 8);
            }
        }
    }
    if (elapsed >= 0.12f && elapsed < 2.08f) {
        float ft = (elapsed - 0.12f) / 1.96f;
        if (ft < 0.0f) ft = 0.0f; if (ft > 1.0f) ft = 1.0f;
        float fin = ft < 0.22f ? EaseInOutCubic(ft / 0.22f) : 1.0f;
        float fout = ft > 0.62f ? (1.0f - EaseInOutCubic((ft - 0.62f) / 0.38f)) : 1.0f;
        if (fout < 0.0f) fout = 0.0f;
        float flat_denoise = ft > 0.64f ? (1.0f - (ft - 0.64f) / 0.36f * 0.28f) : 1.0f;
        if (flat_denoise < 0.0f) flat_denoise = 0.0f;
        float fa_base = fin * fout * flat_denoise * global_alpha;
        const int FLAT_CARDS = 10;
        for (int i = 0; i < FLAT_CARDS; ++i) {
            float row = (float)i - (float)(FLAT_CARDS - 1) * 0.5f;
            float delay = (float)i * 0.024f;
            float ct = (ft - delay) / 0.48f;
            if (ct < 0.0f) ct = 0.0f; if (ct > 1.0f) ct = 1.0f;
            float ce = EaseInOutCubic(ct);
            float side = (i & 1) ? 1.0f : -1.0f;
            float y = cnt.y + row * text_fs * 0.34f;
            float w = ring_r * (0.16f + 0.046f * (float)((i * 7) % 9));
            float h = 1.6f * us + (float)(i % 3) * 0.85f * us;
            float x0 = cnt.x + side * (ring_r * (1.45f - ce * 1.02f)) - w * 0.5f;
            float x1 = x0 + w * (0.22f + 0.78f * ce);
            float alpha = fa_base * (0.105f + 0.032f * (float)(i % 4));
            if (alpha < 0.004f) continue;
            int r = 118 + (i % 4) * 12;
            int g = 138 + (i % 4) * 14;
            int b = 158 + (i % 4) * 16;
            dl->AddRectFilled(ImVec2(x0, y - h), ImVec2(x1, y + h),
                IM_COL32(r, g, b, (int)(alpha * 255.0f)), 1.5f * us);
            if ((i % 4) == 0) {
                dl->AddLine(ImVec2(x1, y - h * 1.8f), ImVec2(x1 + 14.0f * us * ce, y - h * 1.8f),
                    IM_COL32(170, 205, 235, (int)(alpha * 150.0f)), 1.0f * us);
            }
        }
        const int BARS = 48;
        for (int i = 0; i < BARS; ++i) {
            float a = (float)i / (float)BARS * 2.0f * IM_PI;
            float delay = fmodf((float)(i * 17), 23.0f) / 23.0f * 0.24f;
            float bt = (ft - 0.16f - delay) / 0.50f;
            if (bt < 0.0f) bt = 0.0f; if (bt > 1.0f) bt = 1.0f;
            float be = EaseInOutCubic(bt);
            float kill = ft > 0.66f ? (1.0f - EaseOutCubic((ft - 0.66f) / 0.34f)) : 1.0f;
            if (kill < 0.0f) kill = 0.0f;
            float len = ring_r * (0.030f + 0.042f * fabsf(sinf((float)i * 2.13f)));
            float r0 = ring_r * (1.23f + 0.09f * (1.0f - be));
            float r1 = r0 + len * be;
            float wob = sinf(ft * 4.2f + (float)i * 0.37f) * 0.006f * ring_r;
            float alpha = fa_base * kill * (0.14f + 0.16f * be);
            if (alpha < 0.004f) continue;
            ImVec2 p0(cnt.x + cosf(a) * (r0 + wob), cnt.y + sinf(a) * (r0 + wob));
            ImVec2 p1(cnt.x + cosf(a) * (r1 + wob), cnt.y + sinf(a) * (r1 + wob));
            int r = (i % 6 == 0) ? 190 : 130;
            int g = (i % 6 == 0) ? 215 : 154;
            int b = (i % 6 == 0) ? 238 : 178;
            dl->AddLine(p0, p1, IM_COL32(r, g, b, (int)(alpha * 255.0f)), ((i % 5 == 0) ? 1.8f : 1.0f) * us);
        }
        for (int q = 0; q < 4; ++q) {
            float qt = (ft - 0.06f - q * 0.035f) / 0.38f;
            if (qt < 0.0f) qt = 0.0f; if (qt > 1.0f) qt = 1.0f;
            float qe = EaseOutCubic(qt);
            float qa = fa_base * (1.0f - ft * 0.55f) * 0.12f;
            if (qa < 0.004f) continue;
            float bw = ring_r * (0.42f + q * 0.045f);
            float bh = text_fs * (0.28f + q * 0.03f);
            float sx = (q == 0 || q == 3) ? -1.0f : 1.0f;
            float sy = (q < 2) ? -1.0f : 1.0f;
            float px = cnt.x + sx * (ring_r * (1.85f - qe * 1.10f));
            float py = cnt.y + sy * (ring_r * (1.05f - qe * 0.62f));
            dl->AddRectFilled(ImVec2(px - bw, py - bh), ImVec2(px + bw, py + bh),
                IM_COL32(190, 215, 232, (int)(qa * 150.0f)), 2.0f * us);
            dl->AddRectFilled(ImVec2(px - bw * 0.22f, py - bh * 1.15f), ImVec2(px - bw * 0.12f, py + bh * 1.15f),
                IM_COL32(0, 0, 0, (int)(qa * 230.0f)), 0.8f * us);
        }
        float lock_t = (ft - 0.22f) / 0.38f;
        if (lock_t < 0.0f) lock_t = 0.0f; if (lock_t > 1.0f) lock_t = 1.0f;
            float lock_e = EaseInOutCubic(lock_t);
        float lock_a = fa_base * (ft > 0.74f ? (1.0f - EaseOutCubic((ft - 0.74f) / 0.26f)) : 1.0f) * 0.46f;
        if (lock_a > 0.004f) {
            float gap = ring_r * (0.48f + 0.40f * lock_e);
            float len = ring_r * (0.26f + 0.22f * lock_e);
            dl->AddLine(ImVec2(cnt.x - gap - len, cnt.y), ImVec2(cnt.x - gap, cnt.y), IM_COL32(185,215,235,(int)(lock_a*185.0f)), 1.2f*us);
            dl->AddLine(ImVec2(cnt.x + gap, cnt.y), ImVec2(cnt.x + gap + len, cnt.y), IM_COL32(185,215,235,(int)(lock_a*185.0f)), 1.2f*us);
            dl->AddLine(ImVec2(cnt.x, cnt.y - gap - len), ImVec2(cnt.x, cnt.y - gap), IM_COL32(185,215,235,(int)(lock_a*185.0f)), 1.2f*us);
            dl->AddLine(ImVec2(cnt.x, cnt.y + gap), ImVec2(cnt.x, cnt.y + gap + len), IM_COL32(185,215,235,(int)(lock_a*185.0f)), 1.2f*us);
            float cursor = -ring_r * 0.78f + ring_r * 1.56f * fmodf(ft * 2.15f, 1.0f);
            dl->AddRectFilled(ImVec2(cnt.x + cursor - 1.2f*us, cnt.y - ring_r * 0.52f),
                              ImVec2(cnt.x + cursor + 1.2f*us, cnt.y + ring_r * 0.52f),
                              IM_COL32(170,205,228,(int)(lock_a*85.0f)), 1.0f*us);
        }
    }
    if (elapsed >= 0.24f && elapsed < 2.05f) {
        float ht = (elapsed - 0.24f) / 1.81f;
        if (ht < 0.0f) ht = 0.0f; if (ht > 1.0f) ht = 1.0f;
        float sys_fade = ht > 0.70f ? (1.0f - EaseOutCubic((ht - 0.70f) / 0.30f)) : 1.0f;
        if (sys_fade < 0.0f) sys_fade = 0.0f;
        for (int b = 0; b < 9; ++b) {
            float by = fmodf(elapsed * (45.0f + b * 7.0f) + b * scr.y * 0.137f, scr.y);
            float bw = scr.x * (0.16f + 0.09f * (float)((b * 5) % 7));
            float bx = fmodf(sinf((float)b * 9.17f + elapsed * 3.0f) * scr.x * 0.45f + scr.x * 0.52f, scr.x);
            float ba = global_alpha * sys_fade * (0.020f + 0.018f * (float)(b % 3));
            dl->AddRectFilled(ImVec2(bx - bw * 0.5f, by), ImVec2(bx + bw * 0.5f, by + (1.0f + b % 2) * us),
                IM_COL32(220, 235, 255, (int)(ba * 255.0f)));
        }
        for (int i = 0; i < STARS_N; i += 18) {
            int j = (i + 37) % STARS_N;
            float a0 = star_alpha[i] * global_alpha;
            float a1 = star_alpha[j] * global_alpha;
            float la = (a0 < a1 ? a0 : a1) * 0.32f;
            if (la < 0.006f) continue;
            float dx = star_x[i] - star_x[j], dy = star_y[i] - star_y[j];
            float d2 = dx*dx + dy*dy;
            if (d2 > ring_r * ring_r * 0.78f) continue;
            dl->AddLine(ImVec2(star_x[i], star_y[i]), ImVec2(star_x[j], star_y[j]),
                IM_COL32(145, 170, 195, (int)(la * 255.0f)), 0.8f * us);
        }
        for (int i = 0; i < STARS_N; ++i) {
            float sa = star_alpha[i] * global_alpha;
            if (sa < 0.005f) continue;
            float s1 = (float)i * 97.13f + (float)(i * i) * 0.021f;
            int mode = i % 6;
            float px = star_x[i], py = star_y[i];
            if (star_px[i] != 0.0f || star_py[i] != 0.0f) {
                float dir = (i & 1) ? 1.0f : -1.0f;
                float trail = (8.0f + (float)(i % 7) * 2.0f) * us * sa;
                dl->AddLine(ImVec2(px, py), ImVec2(px - dir * trail, py),
                    IM_COL32(210, 225, 240, (int)(sa * 120.0f)), 1.0f * us);
                if ((i % 11) == 0) {
                    dl->AddLine(ImVec2(px - dir * trail, py), ImVec2(px - dir * trail, py + ((i & 2) ? 7.0f : -7.0f) * us),
                        IM_COL32(210, 225, 240, (int)(sa * 80.0f)), 0.8f * us);
                }
            }
            if (mode == 0) {
                float sz = (2.2f + fabsf(sinf(s1)) * 2.2f) * us;
                dl->AddRectFilled(ImVec2(px - sz, py - sz), ImVec2(px + sz, py + sz),
                    IM_COL32(235, 242, 250, (int)(sa * 210.0f)), 0.6f * us);
                dl->AddRect(ImVec2(px - sz * 1.7f, py - sz * 1.7f), ImVec2(px + sz * 1.7f, py + sz * 1.7f),
                    IM_COL32(120, 150, 180, (int)(sa * 95.0f)), 0.8f * us, 0, 0.8f * us);
            } else if (mode == 1 || mode == 2) {
                float w = (6.0f + (float)((i * 3) % 11)) * us;
                float h = (1.2f + (float)(i % 2)) * us;
                dl->AddRectFilled(ImVec2(px - w * 0.5f, py - h), ImVec2(px + w * 0.5f, py + h),
                    IM_COL32(200, 215, 230, (int)(sa * 180.0f)), 0.5f * us);
                if ((i % 17) == 0) {
                    dl->AddRectFilled(ImVec2(px + w * 0.65f, py - h), ImVec2(px + w * 0.95f, py + h),
                        IM_COL32(196, 216, 232, (int)(sa * 145.0f)), 0.5f * us);
                }
            } else if (mode == 3) {
                float r = (3.0f + (float)(i % 4)) * us;
                dl->AddLine(ImVec2(px - r, py), ImVec2(px + r, py), IM_COL32(225,235,245,(int)(sa*165.0f)), 0.9f*us);
                dl->AddLine(ImVec2(px, py - r), ImVec2(px, py + r), IM_COL32(225,235,245,(int)(sa*165.0f)), 0.9f*us);
            } else {
                float w = (5.0f + (float)(i % 5) * 1.4f) * us;
                float h = (3.0f + (float)(i % 3) * 1.2f) * us;
                dl->AddRect(ImVec2(px - w, py - h), ImVec2(px + w, py + h),
                    IM_COL32(180, 200, 220, (int)(sa * 135.0f)), 0.8f * us, 0, 0.8f * us);
            }
        }
    }
    if (elapsed >= 1.58f && elapsed < T_TEXT_BEG + 0.22f) {
        float ht = (elapsed - 1.58f) / (T_TEXT_BEG + 0.22f - 1.58f);
        if (ht < 0.0f) ht = 0.0f; if (ht > 1.0f) ht = 1.0f;
        float ha = (ht < 0.18f ? EaseOutCubic(ht / 0.18f) : 1.0f) *
                   (ht > 0.76f ? (1.0f - EaseOutCubic((ht - 0.76f) / 0.24f)) : 1.0f) * global_alpha;
        const int HAND_N = 38;
        for (int i = 0; i < HAND_N; ++i) {
            float seed = (float)i * 1.913f + 0.21f;
            float d = fmodf(fabsf(sinf(seed * 2.1f)) * 0.20f, 0.20f);
            float t = (ht - d) / (1.0f - d);
            if (t < 0.0f) continue; if (t > 1.0f) t = 1.0f;
            float e = EaseInOutCubic(t);
            float side = (i & 1) ? 1.0f : -1.0f;
            float row = ((float)(i % 11) - 5.0f) / 5.0f;
            ImVec2 p0(cnt.x + side * ring_r * (1.62f + 0.28f * fabsf(sinf(seed))), cnt.y + row * ring_r * 0.72f);
            ImVec2 p1(cnt.x + side * ring_r * 0.96f, cnt.y - ring_r * (0.88f + 0.22f * sinf(seed)));
            ImVec2 p2(cnt.x - side * ring_r * 0.38f, cnt.y + ring_r * (0.54f + 0.12f * cosf(seed)));
            float ta = seed + e * 2.0f * IM_PI * side;
            ImVec2 p3(cnt.x + cosf(ta) * ring_r * 0.72f, cnt.y + sinf(ta) * ring_r * 0.72f);
            float u = 1.0f - e;
            ImVec2 p(
                u*u*u*p0.x + 3*u*u*e*p1.x + 3*u*e*e*p2.x + e*e*e*p3.x,
                u*u*u*p0.y + 3*u*u*e*p1.y + 3*u*e*e*p2.y + e*e*e*p3.y
            );
            float e2 = e + 0.018f; if (e2 > 1.0f) e2 = 1.0f;
            float u2 = 1.0f - e2;
            ImVec2 pn(
                u2*u2*u2*p0.x + 3*u2*u2*e2*p1.x + 3*u2*e2*e2*p2.x + e2*e2*e2*p3.x,
                u2*u2*u2*p0.y + 3*u2*u2*e2*p1.y + 3*u2*e2*e2*p2.y + e2*e2*e2*p3.y
            );
            float ang = atan2f(pn.y - p.y, pn.x - p.x);
            float vx = cosf(ang), vy = sinf(ang);
            float a = ha * (0.28f + 0.18f * (float)(i % 3));
            a *= 0.82f + 0.18f * fabsf(sinf(elapsed * 3.2f + seed));
            if (a < 0.004f) continue;
            float trail = (12.0f + 20.0f * e) * us;
            dl->AddLine(ImVec2(p.x - vx * trail, p.y - vy * trail), p,
                IM_COL32(100, 175, 245, (int)(a * 150.0f)), 1.2f * us);
            if ((i % 4) == 0) {
                float sz = (2.2f + (i % 3) * 0.7f) * us;
                dl->AddRectFilled(ImVec2(p.x - sz, p.y - sz), ImVec2(p.x + sz, p.y + sz),
                    IM_COL32(235, 246, 255, (int)(a * 230.0f)), 0.5f * us);
            } else {
                float len = (7.0f + (i % 5) * 1.2f) * us;
                dl->AddLine(ImVec2(p.x - vx * len * 0.4f, p.y - vy * len * 0.4f),
                            ImVec2(p.x + vx * len * 0.6f, p.y + vy * len * 0.6f),
                            IM_COL32(225, 238, 250, (int)(a * 220.0f)), 1.5f * us);
            }
        }
    }
    if(bounce_glow>0.005f && elapsed<T_PULSE_BEG){
        float cu_r=current_ring_r;
        for(int i=0;i<3;++i){
            float gr=cu_r*(1.18f+i*0.28f);
            float ga=bounce_glow*global_alpha*(0.22f-i*0.07f);
            if(ga<0.005f)continue;
            dl->AddCircle(cnt,gr,IM_COL32(200,215,245,(int)(ga*255.0f)),96,2.6f*us);
        }
    }
    if(breath_glow>0.005f && elapsed<T_PULSE_BEG && ring_alpha>0.01f){
        float glow_r=current_ring_r*1.35f;
        for(int i=0;i<3;++i){
            float gr=glow_r*(1.0f+i*0.22f);
            float ga=breath_glow*global_alpha*(0.16f-i*0.05f);
            if(ga<0.005f)continue;
            dl->AddCircle(cnt,gr,IM_COL32(180,200,235,(int)(ga*255.0f)),80,2.0f*us);
        }
    }
    if (elapsed >= T_PULSE_BEG && elapsed < T_BURST_END + 0.3f) {
        for (int i = 0; i < CV_N; ++i) {
            float pa = cv_alpha[i] * global_alpha;
            if (pa < 0.005f) continue;
            float ps = 2.0f * us;
            float angle = atan2f(cv_y[i] - cnt.y, cv_x[i] - cnt.x);
            float trail = pa * 14.0f * us;
            dl->AddLine(ImVec2(cv_x[i], cv_y[i]),
                ImVec2(cv_x[i] - cosf(angle)*trail, cv_y[i] - sinf(angle)*trail),
                IM_COL32(200, 210, 245, (int)(pa * 130.0f)), 1.0f * us);
            dl->AddCircleFilled(ImVec2(cv_x[i], cv_y[i]), ps,
                IM_COL32(240, 245, 255, (int)(pa * 230.0f)), 12);
            dl->AddCircleFilled(ImVec2(cv_x[i], cv_y[i]), ps * 0.35f,
                IM_COL32(205, 226, 240, (int)(pa * 140.0f)), 8);
        }
    }
    if (ball_glow > 0.005f && elapsed < T_BURST_END) {
        float ga = ball_glow * global_alpha;
        for (int i = 0; i < 6; ++i) {
            float gr = ball_r * (0.6f + i * 0.55f + ga * 0.8f);
            float a = ga * (0.40f - i * 0.06f);
            if (a < 0.005f) continue;
            dl->AddCircle(cnt, gr, IM_COL32(220, 230, 255, (int)(a * 255.0f)), 96, 4.0f * us);
        }
        dl->AddCircleFilled(cnt, ball_r * 0.48f,
            IM_COL32(202, 224, 240, (int)(ga * 155.0f)), 64);
    }
    float ring_float_y = 0.0f;
    {
        float float_amp = 2.5f * us;
        float float_speed = 2.2f;
        float float_factor = 1.0f;
        if (elapsed >= T_BREATH_BEG) {
            if (elapsed < T_BREATH_END) {
                float_factor = breath_glow * 0.8f + 0.2f;
            } else {
                float blend = (elapsed - T_BREATH_END) / 0.50f;
                if (blend > 1.0f) blend = 1.0f;
                float_factor = 0.2f + (0.45f - 0.2f) * blend;
            }
            if (elapsed >= T_PULSE_BEG) float_speed = 1.6f;
        }
        float_amp *= float_factor;
        ring_float_y = sinf(elapsed * float_speed * IM_PI) * float_amp;
    }
    ImVec2 float_cnt(cnt.x, cnt.y + ring_float_y);
    if (elapsed >= T_TEXT_BEG - 0.12f && elapsed < T_PULSE_BEG + 0.18f) {
        float ct = (elapsed - (T_TEXT_BEG - 0.12f)) / (T_PULSE_BEG + 0.18f - (T_TEXT_BEG - 0.12f));
        if (ct < 0.0f) ct = 0.0f; if (ct > 1.0f) ct = 1.0f;
        float cin = (ct < 0.18f) ? EaseOutCubic(ct / 0.18f) : 1.0f;
        float cout = (ct > 0.76f) ? (1.0f - EaseOutCubic((ct - 0.76f) / 0.24f)) : 1.0f;
        float ca = cin * cout * global_alpha;
        float power = 0.55f + 0.45f * fabsf(sinf(elapsed * 2.8f));
        float breath_power = 0.55f + 0.45f * breath_glow;
        float flick = 0.88f + 0.12f * fabsf(sinf(elapsed * 2.4f));
        ca *= flick;
        const float grid = 42.0f * us;
        int cols = (int)(scr.x / grid) + 3;
        int rows = (int)(scr.y / grid) + 3;
        float ox = fmodf(-elapsed * 10.0f * us, grid);
        float oy = fmodf(elapsed * 6.0f * us, grid);
        float focus_r = ring_r * 3.85f;
        for (int x = 0; x < cols; ++x) {
            float px = ox + x * grid;
            float d = fabsf(px - cnt.x);
            float near = 1.0f - d / (focus_r * 1.55f);
            if (near < 0.0f) near = 0.0f; if (near > 1.0f) near = 1.0f;
            near = near * near * (3.0f - 2.0f * near);
            float scan = 0.5f + 0.5f * sinf(elapsed * 3.2f - d * 0.010f);
            float a = ca * (0.010f + 0.048f * near * breath_power + 0.012f * scan * near);
            if (a > 0.003f) dl->AddLine(ImVec2(px, 0), ImVec2(px, scr.y), IM_COL32(75, 150, 230, (int)(a * 255.0f)), 0.65f * us);
        }
        for (int y = 0; y < rows; ++y) {
            float py = oy + y * grid;
            float d = fabsf(py - cnt.y);
            float near = 1.0f - d / (focus_r * 1.22f);
            if (near < 0.0f) near = 0.0f; if (near > 1.0f) near = 1.0f;
            near = near * near * (3.0f - 2.0f * near);
            float scan = 0.5f + 0.5f * cosf(elapsed * 3.5f - d * 0.012f);
            float a = ca * (0.008f + 0.040f * near * breath_power + 0.010f * scan * near);
            if (a > 0.003f) dl->AddLine(ImVec2(0, py), ImVec2(scr.x, py), IM_COL32(65, 130, 210, (int)(a * 255.0f)), 0.65f * us);
        }
        for (int p = 0; p < 4; ++p) {
            float wave = fmodf(elapsed * 0.62f + p * 0.25f, 1.0f);
            float wr = Lerp(ring_r * 4.10f, ring_r * 1.18f, EaseInOutCubic(wave));
            float wa = ca * (1.0f - wave) * (0.055f + 0.030f * breath_power);
            if (wa > 0.004f)
                dl->AddCircle(cnt, wr, IM_COL32(95, 185, 255, (int)(wa * 255.0f)), 128, (1.0f + (1.0f - wave) * 1.2f) * us);
        }
        const int PATHS = 22;
        for (int i = 0; i < PATHS; ++i) {
            float seed = i * 1.917f + 0.37f;
            float side = (i & 1) ? 1.0f : -1.0f;
            float y = scr.y * (0.14f + 0.72f * fmodf(fabsf(sinf(seed)), 1.0f));
            float x0 = side > 0 ? scr.x + 18.0f * us : -18.0f * us;
            float x1 = cnt.x + side * ring_r * (2.25f + 0.42f * fabsf(cosf(seed)));
            float x2 = cnt.x + side * ring_r * (1.30f + 0.20f * sinf(seed));
            float y1 = y + sinf(seed * 2.0f) * ring_r * 0.24f;
            float y2 = cnt.y + sinf(seed + elapsed * 0.3f) * ring_r * 0.78f;
            float mid_dx = x2 - cnt.x, mid_dy = y2 - cnt.y;
            float dist = sqrtf(mid_dx * mid_dx + mid_dy * mid_dy);
            float near = 1.0f - dist / (ring_r * 3.25f);
            if (near < 0.0f) near = 0.0f; if (near > 1.0f) near = 1.0f;
            near = near * near * (3.0f - 2.0f * near);
            float attract = 0.5f + 0.5f * sinf(elapsed * 5.6f - dist * 0.018f + seed);
            float pa = ca * (0.030f + 0.050f * fabsf(sinf(elapsed * 4.0f + seed)) + 0.105f * near * breath_power + 0.045f * attract * near);
            pa *= 0.88f + 0.12f * fabsf(sinf(elapsed * 2.6f + seed));
            if (pa < 0.004f) continue;
            ImU32 col_far = IM_COL32(70, 130, 190, (int)(pa * 135.0f));
            ImU32 col_hot = IM_COL32(120, 210, 255, (int)(pa * 255.0f));
            dl->AddLine(ImVec2(x0, y), ImVec2(x1, y), col_far, 0.9f * us);
            dl->AddLine(ImVec2(x1, y), ImVec2(x1, y1), col_far, 0.9f * us);
            dl->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col_hot, (1.0f + near * 0.9f) * us);
            ImVec2 sink(cnt.x + side * ring_r * 0.98f, cnt.y + sinf(seed * 1.4f) * ring_r * 0.36f);
            dl->AddLine(ImVec2(x2, y2), sink, IM_COL32(155, 225, 255, (int)(pa * near * 215.0f)), 1.15f * us);
            dl->AddRectFilled(ImVec2(x1 - 2.2f*us, y1 - 2.2f*us), ImVec2(x1 + 2.2f*us, y1 + 2.2f*us),
                IM_COL32(210, 235, 255, (int)(pa * 150.0f)), 0.8f * us);
            dl->AddCircleFilled(ImVec2(x2, y2), (2.1f + near * 2.0f) * us,
                IM_COL32(230, 248, 255, (int)(pa * near * 230.0f)), 16);
            float run = fmodf(elapsed * (0.62f + 0.055f * i) + i * 0.071f, 1.0f);
            float bx = Lerp(x0, x1, run);
            float by = y;
            if (run > 0.68f) {
                float rr = (run - 0.68f) / 0.32f;
                bx = Lerp(x1, x2, rr);
                by = Lerp(y1, y2, rr);
            }
            dl->AddRectFilled(ImVec2(bx - 5.0f*us, by - 1.2f*us), ImVec2(bx + 5.0f*us, by + 1.2f*us),
                IM_COL32(235, 250, 255, (int)(pa * power * 235.0f)), 0.5f * us);
        }
    }
    if (elapsed >= T_BREATH_BEG && elapsed < T_PULSE_BEG && ring_alpha > 0.01f) {
        float proto_in = (elapsed - T_BREATH_BEG) / 0.45f;
        if (proto_in < 0.0f) proto_in = 0.0f; if (proto_in > 1.0f) proto_in = 1.0f;
        float proto_out = (T_PULSE_BEG - elapsed) / 0.75f;
        if (proto_out < 0.0f) proto_out = 0.0f; if (proto_out > 1.0f) proto_out = 1.0f;
        proto_out = EaseOutCubic(proto_out);
        float proto_a = EaseOutCubic(proto_in) * proto_out * global_alpha;
        float base_r = current_ring_r;
        const int CLOUD_N = 7;
        const float cloud_ang[CLOUD_N] = {-2.55f, -2.05f, -1.46f, -0.74f, -0.04f, 0.58f, 1.15f};
        const float cloud_rad[CLOUD_N] = {0.70f, 0.88f, 1.04f, 0.96f, 0.80f, 0.66f, 0.54f};
        for (int i = 0; i < CLOUD_N; ++i) {
            float pulse = 0.5f + 0.5f * sinf(elapsed * 2.7f + (float)i * 0.83f);
            float a = cloud_ang[i] + sinf(elapsed * 0.65f + i) * 0.035f;
            float rr = base_r * (0.58f + 0.08f * cloud_rad[i]);
            ImVec2 cp(float_cnt.x + cosf(a) * base_r * 0.33f, float_cnt.y + sinf(a) * base_r * 0.20f);
            float ca = proto_a * (0.055f + pulse * 0.035f);
            if (ca < 0.004f) continue;
            dl->AddCircle(cp, rr, IM_COL32(205, 220, 238, (int)(ca * 255.0f)), 64, (1.1f + pulse * 0.5f) * us);
        }
        const int TICKS = 48;
        float spin = elapsed * 0.72f;
        for (int i = 0; i < TICKS; ++i) {
            if ((i % 4) == 1) continue;
            float a = spin + (float)i / TICKS * 2.0f * IM_PI;
            float r0 = base_r * (1.16f + 0.018f * sinf(i * 1.7f));
            float len = base_r * (0.025f + 0.018f * (float)(i % 3));
            float ta = proto_a * (0.11f + 0.07f * fabsf(sinf(elapsed * 3.0f + i)));
            if (ta < 0.004f) continue;
            ImVec2 p0(float_cnt.x + cosf(a) * r0 * ring_aspect, float_cnt.y + sinf(a) * r0);
            ImVec2 p1(float_cnt.x + cosf(a) * (r0 + len) * ring_aspect, float_cnt.y + sinf(a) * (r0 + len));
            dl->AddLine(p0, p1, IM_COL32(180, 205, 232, (int)(ta * 255.0f)), 1.0f * us);
        }
        const int PACKETS = 6;
        for (int i = 0; i < PACKETS; ++i) {
            float a = -elapsed * 1.65f + (float)i / PACKETS * 2.0f * IM_PI;
            float rr = base_r * (1.04f + 0.045f * sinf(elapsed * 2.0f + i));
            float px = float_cnt.x + cosf(a) * rr * ring_aspect;
            float py = float_cnt.y + sinf(a) * rr;
            float pa = proto_a * (0.35f + 0.18f * sinf(elapsed * 5.0f + i));
            if (pa < 0.004f) continue;
            dl->AddCircleFilled(ImVec2(px, py), 2.1f * us, IM_COL32(235, 245, 255, (int)(pa * 255.0f)), 12);
            dl->AddCircle(ImVec2(px, py), 5.0f * us, IM_COL32(120, 165, 210, (int)(pa * 90.0f)), 16, 0.8f * us);
        }
    }
    if (elapsed >= T_RING_BEG && elapsed < T_RING_END && ring_alpha > 0.005f) {
        float lt = (elapsed - T_RING_BEG) / (T_RING_END - T_RING_BEG);
        if (lt > 1.0f) lt = 1.0f;
        float pop_a = ring_alpha * global_alpha;
        float pop_r = ring_r * ring_scale;
        if (ring_pop_flash > 0.005f) {
            float fa = ring_pop_flash * pop_a;
            dl->AddCircleFilled(float_cnt, pop_r * 0.30f,
                IM_COL32(205, 226, 240, (int)(fa * 72.0f)), 64);
            dl->AddCircle(float_cnt, pop_r * (0.70f + ring_pop_flash * 0.14f),
                IM_COL32(155, 205, 235, (int)(fa * 118.0f)), 96, 3.8f * us);
        }
        if (ring_pop_wave > 0.001f) {
            for (int w = 0; w < 3; ++w) {
                float wt = ring_pop_wave - w * 0.13f;
                if (wt < 0.0f) continue;
                if (wt > 1.0f) wt = 1.0f;
                float we = EaseOutCubic(wt);
                float wr = ring_r * (0.52f + we * (0.95f + w * 0.25f));
                float wa = (1.0f - wt) * pop_a * (0.42f - w * 0.09f);
                if (wa < 0.005f) continue;
                dl->AddCircle(float_cnt, wr,
                    IM_COL32(210, 225, 255, (int)(wa * 255.0f)), 128, Lerp(5.2f, 1.2f, we) * us);
                dl->AddCircle(float_cnt, wr,
                    IM_COL32(120, 170, 255, (int)(wa * 75.0f)), 128, Lerp(14.0f, 3.0f, we) * us);
            }
        }
        if (ring_chroma > 0.01f) {
            float ca = ring_chroma * pop_a * 0.42f;
            float off = (10.0f + ring_chroma * 18.0f) * us;
            dl->AddCircle(ImVec2(float_cnt.x - off, float_cnt.y), pop_r * 1.015f,
                IM_COL32(255, 70, 90, (int)(ca * 180.0f)), 96, 2.2f * us);
            dl->AddCircle(ImVec2(float_cnt.x + off, float_cnt.y), pop_r * 0.985f,
                IM_COL32(70, 180, 255, (int)(ca * 180.0f)), 96, 2.2f * us);
        }
        if (ring_slash > 0.005f) {
            const int SL_N = 9;
            float spin = -lt * 5.8f * IM_PI;
            for (int i = 0; i < SL_N; ++i) {
                float a = spin + (float)i / SL_N * 2.0f * IM_PI;
                float len = ring_r * (0.17f + ring_slash * 0.18f);
                float rr = pop_r * (0.92f + 0.018f * (float)(i % 3));
                float px = float_cnt.x + cosf(a) * rr * ring_aspect;
                float py = float_cnt.y + sinf(a) * rr;
                float tx = -sinf(a), ty = cosf(a);
                float sa = ring_slash * pop_a * (0.58f - i * 0.035f);
                if (sa < 0.005f) continue;
                dl->AddLine(ImVec2(px - tx * len, py - ty * len),
                            ImVec2(px + tx * len, py + ty * len),
                            IM_COL32(245, 250, 255, (int)(sa * 255.0f)), 2.0f * us);
                dl->AddLine(ImVec2(px - tx * len * 1.25f, py - ty * len * 1.25f),
                            ImVec2(px + tx * len * 1.25f, py + ty * len * 1.25f),
                            IM_COL32(150, 190, 255, (int)(sa * 95.0f)), 5.5f * us);
            }
        }
    }
    if (ring_alpha > 0.005f) {
        float ra = ring_alpha * global_alpha;
        float reverse_breath = 1.0f;
        float reverse_aspect = 1.0f;
        if (elapsed >= T_BURST_END && elapsed < T_BURST_END + 0.80f) {
            float rev_lt = (elapsed - T_BURST_END) / 0.80f;
            float min_ra = 0.50f * (1.0f - rev_lt * 0.55f);
            if (ra < min_ra) ra = min_ra;
            auto elastic = [](float t, float from, float to, float bounce) -> float {
                if (t <= 0.0f) return from;
                if (t >= 1.0f) return to;
                float diff = to - from;
                float s = 1.0f - expf(-7.0f * t);
                float w = expf(-5.0f * t) * sinf(t * 9.0f * IM_PI) * bounce;
                float v = from + diff * (s + w);
                if (diff < 0 && v > from + diff*0.3f) v = from + diff*0.3f;
                if (diff > 0 && v < from + diff*0.7f) v = from + diff*0.7f;
                return v;
            };
            float step_scale, deform = 0.0f, bounce;
            if (rev_lt < 0.12f) {
                float t = rev_lt / 0.12f;
                bounce = 0.18f;
                step_scale = elastic(t, 1.00f, 0.72f, bounce);
                deform = expf(-4.0f * t) * fabsf(sinf(t * 7.0f * IM_PI)) * bounce * 2.5f;
            } else if (rev_lt < 0.18f) {
                step_scale = 0.72f;
            } else if (rev_lt < 0.30f) {
                float t = (rev_lt - 0.18f) / 0.12f;
                bounce = 0.24f;
                step_scale = elastic(t, 0.72f, 0.50f, bounce);
                deform = expf(-4.0f * t) * fabsf(sinf(t * 7.0f * IM_PI)) * bounce * 3.0f;
            } else if (rev_lt < 0.36f) {
                step_scale = 0.50f;
            } else if (rev_lt < 0.48f) {
                float t = (rev_lt - 0.36f) / 0.12f;
                bounce = 0.32f;
                step_scale = elastic(t, 0.50f, 0.30f, bounce);
                deform = expf(-4.0f * t) * fabsf(sinf(t * 7.0f * IM_PI)) * bounce * 3.8f;
            } else if (rev_lt < 0.54f) {
                step_scale = 0.30f;
            } else {
                float t = (rev_lt - 0.54f) / 0.46f;
                bounce = 0.40f;
                step_scale = elastic(t, 0.30f, 1.00f, bounce);
                deform = expf(-4.0f * t) * fabsf(sinf(t * 9.0f * IM_PI)) * bounce * 4.5f;
            }
            reverse_breath = step_scale;
            reverse_aspect = 1.0f + deform;
        }
        float draw_r = (elapsed >= T_PULSE_BEG) ? ball_r : current_ring_r;
        draw_r *= reverse_breath;
        if (draw_r > 3.0f) {
            int cb = (int)((elapsed >= T_PULSE_BEG ? ball_glow : breath_glow) * 40);
            ImU32 center_col = IM_COL32(cb, cb, cb, (int)(ra * 255.0f));
            float draw_aspect = ring_aspect * reverse_aspect;
            float total_scale = ring_scale * breath_scale * ring_shrink * reverse_breath;
            float scale_delta = total_scale - _prev_total_scale;
            _ring_rotation += scale_delta * 8.0f;  
            _prev_total_scale = total_scale;
            DrawRingLogo(dl, float_cnt, draw_r, us, ra, center_col, draw_aspect, _ring_rotation);
            if (elapsed >= T_BREATH_BEG && elapsed < T_PULSE_BEG && draw_r > 10.0f) {
                float reflect_y = float_cnt.y + draw_r * 1.25f;
                float ra_r = breath_glow * ra * 0.15f;
                for (int i = 0; i < 4; ++i) {
                    float ry = reflect_y + i * 3.0f * us;
                    float rr_a = ra_r * (0.16f - i * 0.04f);
                    if (rr_a < 0.003f) continue;
                    float rx = draw_r * (0.55f + i * 0.12f);
                    const int RSEG = 48;
                    ImVec2 rpt[RSEG + 1];
                    for (int j = 0; j <= RSEG; ++j) {
                        float ga = (float)j / RSEG * 2.0f * IM_PI;
                        rpt[j] = ImVec2(float_cnt.x + cosf(ga) * rx,
                                        ry + (sinf(ga) < 0.0f ? fabsf(sinf(ga)) * rx * 0.32f : 0.0f));
                    }
                    dl->AddPolyline(rpt, RSEG + 1, IM_COL32(215, 222, 238, (int)(rr_a * 255.0f)),
                                    ImDrawFlags_None, 1.4f * us);
                }
            }
        }
    }
    {
        const float T_FOLD_BEG = T_SHRINK_BEG + 0.04f;
        const float T_FOLD_LOCK = T_RING2BALL_END - 0.16f;
        const float T_FOLD_END = T_BURST_BEG + 0.10f;
        if (elapsed >= T_FOLD_BEG && elapsed < T_FOLD_END) {
            float ft_all = (elapsed - T_FOLD_BEG) / (T_FOLD_END - T_FOLD_BEG);
            if (ft_all < 0.0f) ft_all = 0.0f; if (ft_all > 1.0f) ft_all = 1.0f;
            float fin = ft_all < 0.16f ? EaseOutCubic(ft_all / 0.16f) : 1.0f;
            float fout = ft_all > 0.84f ? (1.0f - EaseOutCubic((ft_all - 0.84f) / 0.16f)) : 1.0f;
            float fold_a = fin * fout * global_alpha;
            const int FOLD_N = 72;
            float absorb_t = (elapsed - T_FOLD_BEG) / (T_FOLD_LOCK - T_FOLD_BEG);
            if (absorb_t < 0.0f) absorb_t = 0.0f; if (absorb_t > 1.0f) absorb_t = 1.0f;
            for (int i = 0; i < FOLD_N; ++i) {
                float seed = (float)i * 19.37f + (float)(i * i) * 0.071f;
                float delay = (float)(i % 12) * 0.018f + 0.035f * fabsf(sinf(seed));
                float pt = (absorb_t - delay) / (1.0f - delay * 0.72f);
                if (pt < 0.0f || pt > 1.0f) continue;
                float e = EaseInOutCubic(pt);
                float side = (float)(i % 4);
                float edge_pos = fmodf(fabsf(sinf(seed * 0.43f)) * 1.37f + (float)(i % 17) * 0.071f, 1.0f);
                ImVec2 p0;
                if (side < 0.5f)      p0 = ImVec2(-32.0f * us, edge_pos * scr.y);
                else if (side < 1.5f) p0 = ImVec2(scr.x + 32.0f * us, edge_pos * scr.y);
                else if (side < 2.5f) p0 = ImVec2(edge_pos * scr.x, -32.0f * us);
                else                  p0 = ImVec2(edge_pos * scr.x, scr.y + 32.0f * us);
                float ta = (float)((i * 137) % FOLD_N) / (float)FOLD_N * 2.0f * IM_PI + elapsed * 0.55f;
                float lane = 0.92f + (float)((i % 5) - 2) * 0.026f;
                ImVec2 p3(float_cnt.x + cosf(ta) * current_ring_r * lane * ring_aspect,
                          float_cnt.y + sinf(ta) * current_ring_r * lane);
                ImVec2 c1(p0.x * 0.72f + float_cnt.x * 0.28f + sinf(seed) * 90.0f * us,
                          p0.y * 0.72f + float_cnt.y * 0.28f + cosf(seed) * 64.0f * us);
                ImVec2 c2(float_cnt.x + cosf(ta + 1.45f) * current_ring_r * (1.85f - e * 0.55f),
                          float_cnt.y + sinf(ta + 1.45f) * current_ring_r * (1.25f - e * 0.25f));
                float u = e, iu = 1.0f - u;
                ImVec2 p(iu*iu*iu*p0.x + 3.0f*iu*iu*u*c1.x + 3.0f*iu*u*u*c2.x + u*u*u*p3.x,
                         iu*iu*iu*p0.y + 3.0f*iu*iu*u*c1.y + 3.0f*iu*u*u*c2.y + u*u*u*p3.y);
                float pa = fold_a * (1.0f - pt * 0.20f) * (0.55f + 0.45f * fabsf(sinf(elapsed * 18.0f + seed)));
                if (pa < 0.004f) continue;
                float u0 = u - 0.055f; if (u0 < 0.0f) u0 = 0.0f;
                float iu0 = 1.0f - u0;
                ImVec2 prev(iu0*iu0*iu0*p0.x + 3.0f*iu0*iu0*u0*c1.x + 3.0f*iu0*u0*u0*c2.x + u0*u0*u0*p3.x,
                            iu0*iu0*iu0*p0.y + 3.0f*iu0*iu0*u0*c1.y + 3.0f*iu0*u0*u0*c2.y + u0*u0*u0*p3.y);
                dl->AddLine(prev, p, IM_COL32(105, 180, 245, (int)(pa * 150.0f)), 1.2f * us);
                if ((i % 3) == 0) {
                    float w = (5.0f + (i % 5) * 1.3f) * us;
                    float h = (2.0f + (i % 3) * 0.8f) * us;
                    dl->AddRectFilled(ImVec2(p.x - w, p.y - h), ImVec2(p.x + w, p.y + h),
                                      IM_COL32(225, 240, 255, (int)(pa * 185.0f)), 0.8f * us);
                } else {
                    dl->AddCircleFilled(p, (1.8f + (i % 4) * 0.45f) * us,
                                        IM_COL32(235, 246, 255, (int)(pa * 225.0f)), 10);
                }
            }
            float lock_t = (elapsed - (T_FOLD_LOCK - 0.24f)) / 0.34f;
            if (lock_t > 0.0f && lock_t < 1.0f) {
                float le = EaseInOutCubic(lock_t);
                float la = fold_a * (1.0f - lock_t * 0.25f);
                float rr = current_ring_r * (1.20f - 0.22f * le);
                for (int q = 0; q < 4; ++q) {
                    float a = q * IM_PI * 0.5f + elapsed * 0.16f;
                    float tx = cosf(a), ty = sinf(a);
                    float nx = -ty, ny = tx;
                    ImVec2 m(float_cnt.x + tx * rr * ring_aspect, float_cnt.y + ty * rr);
                    float len = current_ring_r * (0.26f + 0.12f * le);
                    dl->AddLine(ImVec2(m.x - nx * len, m.y - ny * len), ImVec2(m.x + nx * len, m.y + ny * len),
                                IM_COL32(245, 250, 255, (int)(la * 230.0f)), 2.0f * us);
                    dl->AddLine(ImVec2(m.x + tx * 9.0f * us, m.y + ty * 9.0f * us),
                                ImVec2(m.x + tx * (34.0f + 18.0f * le) * us, m.y + ty * (34.0f + 18.0f * le) * us),
                                IM_COL32(95, 175, 245, (int)(la * 165.0f)), 1.2f * us);
                }
            }
            float hit_t = (elapsed - T_FOLD_LOCK) / (T_FOLD_END - T_FOLD_LOCK);
            if (hit_t > 0.0f) {
                if (hit_t > 1.0f) hit_t = 1.0f;
                float ha = (1.0f - hit_t) * global_alpha;
                if (ha > 0.005f) {
                    float flash = (hit_t < 0.10f) ? (1.0f - hit_t / 0.10f) : 0.0f;
                    if (flash > 0.005f) {
                        dl->AddCircleFilled(float_cnt, ball_r * (1.3f + 2.2f * hit_t),
                                            IM_COL32(196, 220, 238, (int)(flash * 95.0f)), 80);
                    }
                    for (int layer = 0; layer < 5; ++layer) {
                        float ld = layer * 0.055f;
                        float tt = (hit_t - ld) / (1.0f - ld);
                        if (tt < 0.0f || tt > 1.0f) continue;
                        float ee = EaseOutCubic(tt);
                        float rr = ball_r * (1.0f + layer * 0.38f) + ee * diag * (0.30f + layer * 0.055f);
                        float aa = ha * (0.34f - layer * 0.045f) * (1.0f - tt * 0.52f);
                        if (aa < 0.004f) continue;
                        dl->AddCircle(float_cnt, rr, IM_COL32(170, 210, 245, (int)(aa * 255.0f)), 160, (3.8f - layer * 0.38f) * us);
                        int SEG = 28 + layer * 4;
                        for (int s = 0; s < SEG; ++s) {
                            if (((s + layer) % 4) == 1) continue;
                            float a = (float)s / SEG * 2.0f * IM_PI + elapsed * (0.8f + layer * 0.12f);
                            float span = 0.022f + 0.006f * (float)((s + layer) % 3);
                            float r0 = rr * (0.94f + 0.025f * sinf(s * 1.7f));
                            float r1 = r0 + (12.0f + layer * 4.0f) * us * (1.0f - tt * 0.25f);
                            ImVec2 p1(float_cnt.x + cosf(a - span) * r0, float_cnt.y + sinf(a - span) * r0);
                            ImVec2 p2(float_cnt.x + cosf(a + span) * r0, float_cnt.y + sinf(a + span) * r0);
                            ImVec2 p3(float_cnt.x + cosf(a + span) * r1, float_cnt.y + sinf(a + span) * r1);
                            ImVec2 p4(float_cnt.x + cosf(a - span) * r1, float_cnt.y + sinf(a - span) * r1);
                            float sa = aa * (0.56f + 0.18f * fabsf(sinf(elapsed * 3.6f + s)));
                            dl->AddQuadFilled(p1, p2, p3, p4, IM_COL32(210, 235, 255, (int)(sa * 150.0f)));
                        }
                    }
                }
            }
        }
    }
    if (elapsed >= T_TEXT_BEG + 0.08f && elapsed < T_PULSE_BEG + 0.06f && ring_alpha > 0.01f) {
        float bt = (elapsed - (T_TEXT_BEG + 0.08f)) / (T_PULSE_BEG + 0.06f - (T_TEXT_BEG + 0.08f));
        if (bt < 0.0f) bt = 0.0f; if (bt > 1.0f) bt = 1.0f;
        float bin = (bt < 0.24f) ? EaseInOutCubic(bt / 0.24f) : 1.0f;
        float bout = (bt > 0.78f) ? (1.0f - EaseInOutCubic((bt - 0.78f) / 0.22f)) : 1.0f;
        float ba = bin * bout * ring_alpha * global_alpha;
        float base = current_ring_r;
        const int RINGS = 3;
        for (int layer = 0; layer < RINGS; ++layer) {
            int SEG = 28 + layer * 8;
            float rr = base * (1.18f + layer * 0.115f + 0.007f * sinf(elapsed * 1.45f + layer));
            float rot = elapsed * (0.16f + layer * 0.045f) * ((layer & 1) ? -1.0f : 1.0f);
            float la = ba * (0.16f - layer * 0.026f);
            if (la < 0.004f) continue;
            for (int i = 0; i < SEG; ++i) {
                if (((i + layer * 3) % 8) == 2) continue;
                float flash = 0.78f + 0.22f * fabsf(sinf(elapsed * (2.2f + layer * 0.35f) + i * 0.39f));
                float a0 = rot + (float)i / SEG * 2.0f * IM_PI;
                float span = (2.0f * IM_PI / SEG) * (0.30f + 0.12f * ((i + layer) % 3));
                float am = a0 + span * 0.5f;
                float tang = span * rr;
                float w = tang * (0.50f + 0.12f * ((i + layer) % 2));
                float h = (2.0f + layer * 0.65f + (i % 3) * 0.28f) * us;
                float px = float_cnt.x + cosf(am) * rr * ring_aspect;
                float py = float_cnt.y + sinf(am) * rr;
                float tx = -sinf(am), ty = cosf(am);
                float nx = cosf(am) * ring_aspect, ny = sinf(am);
                ImVec2 p1(px - tx*w*0.5f - nx*h, py - ty*w*0.5f - ny*h);
                ImVec2 p2(px + tx*w*0.5f - nx*h, py + ty*w*0.5f - ny*h);
                ImVec2 p3(px + tx*w*0.5f + nx*h, py + ty*w*0.5f + ny*h);
                ImVec2 p4(px - tx*w*0.5f + nx*h, py - ty*w*0.5f + ny*h);
                ImU32 c = IM_COL32(150, 190, 225, (int)(la * flash * 255.0f));
                dl->AddQuadFilled(p1, p2, p3, p4, c);
                if ((i % 6) == 0) {
                    dl->AddLine(ImVec2(px + nx*h*1.6f, py + ny*h*1.6f),
                                ImVec2(px + nx*h*3.4f, py + ny*h*3.4f),
                                IM_COL32(95, 155, 215, (int)(la * flash * 110.0f)), 0.8f * us);
                }
            }
        }
        for (int k = 0; k < 2; ++k) {
            float rr = base * (1.56f + k * 0.10f + 0.006f * sinf(elapsed * 1.8f + k));
            float aa = ba * (0.075f - k * 0.025f);
            if (aa > 0.004f) {
                dl->AddCircle(float_cnt, rr, IM_COL32(95, 145, 200, (int)(aa * 255.0f)), 160, (0.9f + k * 0.3f) * us);
            }
        }
    }
    if (elapsed >= T_TEXT_BEG && elapsed < T_PULSE_BEG && ring_alpha > 0.01f) {
        float mt = (elapsed - T_TEXT_BEG) / (T_PULSE_BEG - T_TEXT_BEG);
        if (mt < 0.0f) mt = 0.0f; if (mt > 1.0f) mt = 1.0f;
        float ma = (mt < 0.16f ? EaseOutCubic(mt / 0.16f) : 1.0f) *
                   (mt > 0.82f ? (1.0f - EaseOutCubic((mt - 0.82f) / 0.18f)) : 1.0f) * global_alpha;
        float scan_r = current_ring_r * (1.28f + 0.035f * sinf(elapsed * 3.0f));
        float corner = scan_r * 0.18f;
        float box_w = scan_r * 1.72f;
        float box_h = scan_r * 1.22f;
        float ba = ma * 0.22f;
        if (ba > 0.004f) {
            ImVec2 tl(float_cnt.x - box_w * 0.5f, float_cnt.y - box_h * 0.5f);
            ImVec2 tr(float_cnt.x + box_w * 0.5f, float_cnt.y - box_h * 0.5f);
            ImVec2 bl(float_cnt.x - box_w * 0.5f, float_cnt.y + box_h * 0.5f);
            ImVec2 br(float_cnt.x + box_w * 0.5f, float_cnt.y + box_h * 0.5f);
            ImU32 bc = IM_COL32(190, 220, 245, (int)(ba * 255.0f));
            dl->AddLine(tl, ImVec2(tl.x + corner, tl.y), bc, 1.1f * us); dl->AddLine(tl, ImVec2(tl.x, tl.y + corner), bc, 1.1f * us);
            dl->AddLine(tr, ImVec2(tr.x - corner, tr.y), bc, 1.1f * us); dl->AddLine(tr, ImVec2(tr.x, tr.y + corner), bc, 1.1f * us);
            dl->AddLine(bl, ImVec2(bl.x + corner, bl.y), bc, 1.1f * us); dl->AddLine(bl, ImVec2(bl.x, bl.y - corner), bc, 1.1f * us);
            dl->AddLine(br, ImVec2(br.x - corner, br.y), bc, 1.1f * us); dl->AddLine(br, ImVec2(br.x, br.y - corner), bc, 1.1f * us);
            float sy = tl.y + fmodf(elapsed * 145.0f, box_h);
            dl->AddRectFilled(ImVec2(tl.x, sy - 1.0f * us), ImVec2(tr.x, sy + 1.0f * us),
                IM_COL32(170, 225, 255, (int)(ma * 34.0f)), 0.5f * us);
        }
        const int FLOWS = 9;
        const int FSEG = 22;
        for (int f = 0; f < FLOWS; ++f) {
            float seed = f * 2.37f + 0.5f;
            float ft = fmodf(mt * (0.82f + 0.04f * f) + f * 0.11f, 1.0f);
            float side = (f & 1) ? 1.0f : -1.0f;
            ImVec2 p0(float_cnt.x + side * ring_r * 1.85f, float_cnt.y + sinf(seed) * ring_r * 0.82f);
            ImVec2 p1(float_cnt.x + side * ring_r * 1.05f, float_cnt.y - ring_r * (0.72f + 0.12f * cosf(seed)));
            ImVec2 p2(float_cnt.x - side * ring_r * 0.60f, float_cnt.y + ring_r * (0.58f + 0.08f * sinf(seed)));
            ImVec2 p3(float_cnt.x + cosf(seed + elapsed * 0.45f) * ring_r * 0.38f,
                      float_cnt.y + sinf(seed + elapsed * 0.45f) * ring_r * 0.24f);
            ImVec2 pts[FSEG + 1];
            int pc = 0;
            float head = EaseOutCubic(ft);
            float tail = head - 0.34f;
            if (tail < 0.0f) tail = 0.0f;
            for (int s = 0; s <= FSEG; ++s) {
                float t = tail + (head - tail) * (float)s / (float)FSEG;
                float u = 1.0f - t;
                pts[pc++] = ImVec2(u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
                                   u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y);
            }
            float fa = ma * (0.12f + 0.035f * (f % 3));
            fa *= 0.86f + 0.14f * fabsf(sinf(elapsed * 2.8f + seed));
            if (fa > 0.004f) {
                dl->AddPolyline(pts, pc, IM_COL32(120, 185, 245, (int)(fa * 255.0f)), ImDrawFlags_None, 1.0f * us);
                ImVec2 hp = pts[pc - 1];
                dl->AddCircleFilled(hp, 2.0f * us, IM_COL32(240, 250, 255, (int)(fa * 255.0f)), 10);
            }
        }
        const int CODE_ROWS = 10;
        for (int r = 0; r < CODE_ROWS; ++r) {
            float side = (r & 1) ? 1.0f : -1.0f;
            float y = float_cnt.y - ring_r * 0.58f + r * ring_r * 0.13f;
            float x = float_cnt.x + side * ring_r * (1.05f + 0.04f * sinf(elapsed * 2.0f + r));
            float row_a = ma * (0.055f + 0.035f * fabsf(sinf(elapsed * 5.0f + r)));
            if (row_a < 0.004f) continue;
            for (int k = 0; k < 4; ++k) {
                float w = ring_r * (0.10f + 0.035f * ((r + k) % 4));
                float ox = side * k * ring_r * 0.13f;
                dl->AddRectFilled(ImVec2(x + ox, y - 1.2f*us), ImVec2(x + ox + side * w, y + 1.2f*us),
                    IM_COL32(185, 210, 235, (int)(row_a * 255.0f)), 0.7f * us);
            }
        }
        const int SIDE_ROWS = 7;
        for (int side_i = 0; side_i < 2; ++side_i) {
            float side = side_i == 0 ? -1.0f : 1.0f;
            float panel_x = side_i == 0 ? scr.x * 0.075f : scr.x * 0.925f;
            float panel_w = scr.x * 0.105f;
            float panel_h = scr.y * 0.42f;
            float panel_y = cnt.y - panel_h * 0.5f + sinf(elapsed * 1.4f + side_i) * 5.0f * us;
            float pa = ma * 0.18f;
            if (pa > 0.004f) {
                dl->AddLine(ImVec2(panel_x, panel_y), ImVec2(panel_x, panel_y + panel_h),
                    IM_COL32(110, 175, 235, (int)(pa * 255.0f)), 1.2f * us);
                dl->AddLine(ImVec2(panel_x, panel_y), ImVec2(panel_x - side * panel_w * 0.55f, panel_y),
                    IM_COL32(150, 205, 245, (int)(pa * 210.0f)), 1.0f * us);
                dl->AddLine(ImVec2(panel_x, panel_y + panel_h), ImVec2(panel_x - side * panel_w * 0.55f, panel_y + panel_h),
                    IM_COL32(150, 205, 245, (int)(pa * 210.0f)), 1.0f * us);
                for (int r = 0; r < SIDE_ROWS; ++r) {
                    float ry = panel_y + panel_h * (0.10f + r * 0.13f);
                    float blink = 0.68f + 0.32f * fabsf(sinf(elapsed * (2.2f + r * 0.18f) + r + side_i));
                    float row_a = pa * blink;
                    float len = panel_w * (0.20f + 0.12f * ((r + side_i) % 4));
                    float x0 = panel_x;
                    float x1 = panel_x - side * len;
                    dl->AddLine(ImVec2(x0, ry), ImVec2(x1, ry),
                        IM_COL32(200, 230, 250, (int)(row_a * 230.0f)), 1.0f * us);
                    dl->AddRectFilled(ImVec2(x1 - 2.0f*us, ry - 2.0f*us), ImVec2(x1 + 2.0f*us, ry + 2.0f*us),
                        IM_COL32(235, 248, 255, (int)(row_a * 190.0f)), 0.6f * us);
                    if ((r % 2) == 0) {
                        const int CSEG = 14;
                        ImVec2 cps[CSEG + 1];
                        ImVec2 p0(x1, ry);
                        ImVec2 p1(panel_x + side * ring_r * 0.18f, ry + sinf(r + elapsed) * 18.0f * us);
                        ImVec2 p2(float_cnt.x + side * ring_r * 1.55f, float_cnt.y + (r - 3) * ring_r * 0.12f);
                        ImVec2 p3(float_cnt.x + side * ring_r * 1.23f, float_cnt.y + (r - 3) * ring_r * 0.08f);
                        for (int s = 0; s <= CSEG; ++s) {
                            float t = (float)s / (float)CSEG;
                            float u = 1.0f - t;
                            cps[s] = ImVec2(u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
                                            u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y);
                        }
                        dl->AddPolyline(cps, CSEG + 1,
                            IM_COL32(85, 155, 225, (int)(row_a * 92.0f)), ImDrawFlags_None, 0.8f * us);
                    }
                }
            }
        }
    }
    if (elapsed >= T_BREATH_BEG && elapsed < T_PULSE_BEG && ring_alpha > 0.01f) {
        const int np = 5;
        float pr = current_ring_r * 1.08f;
        for (int i = 0; i < np; ++i) {
            float angle = _particle_phase + (float)i / np * 2.0f * IM_PI;
            float px = float_cnt.x + cosf(angle) * pr * ring_aspect;
            float py = float_cnt.y + sinf(angle) * pr;
            float pa = (breath_glow * 1.2f + 0.25f) * global_alpha;
            if (pa > 1.0f) pa = 1.0f;
            dl->AddCircleFilled(ImVec2(px, py), 2.2f * us,
                IM_COL32(220, 230, 250, (int)(pa * 255.0f)), 16);
            dl->AddCircleFilled(ImVec2(px, py), 1.0f * us,
                IM_COL32(205, 226, 240, (int)(pa * 115.0f)), 8);
        }
    }
    if (burst_flash > 0.005f) {
        dl->AddRectFilled(ImVec2(0, 0), scr,
            IM_COL32(198, 222, 238, (int)(burst_flash * global_alpha * 105.0f)));
    }
    if (text_burst > 0.005f && elapsed < T_PULSE_BEG) {
        for (int i = 0; i < 3; ++i) {
            float br = ring_r * 0.32f * (0.55f + i * 0.38f + text_burst * 0.75f);
            float ba = text_burst * global_alpha * (0.16f - i * 0.035f);
            if (ba < 0.005f) continue;
            dl->AddCircle(float_cnt, br, IM_COL32(165, 205, 235, (int)(ba * 255.0f)), 96, 1.6f * us);
        }
    }
    if (elapsed >= T_TEXT_BEG && elapsed < T_TEXT_END + 0.70f) {
        for (int i = 0; i < TEXTP_N; ++i) {
            float pa = textp_alpha[i] * global_alpha;
            if (pa < 0.005f) continue;
            float ps = 2.5f * us, px = textp_x[i], py = textp_y[i];
            if (i < 24) {
                float trail = pa * 16.0f * us;
                float angle = atan2f(py - cnt.y, px - cnt.x);
                dl->AddLine(ImVec2(px, py), ImVec2(px - cosf(angle)*trail, py - sinf(angle)*trail),
                    IM_COL32(200, 215, 245, (int)(pa * 110.0f)), 1.4f * us);
            }
            dl->AddCircleFilled(ImVec2(px, py), ps, IM_COL32(218, 232, 242, (int)(pa * 175.0f)), 16);
            dl->AddCircleFilled(ImVec2(px, py), ps * 0.32f, IM_COL32(188, 215, 235, (int)(pa * 120.0f)), 8);
        }
    }
    if (text_alpha > 0.005f && elapsed < T_PULSE_BEG) {
        const char* str = "YCloud UI";
        const float fs = text_fs * text_scale;
        ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(fs, FLT_MAX, -1.0f, str);
        float ta = text_alpha * global_alpha;
        float text_float_y = sinf(elapsed * 1.35f * IM_PI) * (0.55f * us);
        ImVec2 tp(cnt.x - ts.x * 0.5f, cnt.y - ts.y * 0.5f + text_float_y);
        float ga = ta * (0.08f + text_burst * 0.28f);
        dl->AddText(ImGui::GetFont(), fs, ImVec2(tp.x - 1.7f*us, tp.y - 1.7f*us),
            IM_COL32(120, 170, 220, (int)(ga * 115.0f)), str);
        dl->AddText(ImGui::GetFont(), fs, ImVec2(tp.x + 1.8f*us, tp.y + 2.0f*us),
            IM_COL32(0, 0, 0, (int)(ta * 115.0f)), str);
        dl->AddText(ImGui::GetFont(), fs, tp,
            IM_COL32(218, 232, 245, (int)(ta * 245.0f)), str);
        if (text_flash > 0.005f)
            dl->AddText(ImGui::GetFont(), fs, tp,
                IM_COL32(190, 225, 250, (int)(text_flash * global_alpha * 180.0f)), str);
    }
    if (elapsed >= T_BURST_BEG && elapsed < T_BURST_END + 0.4f) {
        float blt = (elapsed - T_BURST_BEG) / (T_BURST_END - T_BURST_BEG);
        if (blt < 0.0f) blt = 0.0f; if (blt > 1.0f) blt = 1.0f;
        float hot = 1.0f - EaseOutCubic(blt);
        if (hot > 0.01f) {
            dl->AddCircleFilled(cnt, ball_r * (0.66f + hot * 0.42f),
                IM_COL32(205, 228, 245, (int)(hot * global_alpha * 92.0f)), 96);
            dl->AddCircle(cnt, ball_r * (1.25f + (1.0f - hot) * 0.34f),
                IM_COL32(105, 180, 230, (int)(hot * global_alpha * 118.0f)), 128, (4.8f + hot * 2.6f) * us);
        }
        {
            float blast = EaseInOutCubic(blt);
            float gate_a = (1.0f - blt) * global_alpha;
            if (gate_a > 0.006f) {
                const float g = 54.0f * us;
                int cols = (int)(scr.x / g) + 3;
                int rows = (int)(scr.y / g) + 3;
                float shock_front = ball_r * 0.35f + blast * diag * 1.22f;
                for (int x = 0; x < cols; ++x) {
                    float px = (x - 1) * g + fmodf(elapsed * 18.0f * us, g);
                    float d = fabsf(px - cnt.x);
                    float band = 1.0f - fabsf(d - shock_front) / (135.0f * us);
                    if (band < 0.0f) band = 0.0f; if (band > 1.0f) band = 1.0f;
                    band = band * band * (3.0f - 2.0f * band);
                    float a = gate_a * band * 0.30f;
                    if (a > 0.004f)
                        dl->AddLine(ImVec2(px, 0), ImVec2(px, scr.y), IM_COL32(70, 160, 255, (int)(a * 255.0f)), (0.75f + band * 1.2f) * us);
                }
                for (int y = 0; y < rows; ++y) {
                    float py = (y - 1) * g - fmodf(elapsed * 12.0f * us, g);
                    float d = fabsf(py - cnt.y);
                    float band = 1.0f - fabsf(d - shock_front) / (120.0f * us);
                    if (band < 0.0f) band = 0.0f; if (band > 1.0f) band = 1.0f;
                    band = band * band * (3.0f - 2.0f * band);
                    float a = gate_a * band * 0.24f;
                    if (a > 0.004f)
                        dl->AddLine(ImVec2(0, py), ImVec2(scr.x, py), IM_COL32(105, 205, 255, (int)(a * 255.0f)), (0.65f + band * 1.0f) * us);
                }
                const int SHARDS = 44;
                for (int i = 0; i < SHARDS; ++i) {
                    float seed = (float)i * 17.371f + sinf((float)i * 0.73f) * 3.0f;
                    float lane = (float)(i % 3) * 0.070f;
                    float delay = (float)(i % 7) * 0.012f;
                    float st = (blt - delay) / (1.0f - delay);
                    if (st < 0.0f) st = 0.0f; if (st > 1.0f) st = 1.0f;
                    float se = EaseInOutCubic(st);
                    float ang = (float)i / (float)SHARDS * 2.0f * IM_PI + sinf(seed) * 0.055f + blt * (0.32f + lane);
                    float radial_noise = 0.90f + 0.16f * fabsf(sinf(seed * 1.41f));
                    float r = ring_r * (0.60f + lane) + se * diag * (0.78f + lane * 0.70f) * radial_noise;
                    float sx = cnt.x + cosf(ang) * r;
                    float sy = cnt.y + sinf(ang) * r;
                    float edge_soft = 1.0f - (r / (diag * 1.05f));
                    if (edge_soft < 0.0f) edge_soft = 0.0f;
                    float sa = gate_a * edge_soft * (0.18f + 0.36f * (1.0f - st));
                    if (sa < 0.005f) continue;
                    float sz = (2.4f + (i % 4) * 0.85f) * us * (1.0f + (1.0f - st) * 0.55f);
                    float rot = 0.785f + ang + st * (1.7f + (i % 3) * 0.35f);
                    float c = cosf(rot), s = sinf(rot);
                    ImVec2 p[4] = {
                        {sx + (-sz*c - -sz*s), sy + (-sz*s + -sz*c)},
                        {sx + ( sz*c - -sz*s), sy + ( sz*s + -sz*c)},
                        {sx + ( sz*c -  sz*s), sy + ( sz*s +  sz*c)},
                        {sx + (-sz*c -  sz*s), sy + (-sz*s +  sz*c)},
                    };
                    ImU32 sqc = IM_COL32(184, 214, 235, (int)(sa * 210.0f));
                    ImU32 glowc = IM_COL32(80, 150, 215, (int)(sa * 70.0f));
                    for (int j = 0; j < 4; ++j) {
                        dl->AddLine(p[j], p[(j+1)%4], glowc, 2.0f * us);
                        dl->AddLine(p[j], p[(j+1)%4], sqc, 0.85f * us);
                    }
                    float tail = (10.0f + (i % 5) * 5.5f) * us * (1.0f - st * 0.45f);
                    ImVec2 tp0(sx - cosf(ang) * tail, sy - sinf(ang) * tail);
                    dl->AddLine(tp0, ImVec2(sx, sy), IM_COL32(90, 165, 220, (int)(sa * 82.0f)), (0.65f + (i % 3) * 0.18f) * us);
                }
                const int RAYS = 16;
                for (int r = 0; r < RAYS; ++r) {
                    float a0 = (float)r / (float)RAYS * 2.0f * IM_PI + sinf(r * 2.17f) * 0.055f;
                    float pulse = 0.5f + 0.5f * sinf(elapsed * 11.0f + r * 1.31f);
                    float ra = gate_a * (0.055f + pulse * 0.095f) * (1.0f - blt * 0.45f);
                    if (ra < 0.006f) continue;
                    float r0 = ring_r * (1.02f + 0.14f * sinf(r * 0.63f)) + blast * ring_r * 0.42f;
                    float r1 = r0 + diag * (0.14f + 0.11f * fabsf(sinf(r * 1.37f))) * (0.55f + blast * 0.38f);
                    ImVec2 p0(cnt.x + cosf(a0) * r0, cnt.y + sinf(a0) * r0);
                    ImVec2 p1(cnt.x + cosf(a0) * r1, cnt.y + sinf(a0) * r1);
                    dl->AddLine(p0, p1, IM_COL32(125, 190, 230, (int)(ra * 255.0f)), (0.75f + pulse * 0.45f) * us);
                }
            }
        }
        for (int w = 0; w < BWAVES; ++w) {
            float wa = bwave_a[w] * global_alpha;
            if (wa < 0.005f) continue;
            float wr = bwave_r[w];
            if (wr < 0.5f) continue;
            float wt = bwave_t[w];
            float wave_hot = wa * (1.0f - 0.08f * w);
            int gray = 225 + w * 8;
            if (gray > 255) gray = 255;
            dl->AddCircle(cnt, wr, IM_COL32(58, 130, 210, (int)(wave_hot * 58.0f)), 192, wt * 5.2f);
            dl->AddCircle(cnt, wr, IM_COL32(110, 185, 230, (int)(wave_hot * 88.0f)), 192, wt * 2.35f);
            float off = (1.4f + w * 0.38f) * us;
            dl->AddCircle(ImVec2(cnt.x - off, cnt.y), wr * 1.001f, IM_COL32(75, 150, 215, (int)(wave_hot * 56.0f)), 192, wt * 0.92f);
            dl->AddCircle(ImVec2(cnt.x + off, cnt.y), wr * 0.999f, IM_COL32(170, 210, 235, (int)(wave_hot * 54.0f)), 192, wt * 0.78f);
            dl->AddCircle(cnt, wr, IM_COL32(205, 226, 240, (int)(wave_hot * 142.0f)), 192, wt * 0.95f);
            dl->AddCircle(cnt, wr, IM_COL32(128, 185, 225, (int)(wave_hot * 92.0f)), 192, wt * 0.42f);
            if (w < 2 && wave_hot > 0.02f) {
                const int TICKS = 28;
                for (int k = 0; k < TICKS; ++k) {
                    if (((k + w) % 3) == 0) continue;
                    float a = (float)k / (float)TICKS * 2.0f * IM_PI + elapsed * (0.09f + w * 0.022f);
                    float jitter = sinf(k * 12.989f + w * 4.37f) * 0.5f + 0.5f;
                    float r0 = wr + (2.0f + jitter * 5.0f) * us;
                    float len = (6.0f + jitter * 12.0f) * us * (1.0f - blt * 0.32f);
                    ImVec2 p0(cnt.x + cosf(a) * r0, cnt.y + sinf(a) * r0);
                    ImVec2 p1(cnt.x + cosf(a) * (r0 + len), cnt.y + sinf(a) * (r0 + len));
                    float ta = wave_hot * (0.12f + jitter * 0.18f);
                    dl->AddLine(p0, p1, IM_COL32(176, 212, 235, (int)(ta * 255.0f)), (0.72f + jitter * 0.55f) * us);
                }
            }
        }
        const int FP = 22;
        for (int i = 0; i < FP; ++i) {
            float fa = (float)i / FP * 2.0f * IM_PI + burst_particles_phase + sinf(i * 1.31f) * 0.10f;
            float fr = ball_r * 0.35f + burst_particles_phase * diag * (0.72f + 0.10f * fabsf(sinf(i * 0.77f)));
            float fx = cnt.x + cosf(fa) * fr;
            float fy = cnt.y + sinf(fa) * fr;
            float fp_a = (1.0f - (fr / (diag * 1.05f))) * global_alpha * 0.78f;
            if (fp_a < 0.005f) continue;
            float tail = (10.0f + (i % 5) * 3.8f) * us;
            dl->AddLine(ImVec2(fx - cosf(fa) * tail, fy - sinf(fa) * tail), ImVec2(fx, fy),
                IM_COL32(90, 160, 220, (int)(fp_a * 92.0f)), (0.85f + (i % 3) * 0.22f) * us);
            dl->AddCircleFilled(ImVec2(fx, fy), (1.55f + (i % 3) * 0.38f) * us,
                IM_COL32(190, 220, 238, (int)(fp_a * 150.0f)), 10);
        }
    }
    {
        const float T_WORLD_BEG = T_PULSE_BEG - 0.18f;
        const float T_WORLD_END = T_BURST_END + 1.08f;
        if (elapsed >= T_WORLD_BEG && elapsed < T_WORLD_END) {
            float wt_all = (elapsed - T_WORLD_BEG) / (T_WORLD_END - T_WORLD_BEG);
            if (wt_all < 0.0f) wt_all = 0.0f; if (wt_all > 1.0f) wt_all = 1.0f;
            float wa_in = wt_all < 0.18f ? EaseOutCubic(wt_all / 0.18f) : 1.0f;
            float wa_out = wt_all > 0.68f ? (1.0f - EaseOutCubic((wt_all - 0.68f) / 0.32f)) : 1.0f;
            if (wa_out < 0.0f) wa_out = 0.0f;
            float dissolve = 1.0f;
            if (wt_all > 0.62f) {
                float dt = (wt_all - 0.62f) / 0.38f;
                if (dt > 1.0f) dt = 1.0f;
                dissolve = 1.0f - dt * 0.28f;
            }
            float wa_base = wa_in * wa_out * dissolve * global_alpha;
            const int LINES = 8;
            const int SEG = 36;
            for (int l = 0; l < LINES; ++l) {
                float seed = (float)l * 1.731f + 0.37f;
                float delay = (float)l * 0.018f;
                float lt = (wt_all - delay) / (1.0f - delay);
                if (lt < 0.0f) lt = 0.0f; if (lt > 1.0f) lt = 1.0f;
                float converge = EaseInOutCubic(lt);
                float side = (l & 1) ? 1.0f : -1.0f;
                float start_ang = -IM_PI * 0.85f + (float)l / (LINES - 1) * IM_PI * 1.70f;
                float start_r = diag * (0.54f + 0.035f * sinf(seed));
                ImVec2 p0(cnt.x + cosf(start_ang) * start_r, cnt.y + sinf(start_ang) * start_r * 0.72f);
                ImVec2 p3(cnt.x + cosf(start_ang + side * 0.42f) * ring_r * (1.04f - 0.82f * converge),
                          cnt.y + sinf(start_ang + side * 0.42f) * ring_r * (1.04f - 0.82f * converge));
                p3.x = p3.x + (cnt.x - p3.x) * EaseInOutCubic(converge);
                p3.y = p3.y + (cnt.y - p3.y) * EaseInOutCubic(converge);
                ImVec2 p1(cnt.x + cosf(start_ang + side * 0.95f) * diag * (0.30f - 0.18f * converge),
                          cnt.y + sinf(start_ang + side * 0.95f) * diag * (0.20f - 0.12f * converge));
                ImVec2 p2(cnt.x + cosf(start_ang - side * 0.72f) * ring_r * (1.75f - 1.10f * converge),
                          cnt.y + sinf(start_ang - side * 0.72f) * ring_r * (1.15f - 0.76f * converge));
                ImVec2 pts[SEG + 1];
                int max_seg = 0;
                float draw_head = EaseInOutCubic(lt);
                for (int s = 0; s <= SEG; ++s) {
                    float t = (float)s / (float)SEG;
                    if (t > draw_head) break;
                    float u = 1.0f - t;
                    pts[max_seg++] = ImVec2(
                        u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
                        u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y
                    );
                }
                if (max_seg > 2) {
                    float line_focus = 0.72f + 0.28f * sinf(wt_all * IM_PI);
                    float la = wa_base * (0.20f + 0.045f * (float)(l % 3)) * (1.0f - converge * 0.28f) * line_focus;
                    if (la > 0.004f) {
                        dl->AddPolyline(pts, max_seg, IM_COL32(112, 158, 205, (int)(la * 230.0f)), ImDrawFlags_None, 1.28f * us);
                        dl->AddPolyline(pts, max_seg, IM_COL32(176, 212, 235, (int)(la * 118.0f)), ImDrawFlags_None, 3.25f * us);
                        dl->AddPolyline(pts, max_seg, IM_COL32(70, 130, 190, (int)(la * 42.0f)), ImDrawFlags_None, 5.80f * us);
                        float scan = fmodf(wt_all * 1.85f + l * 0.137f, 1.0f);
                        float hi_pts[4] = { scan - 0.055f, scan - 0.025f, scan + 0.025f, scan + 0.055f };
                        ImVec2 hp[4];
                        int hc = 0;
                        for (int h = 0; h < 4; ++h) {
                            float ht = hi_pts[h];
                            if (ht < 0.0f || ht > draw_head || ht > 1.0f) continue;
                            float hu = 1.0f - ht;
                            hp[hc++] = ImVec2(
                                hu*hu*hu*p0.x + 3*hu*hu*ht*p1.x + 3*hu*ht*ht*p2.x + ht*ht*ht*p3.x,
                                hu*hu*hu*p0.y + 3*hu*hu*ht*p1.y + 3*hu*ht*ht*p2.y + ht*ht*ht*p3.y
                            );
                        }
                        if (hc > 1) {
                            float ha = wa_base * (0.20f + 0.06f * sinf(elapsed * 2.2f + seed)) * (1.0f - converge * 0.18f);
                            dl->AddPolyline(hp, hc, IM_COL32(205, 228, 240, (int)(ha * 188.0f)), ImDrawFlags_None, 2.05f * us);
                            dl->AddPolyline(hp, hc, IM_COL32(120, 180, 225, (int)(ha * 82.0f)), ImDrawFlags_None, 5.20f * us);
                        }
                    }
                    ImVec2 head = pts[max_seg - 1];
                    float na = wa_base * (0.30f - converge * 0.14f);
                    if (na > 0.004f) {
                        dl->AddCircleFilled(head, (1.55f + (float)(l % 3) * 0.55f) * us, IM_COL32(190, 218, 236, (int)(na * 190.0f)), 10);
                        dl->AddRect(ImVec2(head.x - 4.6f*us, head.y - 4.6f*us), ImVec2(head.x + 4.6f*us, head.y + 4.6f*us),
                            IM_COL32(100, 145, 190, (int)(na * 62.0f)), 1.0f * us, 0, 0.65f * us);
                    }
                    const int CONES_PER_LINE = 2;
                    for (int c = 0; c < CONES_PER_LINE; ++c) {
                        float flow = fmodf(lt * (1.35f + 0.08f * l) + c * 0.31f + l * 0.047f, 1.0f);
                        float force = wt_all > 0.58f ? EaseInOutCubic((wt_all - 0.58f) / 0.42f) : 0.0f;
                        if (force > 1.0f) force = 1.0f;
                        flow = flow + (1.0f - flow) * force * 0.86f;
                        if (flow > draw_head) continue;
                        float u0 = 1.0f - flow;
                        ImVec2 cp = ImVec2(
                            u0*u0*u0*p0.x + 3*u0*u0*flow*p1.x + 3*u0*flow*flow*p2.x + flow*flow*flow*p3.x,
                            u0*u0*u0*p0.y + 3*u0*u0*flow*p1.y + 3*u0*flow*flow*p2.y + flow*flow*flow*p3.y
                        );
                        float f2 = flow + 0.015f;
                        if (f2 > 1.0f) f2 = 1.0f;
                        float u2 = 1.0f - f2;
                        ImVec2 np2 = ImVec2(
                            u2*u2*u2*p0.x + 3*u2*u2*f2*p1.x + 3*u2*f2*f2*p2.x + f2*f2*f2*p3.x,
                            u2*u2*u2*p0.y + 3*u2*u2*f2*p1.y + 3*u2*f2*f2*p2.y + f2*f2*f2*p3.y
                        );
                        float ang = atan2f(np2.y - cp.y, np2.x - cp.x);
                        float vx = cosf(ang), vy = sinf(ang);
                        float nx = -vy, ny = vx;
                        float cone_len = (7.0f + c * 1.45f + 3.0f * force) * us;
                        float cone_w = (2.45f + c * 0.42f) * us;
                        float ca = wa_base * (0.20f + 0.08f * c) * (1.0f - flow * 0.22f);
                        if (ca < 0.005f) continue;
                        ImVec2 tip(cp.x + vx * cone_len * 0.62f, cp.y + vy * cone_len * 0.62f);
                        ImVec2 back(cp.x - vx * cone_len * 0.38f, cp.y - vy * cone_len * 0.38f);
                        ImVec2 tri[3] = {
                            tip,
                            ImVec2(back.x + nx * cone_w, back.y + ny * cone_w),
                            ImVec2(back.x - nx * cone_w, back.y - ny * cone_w)
                        };
                        dl->AddTriangleFilled(tri[0], tri[1], tri[2], IM_COL32(188, 218, 236, (int)(ca * 168.0f)));
                        dl->AddTriangle(tri[0], tri[1], tri[2], IM_COL32(85, 140, 190, (int)(ca * 128.0f)), 0.85f * us);
                        for (int tr = 0; tr < 3; ++tr) {
                            float tl = cone_len * (0.85f + tr * 0.56f) * (0.60f + force * 0.40f);
                            float tw = (1.45f - tr * 0.30f) * us;
                            float ta = ca * (0.18f - tr * 0.040f);
                            if (ta < 0.004f) continue;
                            dl->AddLine(ImVec2(cp.x - vx * tl, cp.y - vy * tl),
                                        ImVec2(cp.x - vx * cone_len * 0.28f, cp.y - vy * cone_len * 0.28f),
                                        IM_COL32(95, 160, 215, (int)(ta * 255.0f)), tw);
                        }
                    }
                }
            }
            const int W_PARTS = 56;
            for (int p = 0; p < W_PARTS; ++p) {
                float ps = (float)p * 2.417f + 0.19f;
                float pd = fmodf(fabsf(sinf(ps * 1.37f)) * 0.22f, 0.22f);
                float pt = (wt_all - 0.34f - pd) / 0.58f;
                if (pt < 0.0f) continue;
                if (pt > 1.0f) pt = 1.0f;
                float pe = EaseInOutCubic(pt);
                float a = ps + sinf(elapsed * 1.8f + ps) * 0.25f;
                float sr = ring_r * (1.55f + 0.78f * fabsf(cosf(ps * 0.73f)));
                float swirl = (1.0f - pe) * 1.35f * ((p & 1) ? 1.0f : -1.0f);
                float rr = sr + (ball_r * 0.35f - sr) * pe;
                float px = cnt.x + cosf(a + swirl) * rr;
                float py = cnt.y + sinf(a + swirl) * rr * (0.72f + 0.28f * pe);
                float pa = wa_base * (pt < 0.18f ? pt / 0.18f : 1.0f) * (1.0f - pe * 0.76f) * 0.82f;
                if (pa < 0.004f) continue;
                float vang = atan2f(cnt.y - py, cnt.x - px);
                float tvx = cosf(vang), tvy = sinf(vang);
                float trail = (8.0f + 18.0f * pe) * us;
                dl->AddLine(ImVec2(px - tvx * trail, py - tvy * trail), ImVec2(px, py),
                    IM_COL32(95, 155, 210, (int)(pa * 78.0f)), 0.82f * us);
                if ((p % 3) == 0) {
                    float sz = (1.35f + (p % 5) * 0.25f) * us;
                    dl->AddRectFilled(ImVec2(px - sz, py - sz), ImVec2(px + sz, py + sz),
                        IM_COL32(182, 212, 232, (int)(pa * 150.0f)), 0.45f * us);
                } else {
                    dl->AddCircleFilled(ImVec2(px, py), (1.20f + (p % 4) * 0.20f) * us,
                        IM_COL32(170, 205, 230, (int)(pa * 138.0f)), 8);
                }
            }
            const int ORBITS = 5;
            for (int o = 0; o < ORBITS; ++o) {
                float od = (float)o * 0.055f;
                float ot = (wt_all - od) / (1.0f - od);
                if (ot < 0.0f) ot = 0.0f; if (ot > 1.0f) ot = 1.0f;
                float oe = EaseInOutCubic(ot);
                float rr = ring_r * (1.95f - oe * (1.72f - o * 0.045f));
                float oa = wa_base * (0.085f - o * 0.010f) * (1.0f - fabsf(oe - 0.58f) * 0.9f);
                if (oa < 0.004f) continue;
                dl->AddCircle(cnt, rr, IM_COL32(160, 195, 220, (int)(oa * 255.0f)), 144, (0.78f + o * 0.24f) * us);
                for (int k = 0; k < 2; ++k) {
                    float a = elapsed * (0.86f + o * 0.12f) + (float)k / 2.0f * 2.0f * IM_PI + o;
                    ImVec2 pp(cnt.x + cosf(a) * rr, cnt.y + sinf(a) * rr);
                    float pa = oa * 2.2f;
                    if (pa > 1.0f) pa = 1.0f;
                    dl->AddCircleFilled(pp, (1.35f + k * 0.32f) * us, IM_COL32(180, 212, 232, (int)(pa * 128.0f)), 8);
                }
            }
            if (wt_all > 0.70f) {
                float ct = (wt_all - 0.70f) / 0.30f;
                if (ct > 1.0f) ct = 1.0f;
                float ce = EaseOutCubic(ct);
                float breathe = 0.5f + 0.5f * sinf(elapsed * 2.65f);
                float ca = wa_base * ce * 0.86f;
                float cr = ball_r * (2.75f - 2.02f * ce);
                dl->AddCircle(cnt, cr * (1.08f + 0.035f * breathe), IM_COL32(150, 195, 225, (int)(ca * 74.0f)), 144, 4.20f * us);
                dl->AddCircle(cnt, cr * (0.78f + 0.025f * breathe), IM_COL32(195, 222, 238, (int)(ca * 138.0f)), 144, 1.85f * us);
                dl->AddCircleFilled(cnt, ball_r * (0.30f + 0.18f * sinf(ct * IM_PI) + 0.025f * breathe),
                    IM_COL32(190, 220, 235, (int)(ca * 118.0f)), 56);
                dl->AddCircleFilled(cnt, ball_r * (0.16f + 0.030f * breathe),
                    IM_COL32(218, 232, 242, (int)(ca * 92.0f)), 40);
                const int CORE_DOTS = 6;
                for (int d = 0; d < CORE_DOTS; ++d) {
                    float da = elapsed * (0.92f + d * 0.035f) + (float)d / CORE_DOTS * 2.0f * IM_PI;
                    float dr = ball_r * (0.72f + 0.11f * sinf(elapsed * 1.45f + d));
                    ImVec2 dp(cnt.x + cosf(da) * dr, cnt.y + sinf(da) * dr * 0.72f);
                    float dda = ca * (0.34f + 0.10f * sinf(da));
                    if (dda > 0.004f) {
                        dl->AddCircleFilled(dp, (1.10f + (d % 2) * 0.28f) * us, IM_COL32(185, 218, 236, (int)(dda * 170.0f)), 10);
                        dl->AddCircle(dp, (2.25f + (d % 3) * 0.34f) * us, IM_COL32(105, 165, 215, (int)(dda * 76.0f)), 10, 0.72f * us);
                    }
                }
            }
            if (wt_all > 0.84f) {
                float bt = (wt_all - 0.84f) / 0.16f;
                if (bt > 1.0f) bt = 1.0f;
                float be = EaseOutCubic(bt);
                float ba = (1.0f - bt) * global_alpha;
                if (bt < 0.32f) {
                    float flash = (1.0f - bt / 0.32f) * global_alpha;
                    dl->AddCircleFilled(cnt, ball_r * (0.70f + bt * 1.35f),
                        IM_COL32(195, 222, 238, (int)(flash * 96.0f)), 64);
                }
                for (int q = 0; q < 3; ++q) {
                    float qd = q * 0.13f;
                    float qt = (bt - qd) / (1.0f - qd);
                    if (qt < 0.0f) continue; if (qt > 1.0f) qt = 1.0f;
                    float qe = EaseInOutCubic(qt);
                    float rr = ball_r * 0.6f + qe * ring_r * (1.22f + q * 0.26f);
                    float qa = (1.0f - qt) * global_alpha * (0.26f - q * 0.055f);
                    if (qa < 0.004f) continue;
                    dl->AddCircle(cnt, rr, IM_COL32(175, 210, 232, (int)(qa * 220.0f)), 144, Lerp(3.8f, 0.9f, qe) * us);
                    dl->AddCircle(cnt, rr, IM_COL32(80, 150, 215, (int)(qa * 54.0f)), 144, Lerp(9.0f, 2.0f, qe) * us);
                }
                const int BURST_CONES = 24;
                for (int i = 0; i < BURST_CONES; ++i) {
                    float seed = i * 2.618f + 0.33f;
                    float id = fmodf(fabsf(sinf(seed)) * 0.14f, 0.14f);
                    float it = (bt - id) / (1.0f - id);
                    if (it < 0.0f) continue; if (it > 1.0f) it = 1.0f;
                    float ie = EaseInOutCubic(it);
                    float ang = (float)i / BURST_CONES * 2.0f * IM_PI + sinf(seed) * 0.085f;
                    float dist = ball_r * 0.5f + ie * ring_r * (1.00f + 0.38f * fabsf(cosf(seed)));
                    float px = cnt.x + cosf(ang) * dist;
                    float py = cnt.y + sinf(ang) * dist;
                    float vx = cosf(ang), vy = sinf(ang), nx = -vy, ny = vx;
                    float ca = (1.0f - it) * global_alpha * (0.20f + 0.07f * (i % 3));
                    if (ca < 0.004f) continue;
                    float len = (6.2f + 5.0f * ie + (i % 4) * 1.2f) * us;
                    float wid = (2.1f + (i % 3) * 0.34f) * us;
                    ImVec2 tip(px + vx * len * 0.55f, py + vy * len * 0.55f);
                    ImVec2 back(px - vx * len * 0.45f, py - vy * len * 0.45f);
                    dl->AddTriangleFilled(tip,
                        ImVec2(back.x + nx * wid, back.y + ny * wid),
                        ImVec2(back.x - nx * wid, back.y - ny * wid),
                        IM_COL32(180, 214, 235, (int)(ca * 160.0f)));
                    dl->AddLine(ImVec2(px - vx * len * 1.75f, py - vy * len * 1.75f), ImVec2(px - vx * len * 0.4f, py - vy * len * 0.4f),
                        IM_COL32(88, 158, 220, (int)(ca * 76.0f)), 0.95f * us);
                }
            }
        }
    }
    {
        const float T_REVERSE_BEG = T_BURST_END;
        const float T_REVERSE_END = T_BURST_END + 0.92f;
        if (elapsed >= T_REVERSE_BEG && elapsed < T_REVERSE_END) {
            float lt = (elapsed - T_REVERSE_BEG) / (T_REVERSE_END - T_REVERSE_BEG);
            if (lt > 1.0f) lt = 1.0f;
            const int RWAVES = 3;
            for (int w = 0; w < RWAVES; ++w) {
                float w_delay = w * 0.14f;
                float w_dur = 1.0f - w_delay;
                float w_lt = (lt - w_delay) / w_dur;
                if (w_lt < 0.0f) w_lt = 0.0f;
                if (w_lt > 1.0f) w_lt = 1.0f;
                float we = EaseOutCubic(w_lt);
                float rr = diag * 1.05f + (ball_r - diag * 1.05f) * we;
                float peak_a = 1.0f - w * 0.10f;
                float wa = peak_a;
                if (w_lt < 0.10f)        wa = peak_a * (w_lt / 0.10f);
                else if (w_lt > 0.70f)   wa = peak_a * (1.0f - (w_lt - 0.70f) / 0.30f);
                if (wa < 0.005f) continue;
                float wt = Lerp(9.0f, 1.25f, we) * us;
                int rb = 75 + w * 15;
                int rg = 145 + w * 18;
                int rr_col = 220 + w * 10;
                dl->AddCircle(cnt, rr, IM_COL32(rb, rg, rr_col, (int)(wa * 178.0f)), 0, wt);
                dl->AddCircle(cnt, rr, IM_COL32(50, 105, 185, (int)(wa * 48.0f)), 0, wt * 3.2f);
                const int TICKS = 20;
                for (int k = 0; k < TICKS; ++k) {
                    float phase = (float)k / (float)TICKS;
                    float blink = 0.74f + 0.26f * fabsf(sinf(elapsed * 10.0f + k * 1.31f + w));
                    if (((k + w) % 4) == 0) blink *= 0.62f;
                    float ta = wa * blink * (0.13f + 0.035f * w);
                    if (ta < 0.004f) continue;
                    float a0 = phase * 2.0f * IM_PI - lt * (1.05f + w * 0.22f) + sinf(k * 0.93f) * 0.032f;
                    float span = (0.016f + 0.007f * ((k + w) % 3)) * IM_PI;
                    ImVec2 p0(cnt.x + cosf(a0) * rr, cnt.y + sinf(a0) * rr);
                    ImVec2 p1(cnt.x + cosf(a0 + span) * rr, cnt.y + sinf(a0 + span) * rr);
                    dl->AddLine(p0, p1, IM_COL32(105, 170, 220, (int)(ta * 255.0f)), 0.82f * us);
                }
                const int CONES = 6;
                for (int c = 0; c < CONES; ++c) {
                    float seed = c * 2.417f + w * 0.83f;
                    float clt = w_lt - c * 0.012f;
                    if (clt < 0.0f || clt > 1.0f) continue;
                    float ce = EaseOutCubic(clt);
                    float cr = diag * (0.92f + 0.06f * sinf(seed)) + (ball_r * (1.12f + 0.08f * w) - diag * (0.92f + 0.06f * sinf(seed))) * ce;
                    float ang = seed + (float)c / (float)CONES * 2.0f * IM_PI - lt * (2.15f + 0.24f * w);
                    float px = cnt.x + cosf(ang) * cr;
                    float py = cnt.y + sinf(ang) * cr;
                    float ca = wa * (1.0f - clt * 0.48f) * 0.28f;
                    if (ca < 0.004f) continue;
                    float vx = -cosf(ang), vy = -sinf(ang); 
                    float nx = -vy, ny = vx;
                    float len = (6.5f + 2.2f * ((c + w) % 3)) * us;
                    float wid = (2.25f + 0.35f * w) * us;
                    for (int tr = 0; tr < 2; ++tr) {
                        float off0 = len * (0.95f + tr * 0.70f);
                        float off1 = len * (0.22f + tr * 0.70f);
                        float tra = ca * (0.28f - tr * 0.075f);
                        dl->AddLine(ImVec2(px - vx * off0, py - vy * off0), ImVec2(px - vx * off1, py - vy * off1),
                            IM_COL32(75, 140, 205, (int)(tra * 255.0f)), (1.0f - tr * 0.18f) * us);
                    }
                    ImVec2 tip(px + vx * len, py + vy * len);
                    ImVec2 back(px - vx * len * 0.45f, py - vy * len * 0.45f);
                    dl->AddTriangleFilled(tip,
                        ImVec2(back.x + nx * wid, back.y + ny * wid),
                        ImVec2(back.x - nx * wid, back.y - ny * wid),
                        IM_COL32(96, 165, 220, (int)(ca * 168.0f)));
                    dl->AddCircleFilled(ImVec2(px, py), 1.0f * us, IM_COL32(140, 200, 230, (int)(ca * 90.0f)), 6);
                }
            }
        }
    }
    {
        const float T_END_IRIS_BEG = T_FADE_BEG + 0.18f;
        const float T_END_IRIS_END = T_FADE_END;
        if (elapsed >= T_END_IRIS_BEG && elapsed < T_END_IRIS_END) {
            float it = (elapsed - T_END_IRIS_BEG) / (T_END_IRIS_END - T_END_IRIS_BEG);
            if (it < 0.0f) it = 0.0f; if (it > 1.0f) it = 1.0f;
            float ie = EaseInOutCubic(it);
            float alpha_in = it / 0.16f;
            if (alpha_in > 1.0f) alpha_in = 1.0f;
            float iris_a = EaseOutCubic(alpha_in) * 0.94f;
            float inner_r = Lerp(diag * 0.80f, 0.0f, ie);
            float outer_r = diag * 0.94f;
            float thickness = outer_r - inner_r;
            if (thickness < 1.0f) thickness = 1.0f;
            float mid_r = inner_r + thickness * 0.5f;
            dl->AddCircle(cnt, mid_r, IM_COL32(0, 0, 0, (int)(iris_a * 255.0f)), 224, thickness);
            const int END_VORTEX = 34;
            float close_boost = EaseInOutCubic(it);
            for (int i = END_VORTEX; i >= 0; --i) {
                float k = (float)i / (float)END_VORTEX;
                float inv = 1.0f - k;
                float soft = inv * inv * (3.0f - 2.0f * inv);
                float rr = Lerp(inner_r * 0.22f + ball_r * 0.55f, inner_r * 0.96f + ring_r * 0.55f, k);
                if (rr < ball_r * 0.55f) rr = ball_r * 0.55f;
                float aa = iris_a * (0.012f + soft * (0.070f + close_boost * 0.070f));
                if (aa > 0.001f)
                    dl->AddCircleFilled(cnt, rr, IM_COL32(0, 0, 0, (int)(aa * 255.0f)), 160);
            }
            float guide_gate = 1.0f - EaseInOutCubic(it);
            if (guide_gate > 0.015f) {
                float pulse = 0.5f + 0.5f * sinf(elapsed * 7.0f);
                float core_r = ball_r * (1.00f + pulse * 0.055f);
                dl->AddCircle(cnt, core_r, IM_COL32(115, 185, 235, (int)(guide_gate * 135.0f)), 96, 1.25f * us);
                dl->AddCircle(cnt, core_r * 1.72f, IM_COL32(55, 115, 185, (int)(guide_gate * 58.0f)), 128, 0.9f * us);
                dl->AddCircleFilled(cnt, core_r * 0.34f, IM_COL32(10, 18, 26, (int)(guide_gate * 105.0f)), 48);
            }
            const int ARMS = 18;
            const int SEG = 18;
            for (int a = 0; a < ARMS; ++a) {
                float seed = a * 1.731f + 0.41f;
                float arm_delay = (a % 6) * 0.018f;
                float at = (it - arm_delay) / (1.0f - arm_delay);
                if (at < 0.0f || at > 1.0f) continue;
                float ae = EaseInOutCubic(at);
                float fade = sinf(at * IM_PI);
                float line_a = fade * guide_gate * (0.30f + 0.18f * fabsf(sinf(seed)));
                if (line_a < 0.006f) continue;
                ImVec2 pts[SEG];
                int pc = 0;
                float start_r = Lerp(diag * 0.46f, ring_r * 1.22f, ae);
                float end_r = Lerp(ring_r * 1.65f, ball_r * 1.12f, ae);
                float base_ang = (float)a / (float)ARMS * 2.0f * IM_PI + elapsed * (0.38f + 0.03f * (a % 4));
                float curl = Lerp(1.15f, 0.28f, ae);
                for (int s = 0; s < SEG; ++s) {
                    float q = (float)s / (float)(SEG - 1);
                    float qe = q * q * (3.0f - 2.0f * q);
                    float rr = Lerp(start_r, end_r, qe);
                    float ang = base_ang + curl * qe + 0.05f * sinf(elapsed * 5.2f + seed + q * 4.0f);
                    pts[pc++] = ImVec2(cnt.x + cosf(ang) * rr, cnt.y + sinf(ang) * rr);
                }
                dl->AddPolyline(pts, pc, IM_COL32(70, 155, 225, (int)(line_a * 255.0f)), ImDrawFlags_None, (0.85f + 0.35f * (a % 3)) * us);
                ImVec2 hp = pts[pc - 1];
                dl->AddCircleFilled(hp, (1.3f + 0.35f * (a % 3)) * us, IM_COL32(150, 220, 245, (int)(line_a * 210.0f)), 8);
            }
            if (it > 0.78f) {
                float core_t = (it - 0.78f) / 0.22f;
                if (core_t > 1.0f) core_t = 1.0f;
                float core_a = EaseInOutCubic(core_t) * iris_a;
                dl->AddCircleFilled(cnt, diag * 0.12f * core_t, IM_COL32(0, 0, 0, (int)(core_a * 255.0f)), 128);
            }
        }
    }
    dl->PopClipRect();
}
InjectionAnim g_injection_anim;