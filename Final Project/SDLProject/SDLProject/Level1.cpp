// Level1.cpp
#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 2
using namespace std;

unsigned int level1_data[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2,
    2, 2, 2, 2, 2, 2, 0, 0, 1, 1, 1, 2, 2, 2
};


std::string actualNotes = "";
Mix_Chunk* keySounds[3];

void Level1::Initialize(int numLives) {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -2, 0);
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
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("bug.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(5, -5.0f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].jumpPower = 5.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.81, 0);
        
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(11, 0.0f, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].jumpPower = 5.0f;
    state.enemies[1].acceleration = glm::vec3(0, -9.81, 0);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        state.enemies[i].isActive = true;
    }
    
    state.keysCollected = "";
    state.requiredKeys = "CEG";

    GLuint keyTextureID = Util::LoadTexture("key.png");
    

    state.keys = new Entity[3];

    
    state.keys[0].entityType = KEY;
    state.keys[0].textureID = keyTextureID;
    state.keys[0].position = glm::vec3(2, -1, 0);
    state.keys[0].isActive = true;

   
    state.keys[1].entityType = KEY;
    state.keys[1].textureID = keyTextureID;
    state.keys[1].position = glm::vec3(6, -2, 0);
    state.keys[1].isActive = true;

    state.keys[2].entityType = KEY;
    state.keys[2].textureID = keyTextureID;
    state.keys[2].position = glm::vec3(9, 0, 0);
    state.keys[2].isActive = true;
    
    

    keySounds[0] = Mix_LoadWAV("key_E.wav"); 
    keySounds[1] = Mix_LoadWAV("key_C.wav");
    keySounds[2] = Mix_LoadWAV("key_G.wav");


    for (int i = 0; i < 3; ++i) {
        if (keySounds[i] == nullptr) {
            std::cout << "Failed to load key sound " << i << ": " << Mix_GetError() << std::endl;
        }
    }
    


}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    if (state.player->numLives == 0) {
        state.nextScene = 4;
    }
    bool anyAlive = false;
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        if (state.enemies[i].isActive == true) {
            anyAlive = true;
            break;
        }
    }
    if (state.player->position.y <= -10 || state.player->isActive == false) {
        state.player->isActive = true;
        state.player->numLives -= 1;
        state.player->position = glm::vec3(1, 0, 0);
    
    }
    
    
        for (int i = 0; i < 3; ++i) {
            
            state.keys[i].modelMatrix = glm::mat4(1.0f);
            state.keys[i].modelMatrix = glm::translate(state.keys[i].modelMatrix, state.keys[i].position);
            if (state.keys[i].isActive && state.player->CheckCollision(&state.keys[i])) {
                    
                __SIZE_TYPE__ expectedIndex = state.keysCollected.length();
                    if ((expectedIndex == 0 && i == 1) ||
                        (expectedIndex == 1 && i == 0)){
                        
                        
                        Mix_PlayChannel(-1, keySounds[i], 0);

                        state.keysCollected += std::to_string(i);
                        state.keys[i].isActive = false;
                    }
                else if(expectedIndex == 2 && (i == 2) ){
                    Mix_PlayChannel(-1, keySounds[i], 0);
                    
                    SDL_Delay(500);
                    Mix_PlayChannel(-1, keySounds[0], 0);
                    Mix_PlayChannel(-1, keySounds[1], 0);
                    Mix_PlayChannel(-1, keySounds[2], 0);
                    state.keysCollected += std::to_string(i);
                    state.keys[i].isActive = false;
                }
                    else {
                        
                        state.player->numLives -= 1;
                        state.player->position = glm::vec3(1, -2, 0);
                        state.keysCollected = "";

                      
                        for (int j = 0; j < 3; ++j) {
                            state.keys[j].isActive = true;
                        }
                        break;
                    }
                }
        }


        if (state.keysCollected == "102") {
            state.nextScene = 2;
        }
    for (int i = 0; i < state.keysCollected.size(); i++) {
        
        int collectedKeyIndex = state.keysCollected[i] - '0';

        if (collectedKeyIndex == 0 && actualNotes[i] != 'E')
            actualNotes += 'E';
        else if (collectedKeyIndex == 1 && actualNotes[i] != 'C')
            actualNotes += 'C';
        else if (collectedKeyIndex == 2 && actualNotes[i] != 'G')
            actualNotes += 'G';
    }
    

}
void Level1::Render(ShaderProgram *program) {
    
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Level 1", 0.3f, -0.1f, glm::vec3(0.2, -0.6, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Lives " + std::to_string(state.player->numLives), 0.3f, -0.1f, glm::vec3(0.2, -0.3, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"),
                   "Complete Chord: C Major", 0.3f, -0.1f, glm::vec3(0.5, -4, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"),
                   "Keys Collected: " + actualNotes, 0.3f, -0.1f, glm::vec3(0.5, -4.4, 0));

    state.map->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        state.enemies[i].Render(program);
    }
    for (int i = 0; i < 3; ++i) {
            state.keys[i].Render(program);


            std::string keyName;
            if (i == 0) keyName = "E";
            else if (i == 1) keyName = "C";
            else if (i == 2) keyName = "G";

            Util::DrawText(program, Util::LoadTexture("font1.png"), keyName, 0.3f, -0.05f,
                           glm::vec3(state.keys[i].position.x - 0.3f, state.keys[i].position.y - 0.3f, 0));
        }
    for (int i = 0; i < 3; ++i) {
        state.keys[i].Render(program);
    }

}
