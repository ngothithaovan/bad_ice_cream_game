#ifndef GRAPHICS__H
#define GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
struct Graphics{
    //HELLO
//    SDL_Window *window = nullptr;
//    SDL_Renderer *renderer = nullptr;
//    void LogErrorAndExit(const char* msg,const char* error);
//    void initSDL (const int SCREEN_WIDTH,const int SCREEN_HEIGHT, const char* WINDOW_TITLE);
//    void quitSDL();
//    SDL_Texture *LoadTexture(const char *filename);
//    void renderTexture(SDL_Texture *texture,int x=0, int y=0, int w=100,int h=100);
//    void prepareSence();// chuan bi renderer
//    void presentSence();//hien thi renderer ra man hinh


    SDL_Window *window;
    SDL_Renderer *renderer;

    void logErrorAndExit(const char* msg, const char* error);
    void init();
    void prepareScene(SDL_Texture *background);
    void presentScene();
    SDL_Texture *loadTexture(const char*filename);
    void renderTexture(SDL_Texture *texture,int x, int y);
    void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);
    void quit();
};

#endif // GRAPHICS__H
