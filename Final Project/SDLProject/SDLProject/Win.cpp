#include "Win.h"

void Win::Initialize(int numLives) {
    state.nextScene = -1;
    state.player = new Entity();
        state.player->entityType = PLAYER;
        state.player->position = glm::vec3(5, 0, 0);
        state.player->movement = glm::vec3(0);
        state.player->acceleration = glm::vec3(0, -9.81f, 0);
        state.player->speed = 3.0f;
        state.player->textureID = Util::LoadTexture("micheal.png");
        
        state.player->animRight = new int[4] {3, 7, 11, 15};
        state.player->animLeft = new int[4] {1, 5, 9, 13};
        state.player->animUp = new int[4] {2, 6, 10, 14};
        state.player->animDown = new int[4] {0, 4, 8, 12};
        
        state.player->animIndices = state.player->animRight;
        state.player->animFrames = 4;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 4;
        state.player->animRows = 4;
        
    state.player->numLives = numLives;
    
        state.player->height = 0.8f;
        state.player->width = 0.8f;
        
        state.player->jumpPower = 7.0f;
}
void Win::Update(float deltaTime) {
    
}
void Win::Render(ShaderProgram *program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "You Win!", 1.5, -0.5, glm::vec3(1.75f, -3.5f, 0));
}
