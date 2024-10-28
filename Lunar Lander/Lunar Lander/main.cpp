/**
* Author: Sanjana Chowdary
* Assignment: Lunar Lander
* Date due: 2024-10-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
    #define GL_SILENCE_DEPRECATION
    #define STB_IMAGE_IMPLEMENTATION
    #define LOG(argument) std::cout << argument << '\n'

    #ifdef _WINDOWS
    #include <GL/glew.h>
    #endif

    #define GL_GLEXT_PROTOTYPES 1
    #include <SDL.h>
    #include <SDL_opengl.h>
    #include "glm/mat4x4.hpp"
    #include "glm/gtc/matrix_transform.hpp"
    #include "ShaderProgram.h"
    #include "stb_image.h"
    #include "cmath"
    #include <ctime>

    enum AppStatus { RUNNING, DISPLAY_MESSAGE, TERMINATED };

 
    constexpr float GRAVITY = -0.2f;
    constexpr float HORIZONTAL_ACCELERATION = 0.5f;
    constexpr float FRICTION = 1.0f;
    constexpr float DISPLAY_DELAY_SECONDS = 2.0f;
    float end_time = 0.0f;
    GLuint g_mission_failed_texture_id;
    GLuint g_mission_accomplished_texture_id;
    bool mission_success = false;



    glm::vec3 m_acceleration = glm::vec3(0.0f, GRAVITY, 0.0f);

    constexpr float WINDOW_SIZE_MULT = 1.0f;
    constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
                  WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;
    constexpr float BG_RED     = 1.0f,
                    BG_GREEN   = 1.0f,
                    BG_BLUE    = 1.0f,
                    BG_OPACITY = 1.0f;
    constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;
    constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
    constexpr GLint NUMBER_OF_TEXTURES = 1;
    constexpr GLint LEVEL_OF_DETAIL    = 0;
    constexpr GLint TEXTURE_BORDER     = 0;
    constexpr float MILLISECONDS_IN_SECOND = 1000.0;
    constexpr char BEAKER_SPRITE_FILEPATH[] = "cactus.png";
    constexpr char DROPS_SPRITE_FILEPATH[]  = "dino.png";
    constexpr char DESERT_SPRITE_FILEPATH[] = "desert.png";
    constexpr char CLOUD_SPRITE_FILEPATH[] = "cloud.png";
    GLuint g_cloud_texture_id;
    

    std::vector<glm::vec3> cloud_positions = {
        glm::vec3(-3.0f, 0.5f, 0.0f),
        glm::vec3(2.0f, 0.3f, 0.0f),
        glm::vec3(4.0f, 1.1f, 0.0f)
    };

    GLuint g_desert_texture_id;
    glm::mat4 g_cactus_matrix;



    constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
    constexpr glm::vec3 INIT_POS_DROPS    = glm::vec3(0.0f, 3.0f, 0.0f);
    constexpr glm::vec3 INIT_SCALE_DROPS  = glm::vec3(1.0f, 1.0f, 0.0f);
    constexpr glm::vec3 INIT_SCALE_BEAKER = glm::vec3(0.5f, 1.0f, 0.0f);
    constexpr glm::vec3 INIT_POS_BEAKER   = glm::vec3(3.0f, -2.0f, 0.0f);
    
    std::vector<glm::vec3> cactus_positions = {
        glm::vec3(-3.0f, -3.0f, 0.0f), glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(3.5f, -3.0f, 0.0f), glm::vec3(1.0f, -3.0f, 0.0f)
    };

    SDL_Window* g_display_window;

    AppStatus g_app_status = RUNNING;
    ShaderProgram g_shader_program = ShaderProgram();
    glm::mat4 g_view_matrix, g_beaker_matrix, g_projection_matrix, g_drops_matrix;

    float g_previous_ticks = 0.0f;

    GLuint g_beaker_texture_id;
    GLuint g_drops_texture_id;
  
    constexpr float DROPS_SPEED = 3.0f;

    glm::vec3 g_beaker_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 g_beaker_movement = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 g_drops_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 g_drops_movement = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 g_drops_scale = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 g_drops_size  = glm::vec3(1.0f, 1.0f, 0.0f);

    void initialise();
    void process_input();
    void update();
    void render();
    void shutdown();

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

    
        stbi_image_free(image);

        return textureID;
    }
    
    void mission_failed() {
        mission_success = false;
        g_app_status = DISPLAY_MESSAGE;
        end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    }

    void mission_accomplished() {
        mission_success = true;
        g_app_status = DISPLAY_MESSAGE;
        end_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    }


    void initialise()
    {
        SDL_Init(SDL_INIT_VIDEO);
        g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            WINDOW_WIDTH, WINDOW_HEIGHT,
                                            SDL_WINDOW_OPENGL);

        SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
        SDL_GL_MakeCurrent(g_display_window, context);

        if (g_display_window == nullptr) shutdown();

    #ifdef _WINDOWS
        glewInit();
    #endif

        glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
        g_beaker_matrix = glm::mat4(1.0f);
        g_drops_matrix = glm::mat4(1.0f);
        g_drops_matrix = glm::translate(g_drops_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
        g_drops_position += g_drops_movement;
        g_view_matrix = glm::mat4(1.0f);
        g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
        g_shader_program.set_projection_matrix(g_projection_matrix);
        g_shader_program.set_view_matrix(g_view_matrix);
        glUseProgram(g_shader_program.get_program_id());
        glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
        g_beaker_texture_id = load_texture(BEAKER_SPRITE_FILEPATH);
        g_drops_texture_id = load_texture(DROPS_SPRITE_FILEPATH);
        g_desert_texture_id = load_texture(DESERT_SPRITE_FILEPATH);
        g_cloud_texture_id = load_texture(CLOUD_SPRITE_FILEPATH);
        g_mission_failed_texture_id = load_texture("fail.png");
        g_mission_accomplished_texture_id = load_texture("success.png");
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }


    void process_input()
    {
        m_acceleration.x = 0.0f;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    g_app_status = TERMINATED;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                            g_app_status = TERMINATED;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        const Uint8 *key_state = SDL_GetKeyboardState(NULL);
        
  
        if (key_state[SDL_SCANCODE_A]) {
            m_acceleration.x = -HORIZONTAL_ACCELERATION;
        }
        else if (key_state[SDL_SCANCODE_D]) {
            m_acceleration.x = HORIZONTAL_ACCELERATION;
        }
    }


    void update()
    {
        
        float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        m_acceleration.y = GRAVITY;
        g_drops_movement += m_acceleration * delta_time;
        g_drops_movement.x *= FRICTION;
        g_drops_position += g_drops_movement * delta_time;
        g_drops_matrix = glm::mat4(1.0f);
        g_drops_matrix = glm::translate(g_drops_matrix, INIT_POS_DROPS + g_drops_position);

       
        for (const auto& cloud_position : cloud_positions) {
            float x_distance = fabs(g_drops_position.x - cloud_position.x);
            float y_distance = fabs(g_drops_position.y - cloud_position.y);
            if (x_distance < 1.0f && y_distance < 1.0f) {
                mission_failed();
                return;
            }
        }


        for (const auto& cactus_position : cactus_positions) {
            float x_distance = fabs(g_drops_position.x - cactus_position.x);
            float y_distance = fabs(g_drops_position.y - cactus_position.y);
            if (g_drops_position.y <= -3.0f && x_distance < 0.5f && y_distance < 0.1f) {
                mission_failed();
                return;
            }
        }

  
        if (g_drops_position.y <= -3.75f) {
            mission_accomplished();
        }
    }

    void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
    {
        g_shader_program.set_model_matrix(object_model_matrix);
        glBindTexture(GL_TEXTURE_2D, object_texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void render() {
        glClear(GL_COLOR_BUFFER_BIT);

        float vertices[] = {
            -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
        };

        float texture_coordinates[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };

        glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(g_shader_program.get_position_attribute());

        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        if (g_app_status == DISPLAY_MESSAGE) {
            glm::mat4 message_matrix = glm::mat4(1.0f);
            message_matrix = glm::scale(message_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
            GLuint message_texture_id = (mission_success) ? g_mission_accomplished_texture_id : g_mission_failed_texture_id;
            draw_object(message_matrix, message_texture_id);
            
        } else if (g_app_status == RUNNING) {
            
            glm::mat4 desert_matrix = glm::mat4(1.0f);
            desert_matrix = glm::translate(desert_matrix, glm::vec3(0.0f, -2.8f, 0.0f));
            desert_matrix = glm::scale(desert_matrix, glm::vec3(10.0f, 3.5f, 1.0f));
            draw_object(desert_matrix, g_desert_texture_id);

            for (const auto& cactus_position : cactus_positions) {
                g_beaker_matrix = glm::mat4(1.0f);
                g_beaker_matrix = glm::translate(g_beaker_matrix, cactus_position + glm::vec3(0.0f, 2.0f, 0.0f));
                g_beaker_matrix = glm::scale(g_beaker_matrix, INIT_SCALE_BEAKER);
                draw_object(g_beaker_matrix, g_beaker_texture_id);
            }

            for (const auto& cloud_position : cloud_positions) {
                glm::mat4 cloud_matrix = glm::mat4(1.0f);
                cloud_matrix = glm::translate(cloud_matrix, cloud_position);
                cloud_matrix = glm::scale(cloud_matrix, glm::vec3(1.5f, 1.0f, 1.0f));
                draw_object(cloud_matrix, g_cloud_texture_id);
            }

            draw_object(g_drops_matrix, g_drops_texture_id);
        }


        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        SDL_GL_SwapWindow(g_display_window);
    }

    void shutdown() { SDL_Quit(); }


    int main(int argc, char* argv[])
    {
        initialise();

        while (g_app_status == RUNNING || g_app_status == DISPLAY_MESSAGE)
        {
            if (g_app_status == RUNNING) {
                process_input();
                update();
            }
            render();

            if (g_app_status == DISPLAY_MESSAGE) {
                float current_time = SDL_GetTicks() / MILLISECONDS_IN_SECOND;
                if (current_time - end_time >= DISPLAY_DELAY_SECONDS) {
                    g_app_status = TERMINATED;
                }
            }
        }
        shutdown();
        return 0;
    }


