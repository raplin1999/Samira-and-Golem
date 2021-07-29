#include "GameOver.h"


void GameOver::Initialize() {
    state.nextScene = -1;
    state.player = new Entity();

}
void GameOver::Update(float deltaTime) {

}
void GameOver::Render(ShaderProgram* program) {

    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "You Won", 0.5f, -0.25f,
        glm::vec3(4, -3, 0));

}