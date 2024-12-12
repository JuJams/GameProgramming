#include "Level3.h"
#define LEVEL3_WIDTH 30
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 3

using namespace std;

unsigned int Level3_data[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 ,0 ,0, 0, 0, 0, 0, 0, 0, 0, 0 ,1 ,0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 ,0 ,0, 0, 0, 0, 0, 0, 1, 0, 0 ,0 ,1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0, 0, 0, 0, 0, 0, 0, 0, 1 ,0 ,0,
    0, 0, 0, 0, 0, 0, 1, 0, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0 ,0 ,0, 0, 0, 0, 0, 0, 1, 1, 0 ,0 ,0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 2, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0 ,0 ,0,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0,
    2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0
};


std::string actualNotes3 = "";
Mix_Chunk* keySounds3[6];

void Level3::Initialize(int numLives) {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, Level3_data, mapTextureID, 1.0f, 4, 1);

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

    // Initialize Enemy
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("bug.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(10, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].jumpPower = 5.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.81, 0);
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(19, -3, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].jumpPower = 5.0f;
    state.enemies[1].acceleration = glm::vec3(0, -9.81, 0);
    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(4, -4, 0);
    state.enemies[2].speed = 1;
    state.enemies[2].aiType = WAITANDGO;
    state.enemies[2].aiState = IDLE;
    state.enemies[2].jumpPower = 5.0f;
    state.enemies[2].acceleration = glm::vec3(0, -9.81, 0);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
        state.enemies[i].isActive = true;
    }

    
    state.keysCollected = "";
    state.requiredKeys = "CGDAE"; // Chord: C6/9

    GLuint keyTextureID = Util::LoadTexture("key.png");
    state.keys = new Entity[5];


    state.keys[0].entityType = KEY; // C
    state.keys[0].textureID = keyTextureID;
    state.keys[0].position = glm::vec3(3, -1, 0);

    state.keys[1].entityType = KEY; // G
    state.keys[1].textureID = keyTextureID;
    state.keys[1].position = glm::vec3(6, -2, 0);

    state.keys[2].entityType = KEY; // D
    state.keys[2].textureID = keyTextureID;
    state.keys[2].position = glm::vec3(16, -6, 0);

    state.keys[3].entityType = KEY; // A
    state.keys[3].textureID = keyTextureID;
    state.keys[3].position = glm::vec3(30, -1, 0);

    state.keys[4].entityType = KEY; // E
    state.keys[4].textureID = keyTextureID;
    state.keys[4].position = glm::vec3(12, -4, 0);

    for (int i = 0; i < 5; ++i) {
        state.keys[i].isActive = true;
    }

  
    keySounds3[0] = Mix_LoadWAV("key_C.wav");
    keySounds3[1] = Mix_LoadWAV("key_G.wav");
    keySounds3[2] = Mix_LoadWAV("key_D.wav");
    keySounds3[3] = Mix_LoadWAV("key_a2.wav");
    keySounds3[4] = Mix_LoadWAV("key_E.wav");
    keySounds3[5] = Mix_LoadWAV("final.wav");

    for (int i = 0; i < 5; ++i) {
        if (keySounds3[i] == nullptr) {
            std::cerr << "Failed to load key sound " << i << ": " << Mix_GetError() << std::endl;
        }
    }
}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

  
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
    bool anyAlive = false;
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
        if (state.enemies[i].isActive == true) {
            anyAlive = true;
            break;
        }
    }
    if (state.player->position.y <= -10 || state.player->isActive == false) {
        state.player->isActive = true;
        state.player->numLives -= 1;
        state.player->position = glm::vec3(1, 0, 0);
        
        if (state.player->numLives <= 0) {
                    state.nextScene = 4;  // Transition to Scene 4
                }
    }

    for (int i = 0; i < 5; ++i) {
        state.keys[i].modelMatrix = glm::mat4(1.0f);
        state.keys[i].modelMatrix = glm::translate(state.keys[i].modelMatrix, state.keys[i].position);
        if (state.keys[i].isActive && state.player->CheckCollision(&state.keys[i])) {
            size_t expectedIndex = state.keysCollected.length();
            if ((expectedIndex == 0 && i == 0) || // C
                (expectedIndex == 1 && i == 1) || // G
                (expectedIndex == 2 && i == 2) || // D
                (expectedIndex == 3 && i == 3) ) { // E

                Mix_PlayChannel(-1, keySounds3[i], 0);
                state.keysCollected += std::to_string(i);
                state.keys[i].isActive = false;
            }
            else if((expectedIndex == 4 && i == 4)){
                Mix_PlayChannel(-1, keySounds3[i], 0);
                SDL_Delay(500);
                Mix_PlayChannel(-1, keySounds3[0], 0);
                Mix_PlayChannel(-1, keySounds3[1], 0);
                Mix_PlayChannel(-1, keySounds3[2], 0);
                Mix_PlayChannel(-1, keySounds3[3], 0);
                Mix_PlayChannel(-1, keySounds3[4], 0);
                SDL_Delay(500);
                Mix_PlayChannel(-1, keySounds3[5], 0);
                
                state.keysCollected += std::to_string(i);
                state.keys[i].isActive = false;
            }else {
            
                state.player->numLives -= 1;
                state.player->position = glm::vec3(1, -2, 0);
                state.keysCollected = "";
                for (int j = 0; j < 5; ++j) {
                    state.keys[j].isActive = true;
                }
                break;
            }
        }
    }


    if (state.keysCollected == "01234") {
        state.nextScene = 5;
    }
    for (int i = 0; i < state.keysCollected.size(); i++) {
        
        int collectedKeyIndex = state.keysCollected[i] - '0';

        if (collectedKeyIndex == 0 && actualNotes3[i] != 'C')
            actualNotes3 += 'C';
        else if (collectedKeyIndex == 1 && actualNotes3[i] != 'G')
            actualNotes3 += 'G';
        else if (collectedKeyIndex == 2 && actualNotes3[i] != 'D')
            actualNotes3 += 'D';
        else if (collectedKeyIndex == 3 && actualNotes3[i] != 'A')
            actualNotes3 += 'A';
        else if (collectedKeyIndex == 4 && actualNotes3[i] != 'D')
            actualNotes3 += 'D';
    }
}

void Level3::Render(ShaderProgram *program) {
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Level 3", 0.3f, -0.1f, glm::vec3(0.2, -0.6, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"), "Lives " + std::to_string(state.player->numLives), 0.3f, -0.1f, glm::vec3(0.2, -0.3, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"),
                   "Complete Chord: C6/9", 0.3f, -0.1f, glm::vec3(0.5, -4, 0));
    Util::DrawText(program, Util::LoadTexture("font1.png"),
                   "Keys Collected: " + actualNotes3, 0.3f, -0.1f, glm::vec3(0.5, -4.4, 0));
    state.map->Render(program);
    state.player->Render(program);


    for (int i = 0; i < LEVEL3_ENEMY_COUNT; ++i) {
        state.enemies[i].Render(program);
    }

    for (int i = 0; i < 5; ++i) {
        state.keys[i].Render(program);

    
        std::string keyName;
        if (i == 0) keyName = "C";
        else if (i == 1) keyName = "G";
        else if (i == 2) keyName = "D";
        else if (i == 3) keyName = "A";
        else if (i == 4) keyName = "E";

        Util::DrawText(program, Util::LoadTexture("font1.png"), keyName, 0.3f, -0.05f,
                       glm::vec3(state.keys[i].position.x - 0.6f, state.keys[i].position.y + 0.5f, 0));
    }
}
