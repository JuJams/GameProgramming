/**
* Author: Sanjana Chowdary
* Assignment: Rise of the AI
* Date due: 2024-11-09, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 11
#define ENEMY_COUNT 3

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "SDL_mixer.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

// ––––– STRUCTS AND ENUMS ––––– //
struct GameState
{
    Entity *player;
    Entity *platforms;
    Entity *enemies;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
};
GLuint g_font_texture_id;
enum AppStatus { RUNNING, TERMINATED };

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 61.0f / 255.0f,
            BG_BLUE    = 59.0f / 255.0f,
            BG_GREEN   = 64.0f / 255.0f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char SPRITESHEET_FILEPATH[] = "assets/micheal.png",
           PLATFORM_FILEPATH[]    = "assets/platform.png",
           ENEMY_FILEPATH[]       = "assets/ghost.png";

constexpr char BACKGROUND_FILEPATH[] = "assets/forest-background.png";
GLuint g_background_texture_id;

constexpr char BGM_FILEPATH[] = "assets/bgm.mp3",
           SFX_FILEPATH[] = "assets/bounce.wav";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

constexpr float PLATFORM_OFFSET = 5.0f;

// ––––– VARIABLES ––––– //
GameState g_game_state;

SDL_Window* g_display_window;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

AppStatus g_app_status = RUNNING;

GLuint load_texture(const char* filepath);

void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return textureID;
}

void initialise()
{
    // ––––– GENERAL STUFF ––––– //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, AI!",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT,
                                  SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

    #ifdef _WINDOWS
    glewInit();
    #endif
    // ––––– VIDEO STUFF ––––– //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());



    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);
    if (g_background_texture_id == 0) {
        LOG("Failed to load background texture.");
    }

    // ––––– PLATFORM ––––– //
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);

    g_game_state.platforms = new Entity[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
    g_game_state.platforms[i] = Entity(platform_texture_id,0.0f, 0.4f, 1.0f, PLATFORM);
    g_game_state.platforms[i].set_position(glm::vec3(i - PLATFORM_OFFSET, -3.0f, 0.0f));
    g_game_state.platforms[i].update(0.0f, NULL, NULL, 0);
    }
    
    
    g_font_texture_id = load_texture("assets/font1.png");
    //GEORGE//
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
    { 1, 5, 9, 13 },  // for George to move to the left,
    { 3, 7, 11, 15 }, // for George to move to the right,
    { 2, 6, 10, 14 }, // for George to move upwards,
    { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);

    g_game_state.player = new Entity(
    player_texture_id,         // texture id
    5.0f,                      // speed
    acceleration,              // acceleration
    3.0f,                      // jumping power
    player_walking_animation,  // animation index sets
    0.0f,                      // animation time
    4,                         // animation frame amount
    0,                         // current animation index
    4,                         // animation column amount
    4,                         // animation row amount
    0.9f,                      // width
    0.9f,                       // height
    PLAYER
    );


    // Jumping
    g_game_state.player->set_jumping_power(3.0f);

    // ––––– SOPHIE ––––– //
    GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);

    g_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    g_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }
  
    g_game_state.enemies[0].set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    GLuint vertical_enemy_texture_id = load_texture(ENEMY_FILEPATH);
    
    Entity vertical_enemy = Entity(vertical_enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, VERTICAL_MOVER, IDLE);
    vertical_enemy.set_position(glm::vec3(-1.0f, -1.0f, 0.0f));
    vertical_enemy.set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    vertical_enemy.set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    g_game_state.enemies[1].set_acceleration(glm::vec3(0.0f)); // No gravity for vertical enemy
    
    g_game_state.enemies[ENEMY_COUNT - 2] = vertical_enemy;
    GLuint spinning_enemy_texture_id = load_texture(ENEMY_FILEPATH);
    Entity spinning_enemy = Entity(spinning_enemy_texture_id,1.0f,1.0f,1.0f,ENEMY,SPINNER,IDLE);
    
    spinning_enemy.set_position(glm::vec3(3.0f, 2.0f, 0.0f));
    spinning_enemy.set_movement(glm::vec3(0.0f));
    spinning_enemy.set_acceleration(glm::vec3(0.0f));
    g_game_state.enemies[ENEMY_COUNT-1] = spinning_enemy;
    // ––––– AUDIO STUFF ––––– //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);

    g_game_state.jump_sfx = Mix_LoadWAV(SFX_FILEPATH);

    // ––––– GENERAL STUFF ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
 g_game_state.player->set_movement(glm::vec3(0.0f));
 
 SDL_Event event;
 while (SDL_PollEvent(&event))
 {
     switch (event.type) {
         // End game
         case SDL_QUIT:
         case SDL_WINDOWEVENT_CLOSE:
             g_app_status = TERMINATED;
             break;
             
         case SDL_KEYDOWN:
             switch (event.key.keysym.sym) {
                 case SDLK_q:
                    
                     g_app_status = TERMINATED;
                     break;
                     
                 case SDLK_SPACE:
                 
                     
                    g_game_state.player->jump();
                    Mix_PlayChannel(-1, g_game_state.jump_sfx, 0);
                     
                     break;
                     
                 default:
                     break;
             }
             
         default:
             break;
     }
 }
 
 const Uint8 *key_state = SDL_GetKeyboardState(NULL);

 if (key_state[SDL_SCANCODE_LEFT])       g_game_state.player->move_left();
 else if (key_state[SDL_SCANCODE_RIGHT]) g_game_state.player->move_right();
     
 if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    g_game_state.player->normalise_movement();
 
 
}
void render_text(ShaderProgram* program, std::string text, float x, float y, float size) {
    float texture_size = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> tex_coords;

    for (size_t i = 0; i < text.size(); i++) {
        int character = (int)text[i];
        float offset = (size + 0.01f) * i;

        float u = (float)(character % 16) / 16.0f;
        float v = (float)(character / 16) / 16.0f;

        vertices.insert(vertices.end(), {
            x + offset, y,
            x + offset + size, y,
            x + offset + size, y + size,
            x + offset, y,
            x + offset + size, y + size,
            x + offset, y + size,
        });

        tex_coords.insert(tex_coords.end(), {
            u, v + texture_size,
            u + texture_size, v + texture_size,
            u + texture_size, v,
            u, v + texture_size,
            u + texture_size, v,
            u, v,
        });
    }

    glBindTexture(GL_TEXTURE_2D, g_font_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
void update() {
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, g_game_state.platforms, PLATFORM_COUNT);

        for (int i = 0; i < ENEMY_COUNT; i++) {
     
            if (!g_game_state.enemies[i].get_active()) {
                continue;
            }

       
            if (i == 2) {
                float angle_speed = 90.0f;
                float angle_rad = glm::radians(angle_speed * 0.2 * FIXED_TIMESTEP);
                glm::vec3 position = g_game_state.enemies[i].get_position();
                glm::vec3 pivot = glm::vec3(0.0f, 0.0f, 0.0f);

                glm::vec3 relative_position = position - pivot;
                float new_x = cos(angle_rad) * relative_position.x - sin(angle_rad) * relative_position.y;
                float new_y = sin(angle_rad) * relative_position.x + cos(angle_rad) * relative_position.y;

                g_game_state.enemies[i].set_position(glm::vec3(new_x, new_y, 0.0f) + pivot);
            }

            g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, g_game_state.platforms, PLATFORM_COUNT);

           
            if (g_game_state.player->check_collision(&g_game_state.enemies[i])) {
                float y_distance = fabs(g_game_state.player->get_position().y - g_game_state.enemies[i].get_position().y);
                float x_distance = fabs(g_game_state.player->get_position().x - g_game_state.enemies[i].get_position().x);

                if (y_distance > x_distance) {
                  
                    LOG("Player hit from top or bottom. Game over.");
                    g_game_state.player->deactivate();
                } else {
                  
                    LOG("Enemy hit from the side. Enemy dies.");
                    g_game_state.enemies[i].deactivate();
                }
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;
}



void render() {
    glClear(GL_COLOR_BUFFER_BIT);

 
    g_shader_program.set_model_matrix(glm::mat4(1.0f));
    glBindTexture(GL_TEXTURE_2D, g_background_texture_id);

    float vertices[] = { -5.0, -3.75, 5.0, -3.75, 5.0, 3.75, -5.0, -3.75, 5.0, 3.75, -5.0, 3.75 };
    float tex_coords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

   
    g_game_state.player->render(&g_shader_program);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        g_game_state.platforms[i].render(&g_shader_program);
    }

 
    bool all_enemies_inactive = true;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (g_game_state.enemies[i].get_active()) {
            g_game_state.enemies[i].render(&g_shader_program);
            all_enemies_inactive = false;
        }
    }

    if (!g_game_state.player->get_active()) {
       
        render_text(&g_shader_program, "You Lose", -5.0f, 0.0f, 0.5f);
    } else if (all_enemies_inactive) {
 
        render_text(&g_shader_program, "You Win", -5.0f, 0.0f, 0.5f);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete [] g_game_state.platforms;
    delete [] g_game_state.enemies;
    delete    g_game_state.player;
    Mix_FreeChunk(g_game_state.jump_sfx);
    Mix_FreeMusic(g_game_state.bgm);
}

// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
