#ifndef GRAPHICS__H
#define GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
struct Graphics{


    SDL_Window *window;
    SDL_Renderer *renderer;

    void logErrorAndExit(const char* msg, const char* error);
    void init();
    void prepareScene(SDL_Texture *background);
    void presentScene();
    SDL_Texture *loadTexture(const char*filename);
    void renderTexture(SDL_Texture *texture,int x, int y);
    void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);
    void waitUntilKeyPressed();
    void quit();
    bool checkClick(int x,int y,SDL_Rect bottom);
};

#endif // GRAPHICS__H
