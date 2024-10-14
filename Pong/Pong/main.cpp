/**
* Author: Sanjana Chowdary
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

SDL_Texture* ball_texture = nullptr;
SDL_Texture* background_texture = nullptr;
SDL_Texture* racket1_texture = nullptr;
SDL_Texture* racket2_texture = nullptr;
SDL_Texture* p1win_texture = nullptr;
SDL_Texture* p2win_texture = nullptr;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int RACKET_WIDTH = 50;
const int RACKET_HEIGHT = 100;
const int BALL_SIZE = 20;
const float RACKET_SPEED = 1.0f;
const float BALL_SPEED = 0.2f;
float ball_x = (WINDOW_WIDTH / 2);
float ball_y = (WINDOW_HEIGHT / 2);
float racket1_y = ((WINDOW_HEIGHT/2) - (RACKET_HEIGHT/2));
float racket2_y = ((WINDOW_HEIGHT/2) - (RACKET_HEIGHT/2));
float ball_dx = BALL_SPEED;
float ball_dy = BALL_SPEED;
int PlayerOneScore = 0;
int PlayerTwoScore = 0;
bool SinglePlayer = false;
bool end = false;



void handle_input(const Uint8* keys) {
    if (!SinglePlayer) {
        racket1_y = (keys[SDL_SCANCODE_W] && racket1_y > 0) ?
                    racket1_y - RACKET_SPEED * 10 :
                    (keys[SDL_SCANCODE_S] && racket1_y + RACKET_HEIGHT < WINDOW_HEIGHT) ?
                    racket1_y + RACKET_SPEED * 10 :
                    racket1_y;
    }

    racket2_y = (keys[SDL_SCANCODE_UP] && racket2_y > 0) ?
                racket2_y - RACKET_SPEED * 10 :
                (keys[SDL_SCANCODE_DOWN] && racket2_y + RACKET_HEIGHT < WINDOW_HEIGHT) ?
                racket2_y + RACKET_SPEED * 10 :
                racket2_y;

    if (keys[SDL_SCANCODE_T]) {
        SinglePlayer = !SinglePlayer;
        racket1_y = SinglePlayer ? (WINDOW_HEIGHT / 2) - (RACKET_HEIGHT / 2) : racket1_y;
    }
}
void move_ball() {
    ball_x += ball_dx * 10;
    ball_y += ball_dy * 10;

    ball_dy = (ball_y <= 0 || ball_y + BALL_SIZE >= WINDOW_HEIGHT) ? -ball_dy : ball_dy;

    ball_dx = (ball_x <= 20 && ball_y >= racket1_y && ball_y <= racket1_y + RACKET_HEIGHT) ?
              -ball_dx :
              (ball_x + BALL_SIZE >= WINDOW_WIDTH - 20 && ball_y >= racket2_y && ball_y <= racket2_y + RACKET_HEIGHT) ?
              -ball_dx :
              ball_dx;
    
    if (ball_x <= 0) {
        PlayerTwoScore++;
        end = true;
    } else if (ball_x + BALL_SIZE >= WINDOW_WIDTH) {
        PlayerOneScore++;
        end = true;
    }
}

void ai_move_paddle() {
    racket1_y += (ball_y < racket1_y) ? -RACKET_SPEED * 5 :
                 (ball_y > racket1_y + RACKET_HEIGHT) ? RACKET_SPEED * 5 : 0;

    racket1_y = std::max(0.0f, std::min(static_cast<float>(WINDOW_HEIGHT - RACKET_HEIGHT), racket1_y));
}
void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect background_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);

    if (!end) {
        SDL_Rect paddle1 = {10, static_cast<int>(racket1_y), RACKET_WIDTH, RACKET_HEIGHT};
        SDL_Rect paddle2 = {WINDOW_WIDTH - 60, static_cast<int>(racket2_y), RACKET_WIDTH, RACKET_HEIGHT};
        SDL_Rect ball = {static_cast<int>(ball_x), static_cast<int>(ball_y), BALL_SIZE, BALL_SIZE};

        SDL_RenderCopy(renderer, racket1_texture, nullptr, &paddle1);
        SDL_RenderCopy(renderer, racket2_texture, nullptr, &paddle2);
        SDL_RenderCopy(renderer, ball_texture, nullptr, &ball);
    } else {
        SDL_Rect result_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_Texture* result_texture = (PlayerOneScore > PlayerTwoScore) ? p1win_texture : p2win_texture;
        SDL_RenderCopy(renderer, result_texture, nullptr, &result_rect);
    }

    SDL_RenderPresent(renderer);
}



void shutdown() { SDL_Quit(); }

void load_textures(SDL_Renderer* renderer) {
    racket1_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/racket1.png");
    racket2_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/racket2.png");
    ball_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/ball.png");
    background_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/tennis_court.png");
    p1win_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/result1.png");
    p2win_texture = IMG_LoadTexture(renderer, "/Users/anuraghav/Desktop/sahil/result2.png");

    if (!racket1_texture || !racket2_texture || !ball_texture || !background_texture ||
        !p1win_texture || !p2win_texture) {
        std::cerr << "Failed to load textures! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Pong Clone - Space Battle",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    load_textures(renderer);

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        handle_input(keys);
        move_ball();

        if (SinglePlayer) {
            ai_move_paddle();
        }

        render(renderer);

        if (end) {
            std::cout << "Player 1: " << PlayerTwoScore << " | Player 2: " << PlayerOneScore << std::endl;
            SDL_Delay(2000);
            quit = true;
        }

        SDL_Delay(10);
    }

    shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}




