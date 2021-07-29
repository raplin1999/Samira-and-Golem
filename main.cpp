#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SDL_mixer.h"
#include <vector>

using namespace std;

#include "Util.h"
#include "Map.h"
#include "Entity.h"
#include <vector>

#include "scene.h"
#include "Level1.h" 
#include "Level2.h" 
#include "Level3.h" 
#include "Menu.h"
#include "GameOver.h"

Scene* currentScene;
Scene* SceneList[5];


SDL_Window* displayWindow;
bool gameIsRunning = true;
bool missionInProgress = true;
int lives = 3;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music* music;
Mix_Chunk* fallWater;
Mix_Chunk* death;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}



void Initialize() {


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);


#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("Beauty_Flow.mp3");
    fallWater = Mix_LoadWAV("FallWater.wav");
    death= Mix_LoadWAV("DeathSound.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
   


    // Initialize Game Objects
    SceneList[0] = new Menu();
    SceneList[1] = new Level1();
    SceneList[2] = new Level2();
    SceneList[3] = new Level3(); 
    SceneList[4] = new GameOver();
    SwitchToScene(SceneList[0]);
}


void ProcessInput() {
    currentScene->state.player->movement = glm::vec3(0);
    currentScene->state.player->animIndices = currentScene->state.player->animIdle;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                break;

            case SDLK_RIGHT:
                break;

            case SDLK_SPACE:
                currentScene->state.player->jump = true;
                break;
            case SDLK_RETURN:
                if (currentScene == SceneList[0]) {
                    currentScene->state.nextScene = 1;
                }
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }


    if (keys[SDL_SCANCODE_A]) {
        currentScene->state.player->attack = true;
    }

    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    //abs(currentScene->state.player->position.x - currentScene->state.enemies[i].position.x < 1)
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        if (missionInProgress) {
            currentScene->Update(FIXED_TIMESTEP);
            if (currentScene->state.player->collidedLeft || currentScene->state.player->collidedRight) {
                for (int i = 0; i < currentScene->state.enemyCount; i++) {
                    if (currentScene->state.enemies[i].collidedRight || currentScene->state.enemies[i].collidedLeft ) {
                        Mix_PlayChannel(-1, death, 0);
                        lives -= 1;
                        if (lives == 0) {
                            missionInProgress = false;
                            break;
                        }
                        currentScene->Initialize();
                    }
                }
            }
            if (currentScene->state.player->position.y < -8) {
                Mix_PlayChannel(-1, fallWater, 0);
                lives -= 1;
                if (lives == 0) {
                    missionInProgress = false;
                    break;
                }
                currentScene->Initialize();
            }
        }



        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;


    viewMatrix = glm::mat4(1.0f);

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 8) {
        viewMatrix = glm::translate(viewMatrix,
            glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
    else if (currentScene->state.player->position.x > 8) {
        viewMatrix = glm::translate(viewMatrix,
            glm::vec3(-8, 3.75, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);



    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);
    if (!missionInProgress) {
        GLuint fontTextureID1 = Util::LoadTexture("font1.png");
        Util::DrawText(&program, fontTextureID1, "You Lose", 0.5f, -0.25f,
            glm::vec3(3, -2, 0));
    }
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {

    Mix_FreeChunk(fallWater);
    Mix_FreeMusic(music);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) SwitchToScene(SceneList[currentScene->state.nextScene]);
        Render();
    }

    Shutdown();
    return 0;
}