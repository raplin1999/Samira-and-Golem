#include "Menu.h"


void Menu::Initialize() {
    state.nextScene = -1;
    state.player = new Entity();

}
void Menu::Update(float deltaTime) {
    
}
void Menu::Render(ShaderProgram* program) {

   GLuint fontTextureID = Util::LoadTexture("font1.png");
   Util::DrawText(program, fontTextureID, "Samira and Golems", 0.5f, -0.25f,
        glm::vec3(3, -2, 0));


   Util::DrawText(program, fontTextureID, "Press Enter To Start", 0.5f, -0.25f,
       glm::vec3(3, -4, 0));
}