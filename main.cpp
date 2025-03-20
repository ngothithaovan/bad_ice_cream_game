#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include "graphics.h"

using namespace std;
//HELLO

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_PollEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}
int main(int argc , char* argv[])
{
    Graphics graphics;
    graphics.init();
    SDL_Texture *background = graphics.loadTexture("bikiniBottom.JPG");
    graphics.prepareScene(background);
    graphics.presentScene();
    waitUntilKeyPressed();

    SDL_Texture *spongeBob = graphics.loadTexture("Spongebob.PNG");
    graphics.renderTexture(spongeBob,300,200);
    graphics.presentScene();
    waitUntilKeyPressed();

    SDL_DestroyTexture(spongeBob);
    spongeBob=NULL;
    SDL_DestroyTexture(background);
    background=NULL;
    graphics.quit();

    return 0;
}
