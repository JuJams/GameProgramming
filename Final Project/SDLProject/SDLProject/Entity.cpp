#include "Entity.h"
#include <iostream>
using namespace std;

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
   for (int i = 0; i < objectCount; i++)
   {
       Entity *object = &objects[i];
       if (CheckCollision(object))
       {
           float ydist = fabs(position.y - object->position.y);
           float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
           if (velocity.y > 0) {
               position.y -= penetrationY;
               velocity.y = 0;
               collidedTop = true;
               object->collidedBottom = true;
           }
           else if (velocity.y < 0) {
               position.y += penetrationY;
               velocity.y = 0;
               collidedBottom = true;
               object->collidedTop = true;
} }
} }

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
   for (int i = 0; i < objectCount; i++)
   {
       Entity *object = &objects[i];
       if (CheckCollision(object))
       {
           float xdist = fabs(position.x - object->position.x);
           float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
           if (velocity.x > 0) {
               position.x -= penetrationX;
               velocity.x = 0;
               collidedRight = true;
               object->collidedLeft = true;
           }
           else if (velocity.x < 0) {
               position.x += penetrationX;
               velocity.x = 0;
               collidedLeft = true;
               object->collidedRight = true;
} }
} }

void Entity::CheckEnemyCollided(Entity *enemies, int enemyCount) {
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    CheckCollisionsX(enemies, enemyCount);
    if (collidedLeft == true || collidedRight == true) isActive = false;
    CheckCollisionsY(enemies, enemyCount);
    if (collidedTop == true) isActive = false;
    if (collidedBottom == true) {
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i].collidedTop == true) {
                enemies[i].isActive = false;
            }
        }
    }
    
    /*for (int i = 0; i < enemyCount; ++i) {
        if (CheckCollision(&enemies[i])) {
            if (collidedBottom && enemies[i].collidedTop) {
                cout << "enemy dead" << endl;
                enemies[i].isActive = false;
            }
            else {
                cout << "we ded" << endl;
                isActive = false;
                break;
            }
        }
    }*/
}

void Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
    position.y += penetration_y;
    velocity.y = 0;
    collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{

    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
}
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
} }

void Entity::CheckPit(Entity *platforms, int platformCount) {
    int leftLocation = -1;
    int rightLocation = -1;
    glm::vec3 sensorRight = glm::vec3(position.x + 0.6f, position.y - 0.6f, 0);
    glm::vec3 sensorLeft = glm::vec3(position.x - 0.6f, position.y - 0.6f, 0);
    for (int i = 0; i < platformCount; ++i) {
        float leftDistX = fabs(sensorLeft.x - platforms[i].position.x) - ((width + platforms[i].width) / 2.0f);
        float rightDistX = fabs(sensorRight.x - platforms[i].position.x) - ((width + platforms[i].width) / 2.0f);
        float leftDistY = fabs(sensorLeft.y - platforms[i].position.y) - ((height + platforms[i].height) / 2.0f);
        float rightDistY = fabs(sensorRight.y - platforms[i].position.y) - ((height + platforms[i].height) / 2.0f);
        
        if (leftDistX < 0 && leftDistY < 0) leftLocation = i;
        if (rightDistX < 0 && rightDistY < 0) rightLocation = i;
    }
    if (leftLocation == -1) pitLeft = true;
    if (rightLocation == -1) pitRight = true;
}

void Entity::AI(Entity *player) {
    switch(aiType) {
        case WALKER:
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case JUMPER:
            AIJumper();
            break;
    }
}

void Entity::AIJumper() {
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
}

void Entity::AIWalker(Entity *platforms, int platformCount) {
    if (movement == glm::vec3(0)) {
        movement = glm::vec3(-1, 0, 0);
    }
    CheckPit(platforms, platformCount);
    if (pitLeft) {
        movement = glm::vec3(1, 0, 0);
        pitLeft = false;
    }
    if (pitRight) {
        movement = glm::vec3(-1, 0, 0);
        pitRight = false;
    }
}

void Entity::AIWaitAndGo(Entity *player) {
    switch(aiState) {
        case IDLE:
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            }
            else {
                movement = glm::vec3(1, 0, 0);
            }
            break;
            
        case JUMPING:
            break;
            
        case ATTACKING:
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map)
{
    if (isActive == false) {
        return;
    }
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    pitLeft = false;
    pitRight = false;
    
    if (entityType == ENEMY) {
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    
    
    if(entityType == PLAYER) {
        //CheckCollisionsY(objects, objectCount);
        //CheckCollisionsX(objects, objectCount);
        CheckEnemyCollided(objects, objectCount);
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (!isActive) return;

    program->SetModelMatrix(modelMatrix);

    if (animIndices != nullptr) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

