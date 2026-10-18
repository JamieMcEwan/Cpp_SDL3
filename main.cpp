#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

const int CELL_SIZE = 36;
const int CHAR_WIDTH = 5;
const int CHAR_HEIGHT = 7;
const int LOGICAL_W = 3840;
const int LOGICAL_H = 2160;

// High-fidelity 5x7 ASCII bitmap font grid (0x21 to 0x7E)
const uint8_t FONT_BITMAP[][CHAR_HEIGHT] = {
    {0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04}, // !
    {0x0A, 0x0A, 0x14, 0x00, 0x00, 0x00, 0x00}, // "
    {0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A}, // #
    {0x04, 0x0F, 0x14, 0x0E, 0x05, 0x1E, 0x04}, // $
    {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03}, // %
    {0x0C, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0D}, // &
    {0x0C, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00}, // '
    {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02}, // (
    {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08}, // )
    {0x00, 0x04, 0x15, 0x0E, 0x15, 0x04, 0x00}, // *
    {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}, // +
    {0x00, 0x00, 0x00, 0x00, 0x0C, 0x04, 0x08}, // ,
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}, // -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C}, // .
    {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00}, // /
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, // 2
    {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}, // 5
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}, // 9
    {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00}, // :
    {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x04, 0x08}, // ;
    {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02}, // <
    {0x00, 0x1F, 0x00, 0x1F, 0x00, 0x00, 0x00}, // =
    {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08}, // >
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}, // ?
    {0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0F}, // @
    {0x04, 0x0A, 0x11, 0x11, 0x1F, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}, // B
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}, // C
    {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}, // D
    {0x1F, 0x10, 0x10, 0x1C, 0x10, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x10, 0x1C, 0x10, 0x10, 0x10}, // F
    {0x0F, 0x10, 0x10, 0x17, 0x11, 0x11, 0x0F}, // G
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // H
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}, // I
    {0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E}, // J
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, // K
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, // L
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}, // M
    {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}, // P
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}, // Q
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}, // R
    {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}, // S
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, // T
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // U
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}, // V
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}, // W
    {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}, // X
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}, // Y
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}  // Z
};

enum ColorMode { M_GREEN, M_AMBER, M_PURPLE, M_BLUE, M_WHITE, M_RED, M_COLORS };
enum SpeedMode { S_NORMAL, S_HYPER, S_SLOW, S_FROZEN, M_SPEEDS };
enum BgMode    { B_BLACK, B_BLUE, B_GRAY, B_MAROON, B_GREEN, M_BGS };

struct CodeColor  { uint8_t r, g, b; std::string label; };
struct SpeedLevel { float mult; std::string label; };
struct BgColor { 
    uint8_t r1, g1, b1; // Top Horizon Colors
    uint8_t r2, g2, b2; // Bottom Ground Colors
    std::string label; 
};

const CodeColor CODES[M_COLORS] = {
    {0, 240, 0, "MATRIX GREEN"}, {240, 150, 0, "AMBER TERMINAL"},
    {185, 30, 240, "CYBER PURPLE"}, {0, 160, 245, "SUBZERO BLUE"},
    {220, 220, 220, "STEALTH WHITE"}, {245, 10, 10, "CRIMSON RED"}
};

const SpeedLevel SPEEDS[M_SPEEDS] = {
    {1.00f, "NORMAL FLOW"}, {2.50f, "HYPER DRIVE"},
    {0.25f, "SLOW MOTION"}, {0.00f, "TIME FROZEN"}
};

const BgColor BGS[M_BGS] = {
    {0, 0, 0, 0, 0, 0, "SOLID BLACK"},
    {10, 28, 64, 2, 4, 16, "GRADIENT CYBER BLUE"},
    {32, 32, 38, 6, 6, 8,   "GRADIENT CHARCOAL GRAY"},
    {56, 8, 20, 8, 2, 4,    "GRADIENT MIDNIGHT MAROON"},
    {12, 48, 16, 2, 6, 3,   "GRADIENT FOREST GREEN"}
};

struct RainColumn { float y; float speed; char glyph; float tick; int interval; };

std::random_device rd;
std::mt19937 gen(rd());

char get_random_char() {
    std::uniform_int_distribution<> dis(33, 90); 
    return static_cast<char>(dis(gen));
}
void draw_glyph(SDL_Renderer* ren, char c, int x, int y, float w, float h, uint8_t r, uint8_t g, uint8_t b) {
    if (c < 33 || c > 90) return;
    int idx = c - 33;
    SDL_SetRenderDrawColor(ren, r, g, b, 255);
    float scale_x = w / (CHAR_WIDTH + 2);
    float scale_y = h / (CHAR_HEIGHT + 2);
    for (int row = 0; row < CHAR_HEIGHT; ++row) {
        uint8_t row_bits = FONT_BITMAP[idx][row];
        for (int col = 0; col < CHAR_WIDTH; ++col) {
            if (row_bits & (1 << (CHAR_WIDTH - 1 - col))) {
                SDL_FRect pixel = { x + (col + 1) * scale_x, y + (row + 1) * scale_y, scale_x, scale_y };
                SDL_RenderFillRect(ren, &pixel);
            }
        }
    }
}

