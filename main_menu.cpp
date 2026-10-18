#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // For rand()
#include <cmath>   // For sin(), cos(), and atan2()

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Model representing a text item linked to an active background color template
struct ColorItem {
    std::string name;
    SDL_Color color;
};

// Firework particle structure with individual layout specs
struct FireworkParticle {
    float x, y;
    float vx, vy;
    float lifetime;
    float max_life;
    float size;
    SDL_Color color;
};

// Ambient structural backdrop star model
struct Star {
    float x, y;
    float size;
    float twinkle_speed;
    float phase; 
};

// Kinetic line-rendered shooting star model
struct ShootingStar {
    float x, y;
    float vx, vy;
    float lifetime;
    float max_life;
    float length;
};

// NEW: Model representing a star within the rotating galaxy cluster
struct GalaxyStar {
    float distance;     // Radial distance from center core
    float base_angle;   // Initial orbital angle position
    float size;         // Visual pixel thickness
    SDL_Color color;    // Cosmic core tint variation
};
int main(int argc, char* argv[]) {
    // 1. Initialize SDL3 Video Contexts
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Setup Font Extensions
    if (!TTF_Init()) {
        std::cerr << "TTF Initialization failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 3. Create Hardware-Accelerated Output Windows
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("SDL3 Cosmic System", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        std::cerr << "Window/Renderer creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // 4. Load standard font layout asset
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24.0f);
    if (!font) {
        std::cerr << "Font loading failed: " << SDL_GetError() << std::endl;
    }

    TTF_TextEngine* text_engine = nullptr;
    if (font) {
        text_engine = TTF_CreateRendererTextEngine(renderer);
    }

    // Comprehensive themed palette list
    std::vector<ColorItem> palette = {
        {"Dark Charcoal",       {20, 20, 20, 255}},
        {"Dark Blue Night Sky", {10, 20, 50, 255}},  
        {"Crimson Red",         {220, 53, 69, 255}},
        {"Emerald Green",       {40, 167, 69, 255}},
        {"Electric Blue",       {0, 123, 255, 255}},
        {"Amber Yellow",        {255, 193, 7, 255}},  
        {"Deep Purple",         {111, 66, 193, 255}},
        {"Hot Pink",            {232, 62, 140, 255}},
        {"Teal Cyan",           {32, 201, 151, 255}}
    };

    // Pre-allocate memory handles for structural text formatting objects
    std::vector<TTF_Text*> menu_text_objects;
    if (font && text_engine) {
        for (const auto& item : palette) {
            TTF_Text* text_obj = TTF_CreateText(text_engine, font, item.name.c_str(), 0);
            menu_text_objects.push_back(text_obj);
        }
    }

    size_t current_index = 0;
    bool running = true;
    SDL_Event event;

    // Allocate runtime memory tables for ambient assets
    std::vector<FireworkParticle> firework_particles;
    std::vector<Star> night_sky_stars;
    std::vector<ShootingStar> shooting_stars;
    std::vector<GalaxyStar> galaxy_stars; // NEW: Galaxy particle collection

    // Seed 60 twinkling space stars uniformly across coordinate regions
    for (int i = 0; i < 60; ++i) {
        Star s;
        s.x = static_cast<float>(rand() % WINDOW_WIDTH);
        s.y = static_cast<float>(rand() % WINDOW_HEIGHT);
        s.size = 1.0f + (rand() % 2);
        s.twinkle_speed = 1.5f + (static_cast<float>(rand() % 35) / 10.0f);
        s.phase = static_cast<float>(rand() % 100);
        night_sky_stars.push_back(s);
    }

    // NEW: Seed 150 spiral galaxy cluster particles centered on the right half of the sky
    float galaxy_center_x = 600.0f;
    float galaxy_center_y = 250.0f;
    for (int i = 0; i < 150; ++i) {
        GalaxyStar gs;
        // Distribute distances with a bias toward the bright central core
        float r_factor = static_cast<float>(rand() % 100) / 100.0f;
        gs.distance = r_factor * r_factor * 95.0f + 5.0f; 
        
        // Use a logarithmic formula to tightly wrap particles into two distinct spiral arms
        int arm = rand() % 2;
        float arm_offset = arm * 3.14159f;
        float spiral_bend = 3.0f; 
        gs.base_angle = (gs.distance * (spiral_bend / 100.0f)) + arm_offset;
        
        // Add a random offset so the arms look naturally scattered rather than like perfect lines
        gs.base_angle += ((rand() % 100) - 50) / 130.0f;
        gs.size = 1.0f + (rand() % 2);

        // Core points shine hot white/purple, while outer edges turn into deep cosmic pinks
        if (gs.distance < 30.0f) {
            gs.color = {230, 240, 255, 255}; // Bright core
        } else if (rand() % 2 == 0) {
            gs.color = {210, 130, 255, 255}; // Cosmic Purple arms
        } else {
            gs.color = {255, 110, 180, 255}; // Nebula Pink arms
        }
        galaxy_stars.push_back(gs);
    }

    float firework_spawn_timer = 0.0f;
    float shooting_star_timer = 0.0f;
    float galaxy_rotation_angle = 0.0f; // NEW: Keeps track of galaxy spin state
    Uint64 last_time = SDL_GetTicks();
    // 5. Main Runtime Framework Execution Loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_F1) {
                    current_index = (current_index + 1) % palette.size();
                    // Flush vector caches instantly during state shifts
                    firework_particles.clear();
                    shooting_stars.clear();
                }
                else if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            }
        }

        // Calculate delta timing boundaries
        Uint64 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        if (delta_time > 0.1f) delta_time = 0.1f;

        // --- PIPELINE CALCULATIONS & PHYSICS ---
        std::string current_name = palette[current_index].name;

        if (current_name == "Amber Yellow") {
            firework_spawn_timer += delta_time;
            if (firework_spawn_timer >= 0.8f) {
                firework_spawn_timer = 0.0f;
                float blast_x = static_cast<float>((rand() % 300) + 420); 
                float blast_y = static_cast<float>((rand() % 250) + 150); 

                for (int i = 0; i < 250; ++i) {
                    FireworkParticle p;
                    p.x = blast_x; p.y = blast_y;
                    float angle = static_cast<float>(rand() % 360) * (3.14159f / 180.0f);
                    float velocity = static_cast<float>((rand() % 350) + 400); 
                    p.vx = std::cos(angle) * velocity; p.vy = std::sin(angle) * velocity;
                    p.max_life = 0.5f + (static_cast<float>(rand() % 70) / 100.0f);
                    p.lifetime = p.max_life; p.size = 2.0f + (rand() % 4);

                    int rand_color = rand() % 3;
                    if (rand_color == 0) p.color = {255, 255, 200, 255};
                    else if (rand_color == 1) p.color = {255, 200, 0, 255};
                    else p.color = {240, 85, 5, 255};
                    firework_particles.push_back(p);
                }
            }
        }

        if (current_name == "Dark Blue Night Sky") {
            // NEW: Rotate the spiral galaxy cluster smoothly over time (0.25 radians per second)
            galaxy_rotation_angle += 0.25f * delta_time;

            for (auto& s : night_sky_stars) {
                s.phase += s.twinkle_speed * delta_time;
            }
            shooting_star_timer += delta_time;
            if (shooting_star_timer >= 1.2f) {
                shooting_star_timer = 0.0f;
                if (rand() % 100 < 30) {
                    ShootingStar ss;
                    ss.x = static_cast<float>((rand() % 400) + 300); ss.y = static_cast<float>(rand() % 100);
                    ss.vx = -500.0f - (rand() % 300); ss.vy = 400.0f + (rand() % 200);
                    ss.max_life = 0.4f + (static_cast<float>(rand() % 30) / 100.0f);
                    ss.lifetime = ss.max_life; ss.length = 25.0f + (rand() % 25);
                    shooting_stars.push_back(ss);
                }
            }
        }

        // Firework lifetime processing
        for (auto it = firework_particles.begin(); it != firework_particles.end(); ) {
            it->lifetime -= delta_time;
            if (it->lifetime <= 0.0f) {
                it = firework_particles.erase(it); 
            } else {
                float friction = std::pow(0.92f, delta_time * 60.0f);
                it->vx *= friction; it->vy *= friction;
                it->vy += 140.0f * delta_time;
                it->x += it->vx * delta_time; it->y += it->vy * delta_time;
                ++it;
            }
        }

        // Shooting Star speed computations
        for (auto it = shooting_stars.begin(); it != shooting_stars.end(); ) {
            it->lifetime -= delta_time;
            if (it->lifetime <= 0.0f) {
                it = shooting_stars.erase(it);
            } else {
                it->x += it->vx * delta_time; it->y += it->vy * delta_time;
                ++it;
            }
        }

        // --- DRAW RENDERING OPERATIONS ---
        SDL_Color bg = palette[current_index].color;
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // 1. Draw Space elements if matching active backdrop index
        if (current_name == "Dark Blue Night Sky") {
            // Draw baseline static backdrop stars
            for (const auto& s : night_sky_stars) {
                float brightness = (std::sin(s.phase) * 0.5f) + 0.5f;
                Uint8 alpha = static_cast<Uint8>(100 + (brightness * 155));
                SDL_SetRenderDrawColor(renderer, 240, 245, 255, alpha);
                SDL_FRect star_rect = { s.x, s.y, s.size, s.size };
                SDL_RenderFillRect(renderer, &star_rect);
            }

            // NEW: Render the rotating spiral galaxy structure
            for (const auto& gs : galaxy_stars) {
                // Calculate position using current angular spin offsets
                float absolute_angle = gs.base_angle + galaxy_rotation_angle;
                float star_x = galaxy_center_x + (std::cos(absolute_angle) * gs.distance);
                float star_y = galaxy_center_y + (std::sin(absolute_angle) * gs.distance);
                
                // Fade out stars near the outer edges of the galaxy
                float edge_fade = 1.0f - (gs.distance / 100.0f);
                Uint8 alpha = static_cast<Uint8>(edge_fade * 230 + 25);

                SDL_SetRenderDrawColor(renderer, gs.color.r, gs.color.g, gs.color.b, alpha);
                SDL_FRect star_rect = { star_x, star_y, gs.size, gs.size };
                SDL_RenderFillRect(renderer, &star_rect);
            }

            // Draw active shooting stars
            for (const auto& ss : shooting_stars) {
                float pct = ss.lifetime / ss.max_life;
                Uint8 alpha = static_cast<Uint8>(pct * 255);
                SDL_SetRenderDrawColor(renderer, 220, 240, 255, alpha);
                float tail_x = ss.x - (ss.vx * 0.04f); float tail_y = ss.y - (ss.vy * 0.04f);
                SDL_RenderLine(renderer, ss.x, ss.y, tail_x, tail_y);
            }
        }

        // 2. Render kinetic firework layers
        for (const auto& p : firework_particles) {
            float life_percentage = p.lifetime / p.max_life;
            Uint8 alpha = static_cast<Uint8>(life_percentage * 255);
            SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, alpha);
            SDL_FRect p_rect = { p.x, p.y, p.size, p.size };
            SDL_RenderFillRect(renderer, &p_rect);
        }

        // 3. Render graphic text menu list overlays
        if (font && text_engine) {
            float start_x = 40.0f; float start_y = 50.0f; float line_spacing = 40.0f;

            for (size_t i = 0; i < palette.size(); ++i) {
                float current_line_y = start_y + (i * line_spacing);
                if (i == current_index) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
                    SDL_FRect marker = { start_x - 20.0f, current_line_y + 6.0f, 8.0f, 12.0f };
                    SDL_RenderFillRect(renderer, &marker);
                    TTF_SetTextColor(menu_text_objects[i], 255, 255, 255, 255);
                } else {
                    if (palette[current_index].name == "Amber Yellow") {
                        TTF_SetTextColor(menu_text_objects[i], 90, 75, 0, 220);
                    } else {
                        TTF_SetTextColor(menu_text_objects[i], 160, 160, 160, 200);
                    }
                }
                TTF_DrawRendererText(menu_text_objects[i], start_x, current_line_y);
            }
        }
        SDL_RenderPresent(renderer);
    }

    // 6. Complete Hardware Pipeline Memory Destruction
    if (!menu_text_objects.empty()) {
        for (auto* text_obj : menu_text_objects) {
            TTF_DestroyText(text_obj);
        }
    }
    if (text_engine) TTF_DestroyRendererTextEngine(text_engine);
    if (font) TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
