#include "Level3.h"
#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define L3ENEMY_COUNT 3


unsigned int level3_data[] =
{
 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,
 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 9,
 8, 0, 0, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 9,
 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,
 8, 0, 0, 0, 0, 0, 0, 0, 19, 20, 21, 0, 0, 9,
 8, 0, 0, 0, 0, 0, 0, 1, 9, 28, 29, 30, 17, 9,
 9, 1, 1, 1, 1, 3, 1, 9, 9, 9, 9, 9, 9, 9,
 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
};

bool win = false;


void Level3::Initialize() {
    state.nextScene = -1;
    state.enemyCount = L3ENEMY_COUNT;
    GLuint mapTextureID = Util::LoadTexture("DarkTiles.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 8, 4);
    // Move over all of the player and enemy code from initialization.
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(2, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.8f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = Util::LoadTexture("Warrior.png");

    state.player->animIdle = new int[6]{ 0, 1, 2, 3, 4 ,5 };
    state.player->animRight = new int[6]{ 6, 7, 8, 9,10, 11 };
    state.player->animAttack = new int[6]{ 18,19,20,21,22,23 };

    state.player->animIndices = state.player->animIdle;
    state.player->animFrames = 6;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 6;
    state.player->animRows = 17;

    state.player->height = 1.0f;
    state.player->width = 0.8f;

    state.player->jumpPower = 5.0f;

    state.enemies = new Entity[L3ENEMY_COUNT];
    GLuint enemeyTextureID = Util::LoadTexture("Golem.png");

    state.enemies[0].position = glm::vec3(6, -5, 0);
    state.enemies[0].AItype = STALKER;
    state.enemies[0].AIState = LURK;

    state.enemies[1].position = glm::vec3(8, -3, 0);
    state.enemies[1].AItype = LURKER;
    state.enemies[1].AIState = LURK;

    state.enemies[2].position = glm::vec3(5, 0, 0);
    state.enemies[2].AItype = WALKER;
    state.enemies[2].AIState = WALKING;


    for (int i = 0; i < L3ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemeyTextureID;
        state.enemies[i].acceleration = glm::vec3(0, -9.8f, 0);
        state.enemies[i].speed = 0.5;

        state.enemies[i].animIdle = new int[4]{ 0, 1, 2, 3 };
        state.enemies[i].animAttack = new int[9]{ 20, 21, 22, 23,24, 25,26,27,28 };
        state.enemies[i].height = 1.0;
        state.enemies[i].width = 0.8;

        state.enemies[i].animIndices = new int[1]{ 37 };
        state.enemies[i].animFrames = 1;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 10;
        state.enemies[i].animRows = 10;
    }

}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, L3ENEMY_COUNT);
    for (int i = 0; i < L3ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.map, state.enemies, L3ENEMY_COUNT);

    }

    if (state.player->position.x > 12) {
        state.nextScene = 4;
    }
}
void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < L3ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    };
    if (win){
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "Samira and Golems", 0.5f, -0.25f,
        glm::vec3(3, -2, 0));
    }
}