void render_string(SDL_Renderer* ren, const std::string& str, int x, int y, float scale, uint8_t r, uint8_t g, uint8_t b) {
    int cx = x;
    float gw = (CHAR_WIDTH + 2) * scale;
    float gh = (CHAR_HEIGHT + 2) * scale;
    for (char c : str) {
        char target = (c >= 'a' && c <= 'z') ? static_cast<char>(c - 32) : c;
        if (target == ' ') { cx += static_cast<int>(gw); continue; }
        draw_glyph(ren, target, cx, y, gw, gh, r, g, b);
        cx += static_cast<int>(gw);
    }
}

// Renders the background gradient directly into any target buffer canvas
void draw_background_gradient(SDL_Renderer* ren, const BgColor& bg) {
    const int steps = 64;
    int band_h = LOGICAL_H / steps;
    for (int i = 0; i < steps; ++i) {
        float ratio = static_cast<float>(i) / steps;
        uint8_t r = static_cast<uint8_t>(bg.r1 + (bg.r2 - bg.r1) * ratio);
        uint8_t g = static_cast<uint8_t>(bg.g1 + (bg.g2 - bg.g1) * ratio);
        uint8_t b = static_cast<uint8_t>(bg.b1 + (bg.b2 - bg.b1) * ratio);
        SDL_SetRenderDrawColor(ren, r, g, b, 255);
        SDL_FRect band = { 0.0f, static_cast<float>(i * band_h), static_cast<float>(LOGICAL_W), static_cast<float>(band_h) };
        SDL_RenderFillRect(ren, &band);
    }
}

// Alpha overlay pass replicates the organic trail decay beautifully over gradients
void apply_trail_fade_overlay(SDL_Renderer* ren, const BgColor& bg) {
    const int steps = 32;
    int band_h = LOGICAL_H / steps;
    for (int i = 0; i < steps; ++i) {
        float ratio = static_cast<float>(i) / steps;
        uint8_t r = static_cast<uint8_t>(bg.r1 + (bg.r2 - bg.r1) * ratio);
        uint8_t g = static_cast<uint8_t>(bg.g1 + (bg.g2 - bg.g1) * ratio);
        uint8_t b = static_cast<uint8_t>(bg.b1 + (bg.b2 - bg.b1) * ratio);
        
        // Alpha = 13 (approx 5% opacity) gives the perfect cinematic trail longevity
        SDL_SetRenderDrawColor(ren, r, g, b, 13);
        SDL_FRect band = { 0.0f, static_cast<float>(i * band_h), static_cast<float>(LOGICAL_W), static_cast<float>(band_h) };
        SDL_RenderFillRect(ren, &band);
    }
}

