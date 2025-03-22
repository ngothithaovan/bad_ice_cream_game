#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include "graphics.h"

using namespace std;
//1 la tuong co dinh
//0 la duong di
//2 la tuong bang co the pha
//3 la trai cay
//4 la nhan vat
int gameMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 1},
    {1, 2, 0, 2, 0, 1, 1, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 4, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 2, 0, 2, 0, 1, 1, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 3, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 1},
    {1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 1},
    {1, 2, 0, 2, 0, 1, 1, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 4, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

void drawMap(SDL_Renderer* renderer, SDL_Texture* tileset) {
    SDL_Rect srcRect, dstRect;
    srcRect.w = srcRect.h = TILE_SIZE;
    dstRect.w = dstRect.h = TILE_SIZE;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            srcRect.x = gameMap[y][x] * TILE_SIZE;
            srcRect.y = 0;
            dstRect.x = x * TILE_SIZE;
            dstRect.y = y * TILE_SIZE;
            SDL_RenderCopy(renderer, tileset, &srcRect, &dstRect);
        }
    }
}

bool checkMouse(Graphics &graphics, SDL_Rect click) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;
            if (graphics.checkClick(mouseX, mouseY, click)) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();

    SDL_Texture *background = graphics.loadTexture("background_start.JPG");
    SDL_Texture *icecreamtitle = graphics.loadTexture("icecreamtitle.PNG");
    SDL_Texture *startgame = graphics.loadTexture("startgame.PNG");
    SDL_Texture *backgroundPlay = graphics.loadTexture("background_play.PNG");
    SDL_Texture *ice = graphics.loadTexture("ice.PNG");
    bool running = true;
    int gameState = 0;
    SDL_Rect startButton = {380, 450, 200, 80};
    while (running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        if (gameState == 0) {
            graphics.prepareScene(background);
            graphics.renderTexture(icecreamtitle, 150, 50);
            graphics.renderTexture(startgame, startButton.x, startButton.y);
            graphics.presentScene();
            if(checkMouse(graphics,startButton)) {gameState=1;}
        }
        else if(gameState==1)
        {
            graphics.prepareScene(backgroundPlay);
            graphics.renderTexture(ice, 0, 0);
            graphics.presentScene();
        }
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(icecreamtitle);
    SDL_DestroyTexture(startgame);

    graphics.quit();
    return 0;
}

