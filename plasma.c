#include "raylib.h"
#include <math.h>

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
    const float inv_totalFrames = 1.0f / totalFrames;
    const float two_pi = 2.0f * M_PI;

    unsigned char *pixels = (unsigned char *)img.data;

    while (!WindowShouldClose()) {
        // Calculate time based (looping animation)
        float t = fmodf(frameCount, totalFrames) * inv_totalFrames * two_pi;

        // Precompute constants outside loops
        float r_x = (float)w;
        float r_y = (float)h;
        float inv_r_y = 1.0f / r_y;

        // Calculate plasma effect for each pixel - optimized inner loop
        for (int y = 0; y < h; ++y) {
            float fy = (float)y;
            float py_base = (fy * 2.0f - r_y) * inv_r_y;
            
            for (int x = 0; x < w; ++x) {
                float fx = (float)x;
                
                // p = (FC * 2.0 - r) / r.y
                float px = (fx * 2.0f - r_x) * inv_r_y;
                float py = py_base;
                
                // l += 4.0 - 4.0 * abs(0.7 - dot(p, p))
                float dot_pp = px * px + py * py;
                float l_val = 4.0f - 4.0f * fabsf(0.7f - dot_pp);
                
                // v = p * l
                float vx = px * l_val;
                float vy = py * l_val;
                
                // Initialize output
                float ox = 0.0f, oy = 0.0f, oz = 0.0f, ow = 0.0f;
                
                // Inner loop - matches original: i.y goes from 1 to 8 (8 iterations), i.x stays 0
                // Original: for (; i.y++ < 8.0f;) - post-increment means i.y is 1-8 inside loop
                // Original: v += cos(v.yx() * i.y + i + t) / i.y + 0.7
                // Where i = {0, i.y}, so: cos((vy * i.y + 0, vx * i.y + i.y) + t)
                for (int i = 1; i <= 8; ++i) {
                    float fi = (float)i;
                    float inv_i = 1.0f / fi;
                    
                    // v.yx() swaps components: (vy, vx)
                    // v.yx() * i.y = (vy * i.y, vx * i.y)
                    // + i = (vy * i.y + 0, vx * i.y + i.y)
                    // + t = (vy * i.y + t, vx * i.y + i.y + t)
                    float cos_arg_x = vy * fi + t;
                    float cos_arg_y = vx * fi + fi + t;
                    float cos_x = cosf(cos_arg_x);
                    float cos_y = cosf(cos_arg_y);
                    vx += cos_x * inv_i + 0.7f;
                    vy += cos_y * inv_i + 0.7f;
                    
                    // o += (sin(v.xyyx()) + 1.0) * abs(v.x - v.y)
                    // v.xyyx() = (vx, vy, vy, vx), so compute sin once per component
                    float v_diff = fabsf(vx - vy);
                    float sin_vx = sinf(vx);  // Compute once, reuse for x and w components
                    float sin_vy = sinf(vy);  // Compute once, reuse for y and z components
                    
                    ox += (sin_vx + 1.0f) * v_diff;
                    oy += (sin_vy + 1.0f) * v_diff;
                    oz += (sin_vy + 1.0f) * v_diff;
                    ow += (sin_vx + 1.0f) * v_diff;
                }
                
                // o = tanh(5.0 * exp(l.x - 4.0 - p.y * vec4(-1, 1, 2, 0)) / o)
                float exp_base = l_val - 4.0f;
                float exp_x = expf(exp_base + py);
                float exp_y = expf(exp_base - py);
                float exp_z = expf(exp_base - py * 2.0f);
                float exp_w = expf(exp_base);
                
                // Avoid division by zero
                float inv_ox = (ox != 0.0f) ? 1.0f / ox : 1.0f;
                float inv_oy = (oy != 0.0f) ? 1.0f / oy : 1.0f;
                float inv_oz = (oz != 0.0f) ? 1.0f / oz : 1.0f;
                float inv_ow = (ow != 0.0f) ? 1.0f / ow : 1.0f;
                
                ox = tanhf(5.0f * exp_x * inv_ox);
                oy = tanhf(5.0f * exp_y * inv_oy);
                oz = tanhf(5.0f * exp_z * inv_oz);
                ow = tanhf(5.0f * exp_w * inv_ow);
                
                // Clamp and convert to byte - optimized clamping
                int idx = (y * w + x) * 4;
                // Fast clamp: if val < 0 use 0, else if val > 1 use 1, else use val
                float r_val = (ox < 0.0f) ? 0.0f : ((ox > 1.0f) ? 1.0f : ox);
                float g_val = (oy < 0.0f) ? 0.0f : ((oy > 1.0f) ? 1.0f : oy);
                float b_val = (oz < 0.0f) ? 0.0f : ((oz > 1.0f) ? 1.0f : oz);
                pixels[idx + 0] = (unsigned char)(r_val * 255.0f); // R
                pixels[idx + 1] = (unsigned char)(g_val * 255.0f); // G
                pixels[idx + 2] = (unsigned char)(b_val * 255.0f); // B
                pixels[idx + 3] = 255; // A
            }
        }

        // Update texture with new pixel data
        UpdateTexture(texture, pixels);

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