SDL_Texture* compile_scanlines(SDL_Renderer* ren) {
    SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LOGICAL_W, LOGICAL_H);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(ren, tex);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    SDL_RenderClear(ren);
    for (int y = 0; y < LOGICAL_H; y += 12) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 205);
        for (int i = 0; i < 5; ++i) SDL_RenderLine(ren, 0, y + i, LOGICAL_W, y + i);
        SDL_SetRenderDrawColor(ren, 0, 45, 0, 85);
        SDL_RenderLine(ren, 0, y - 1, LOGICAL_W, y - 1);
        SDL_RenderLine(ren, 0, y + 5, LOGICAL_W, y + 5);
    }
    for (int i = 0; i < 120; i += 4) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, static_cast<uint8_t>(115 - (i * 0.95f)));
        SDL_FRect bound = { (float)i, (float)i, (float)(LOGICAL_W - (i * 2)), (float)(LOGICAL_H - (i * 2)) };
        SDL_RenderRect(ren, &bound);
    }
    SDL_SetRenderTarget(ren, nullptr);
    return tex;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return -1;
    SDL_Window* win = SDL_CreateWindow("Matrix Engine 4K", LOGICAL_W, LOGICAL_H, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, nullptr);
    SDL_SetRenderVSync(ren, 1);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    SDL_Texture* scene = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LOGICAL_W, LOGICAL_H);
    SDL_Texture* scan = compile_scanlines(ren);
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);

    bool filter = true;
    ColorMode c_sel = M_GREEN; SpeedMode s_sel = S_NORMAL; BgMode b_sel = B_BLACK;
    int cols_cnt = LOGICAL_W / CELL_SIZE;
    std::vector<RainColumn> items(cols_cnt);
    std::uniform_real_distribution<float> p_dist(-LOGICAL_H * 1.5f, 0);
    std::uniform_real_distribution<float> s_dist(8.0f, 24.0f);
    std::uniform_int_distribution<> i_dist(4, 15);

    for (int i = 0; i < cols_cnt; ++i) {
        items[i] = { p_dist(gen), s_dist(gen), get_random_char(), 0.0f, i_dist(gen) };
    }

    bool running = true; SDL_Event ev;
    uint64_t metric_time = SDL_GetTicks(); uint32_t metric_frames = 0; int frame_rate = 0;
    bool reset_background = true;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) running = false;
                if (ev.key.key == SDLK_F5) filter = !filter;
                if (ev.key.key == SDLK_F1) c_sel = static_cast<ColorMode>((c_sel + 1) % M_COLORS);
                if (ev.key.key == SDLK_F2) s_sel = static_cast<SpeedMode>((s_sel + 1) % M_SPEEDS);
                if (ev.key.key == SDLK_F3) {
                    b_sel = static_cast<BgMode>((b_sel + 1) % M_BGS);
                    reset_background = true; // Flushes previous canvas history instantly on switch
                }
            }
        }

        metric_frames++;
        uint64_t curr_time = SDL_GetTicks();
        if (curr_time - metric_time >= 500) {
            frame_rate = static_cast<int>((metric_frames * 1000) / (curr_time - metric_time));
            metric_frames = 0; metric_time = curr_time;
        }

        SDL_SetRenderTarget(ren, scene);
        BgColor bg = BGS[b_sel];
        
        if (reset_background) {
            draw_background_gradient(ren, bg);
            reset_background = false;
        }

        // Blends a highly transparent layer of the gradient to leave realistic code trails
        if (s_sel != S_FROZEN) {
            apply_trail_fade_overlay(ren, bg);
        }

        CodeColor code = CODES[c_sel]; float sm = SPEEDS[s_sel].mult;

        for (int i = 0; i < cols_cnt; ++i) {
            items[i].y += (items[i].speed * sm);
            items[i].tick += sm;
            if (items[i].tick >= items[i].interval) {
                items[i].glyph = get_random_char(); items[i].tick = 0.0f;
            }
            int dx = i * CELL_SIZE; int dy = static_cast<int>(items[i].y);
            draw_glyph(ren, items[i].glyph, dx, dy, CELL_SIZE, CELL_SIZE, std::min(255, code.r + 80), std::min(255, code.g + 80), std::min(255, code.b + 80));

            for (int t = 1; t <= 15; ++t) {
                int ty = dy - (CELL_SIZE * t); if (ty < -CELL_SIZE) continue;
                float f = 1.0f - (static_cast<float>(t) / 15.0f);
                uint8_t r = static_cast<uint8_t>(code.r * f), g = static_cast<uint8_t>(code.g * f), b = static_cast<uint8_t>(code.b * f);
                if (t <= 2) draw_glyph(ren, get_random_char(), dx, ty, CELL_SIZE, CELL_SIZE, std::min(255, r + 20), std::min(255, g + 20), std::min(255, b + 20));
                else draw_glyph(ren, get_random_char(), dx, ty, CELL_SIZE, CELL_SIZE, r, g, b);
            }
            if (items[i].y > LOGICAL_H + (CELL_SIZE * 15)) {
                items[i].y = -CELL_SIZE * 15; items[i].speed = s_dist(gen);
            }
        }

        render_string(ren, "CODE COLOR: " + code.label, 50, 50, 4.0f, 255, 255, 255);
        render_string(ren, "SPEED MODE: " + SPEEDS[s_sel].label, 50, 120, 4.0f, 255, 255, 255);
        render_string(ren, "SCREEN BG:  " + bg.label, 50, 190, 4.0f, 255, 255, 255);
        render_string(ren, filter ? "SCANLINE:   ACTIVE" : "SCANLINE:   OFF", 50, 260, 2.5f, 140, 140, 140);
        render_string(ren, "HARDWARE STATE: " + std::to_string(frame_rate) + " FPS", 50, 310, 2.5f, 0, 235, 255);

        // Render target flush out to main display window
        SDL_SetRenderTarget(ren, nullptr);
        SDL_SetRenderDrawColor(ren, bg.r2, bg.g2, bg.b2, 255);
        SDL_RenderClear(ren);
        
        SDL_RenderTexture(ren, scene, nullptr, nullptr);
        if (filter && scan) SDL_RenderTexture(ren, scan, nullptr, nullptr);
        SDL_RenderPresent(ren);
    }
    SDL_DestroyTexture(scene); SDL_DestroyTexture(scan);
    SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
    return 0;
}
