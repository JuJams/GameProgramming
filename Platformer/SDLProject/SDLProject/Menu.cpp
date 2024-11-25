#include "Menu.h"

void Menu::Initialize(int numLives) {
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
void Menu::Update(float deltaTime) {
    if (deltaTime == -1) {
        state.nextScene = 1;
    }
}
void Menu::Render(ShaderProgram *program) {
    
    GLuint backgroundTextureID = Util::LoadTexture("background.jpg");
    float vertices[] = {
        -10, -7,
         10, -7,
         10,  7,
        -10,  7
    };
    float texCoords[] = {
        0, 1,
        1, 1,
        1, 0,
        0, 0
    };
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

     
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Platformer by", 1, -0.5, glm::vec3(1.75f, -3.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Sanjana Chowdary", 1, -0.5, glm::vec3(1.25f, -4.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Press Enter/Return", 1, -0.5, glm::vec3(1.0f, -5.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "to Start", 1, -0.5, glm::vec3(3.25f, -6.0f, 0));
}
