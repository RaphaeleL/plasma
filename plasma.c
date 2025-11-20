// IDEA: https://x.com/XorDev/status/1894123951401378051
// FIRST CPP IMPL: https://gist.githubusercontent.com/rexim/ef86bf70918034a5a57881456c0a0ccf/raw/d82ffbc6ccacb6a1fc3167b7f38c526f5e478103/plasma.cpp
// C VERSION: Converted from C++

#include "raylib.h"
#include <math.h>

typedef struct {
    float x, y, z, w;
} vec4;

typedef struct {
    float x, y;
} vec2;

// vec2 operations
vec2 vec2_mul_scalar(vec2 a, float s) { 
    vec2 result = {a.x * s, a.y * s};
    return result;
}

vec2 vec2_add_scalar(vec2 a, float s) { 
    vec2 result = {a.x + s, a.y + s};
    return result;
}

vec2 vec2_sub(vec2 a, vec2 b) { 
    vec2 result = {a.x - b.x, a.y - b.y};
    return result;
}

vec2 vec2_add(vec2 a, vec2 b) { 
    vec2 result = {a.x + b.x, a.y + b.y};
    return result;
}

vec2 vec2_mul(vec2 a, vec2 b) { 
    vec2 result = {a.x * b.x, a.y * b.y};
    return result;
}

vec2 vec2_div_scalar(vec2 a, float s) { 
    vec2 result = {a.x / s, a.y / s};
    return result;
}

float vec2_dot(vec2 a, vec2 b) { 
    return a.x * b.x + a.y * b.y;
}

vec2 vec2_abs(vec2 a) { 
    vec2 result = {fabsf(a.x), fabsf(a.y)};
    return result;
}

vec2 vec2_add_assign(vec2 *a, vec2 b) { 
    *a = vec2_add(*a, b);
    return *a;
}

vec2 vec2_add_scalar_assign(vec2 *a, float s) { 
    *a = vec2_add_scalar(*a, s);
    return *a;
}

vec2 vec2_cos(vec2 a) { 
    vec2 result = {cosf(a.x), cosf(a.y)};
    return result;
}

vec2 vec2_yx(vec2 a) { 
    vec2 result = {a.y, a.x};
    return result;
}

vec4 vec2_xyyx(vec2 a) { 
    vec4 result = {a.x, a.y, a.y, a.x};
    return result;
}

// vec4 operations
vec4 vec4_add_scalar(vec4 a, float s) { 
    vec4 result = {a.x + s, a.y + s, a.z + s, a.w + s};
    return result;
}

vec4 vec4_mul_scalar(vec4 a, float s) { 
    vec4 result = {a.x * s, a.y * s, a.z * s, a.w * s};
    return result;
}

vec4 vec4_add(vec4 a, vec4 b) { 
    vec4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return result;
}

vec4 vec4_sub(vec4 a, vec4 b) { 
    vec4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return result;
}

vec4 vec4_add_assign(vec4 *a, vec4 b) { 
    *a = vec4_add(*a, b);
    return *a;
}

vec4 vec4_sub_from_scalar(float s, vec4 a) { 
    vec4 result = {s - a.x, s - a.y, s - a.z, s - a.w};
    return result;
}

vec4 vec4_div(vec4 a, vec4 b) { 
    vec4 result = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
    return result;
}

vec4 vec4_sin(vec4 a) { 
    vec4 result = {sinf(a.x), sinf(a.y), sinf(a.z), sinf(a.w)};
    return result;
}

vec4 vec4_exp(vec4 a) { 
    vec4 result = {expf(a.x), expf(a.y), expf(a.z), expf(a.w)};
    return result;
}

vec4 vec4_tanh(vec4 a) { 
    vec4 result = {tanhf(a.x), tanhf(a.y), tanhf(a.z), tanhf(a.w)};
    return result;
}

int main()
{
    int w = 16*60;
    int h = 9*60;
    
    InitWindow(w, h, "Plasma in C");
    SetTargetFPS(60);
    
    // Create image buffer
    Image img = GenImageColor(w, h, BLACK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    
    // Create texture from image
    Texture2D texture = LoadTextureFromImage(img);
    
    float frameCount = 0.0f;
    const float totalFrames = 240.0f;
    
    while (!WindowShouldClose()) {
        // Calculate time based (looping animation)
        float t = (fmodf(frameCount, totalFrames) / totalFrames) * 2.0f * M_PI;
        
        // Calculate plasma effect for each pixel
        vec2 r = {(float)w, (float)h};
        unsigned char *pixels = (unsigned char *)img.data;
        
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                vec4 o = {0, 0, 0, 0};
                vec2 FC = {(float)x, (float)y};

                vec2 p = vec2_div_scalar(vec2_sub(vec2_mul_scalar(FC, 2.0f), r), r.y);
                vec2 l = {0, 0};
                vec2 i = {0, 0};
                
                // l += 4.0 - 4.0 * abs(0.7 - dot(p, p))
                float dot_pp = vec2_dot(p, p);
                float scalar_val = 4.0f - 4.0f * fabsf(0.7f - dot_pp);
                l.x += scalar_val;
                l.y += scalar_val;
                vec2 v = vec2_mul(p, l);
                
                for (; i.y++ < 8.0f;) {
                    // v += cos(v.yx() * i.y + i + t) / i.y + 0.7
                    vec2 v_yx = vec2_yx(v);
                    vec2 v_scaled = vec2_mul_scalar(v_yx, i.y);
                    vec2 v_add_i = vec2_add(v_scaled, i);
                    vec2 v_add_t = vec2_add_scalar(v_add_i, t);
                    vec2 cos_result = vec2_cos(v_add_t);
                    vec2 cos_div = vec2_div_scalar(cos_result, i.y);
                    vec2 cos_add = vec2_add_scalar(cos_div, 0.7f);
                    vec2_add_assign(&v, cos_add);
                    
                    // o += (sin(v.xyyx()) + 1.0) * abs(v.x - v.y)
                    vec4 v_xyyx = vec2_xyyx(v);
                    vec4 sin_result = vec4_sin(v_xyyx);
                    vec4 sin_add = vec4_add_scalar(sin_result, 1.0f);
                    vec4 sin_mul = vec4_mul_scalar(sin_add, fabsf(v.x - v.y));
                    vec4_add_assign(&o, sin_mul);
                }
                
                // o = tanh(5.0 * exp(l.x - 4.0 - p.y * vec4(-1, 1, 2, 0)) / o)
                vec4 p_y_vec = {p.y * -1.0f, p.y * 1.0f, p.y * 2.0f, p.y * 0.0f};
                vec4 exp_arg = {l.x - 4.0f, l.x - 4.0f, l.x - 4.0f, l.x - 4.0f};
                exp_arg = vec4_sub(exp_arg, p_y_vec);
                vec4 exp_result = vec4_exp(exp_arg);
                vec4 exp_mul = vec4_mul_scalar(exp_result, 5.0f);
                vec4 exp_div = vec4_div(exp_mul, o);
                o = vec4_tanh(exp_div);
                
                int idx = (y * w + x) * 4;
                pixels[idx + 0] = (unsigned char)(o.x * 255); // R
                pixels[idx + 1] = (unsigned char)(o.y * 255); // G
                pixels[idx + 2] = (unsigned char)(o.z * 255); // B
                pixels[idx + 3] = 255; // A
            }
        }
        
        // Update texture with new pixel data
        UpdateTexture(texture, img.data);
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
        
        frameCount += 1.0f;
    }
    
    UnloadTexture(texture);
    UnloadImage(img);
    CloseWindow();
    
    return 0;
}
