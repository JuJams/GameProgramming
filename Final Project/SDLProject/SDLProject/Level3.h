#include "Scene.h"
#include <SDL_mixer.h>


class Level3 : public Scene {
public:
    void Initialize(int numLives) override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
