#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "defs.h"
#include "graphics.h"

using namespace std;

char levelData[MAP_HEIGHT][MAP_WIDTH] = {
    "##########",
    "#    F   #",
    "#  ###   #",
    "#    #   #",
    "# P  # E #",
    "#    #   #",
    "#    F   #",
    "##########"
};


void drawLevel(Graphics* graphics,SDL_Texture* ice ,SDL_Texture* fruit,SDL_Texture* enemy, SDL_Texture* player) {
    int tileSize = 50;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect rect = {x * tileSize, y * tileSize, tileSize, tileSize};

            SDL_Texture* texture = NULL;

            switch (levelData[y][x]) {
                case '#': texture = ice; break;
                case 'F': texture = fruit; break;
                case 'E': texture = enemy; break;
                case 'P': texture = player; break;
            }

            if (texture != NULL) {
                graphics->renderTexture( texture,rect.x,rect.y);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();

    SDL_Texture *background = graphics.loadTexture("background_start.JPG");
    SDL_Texture *icecreamtitle = graphics.loadTexture("icecreamtitle.PNG");
    SDL_Texture *startgame = graphics.loadTexture("startgame.PNG");
    SDL_Texture *backgroundPlay = graphics.loadTexture("background_play.PNG");


    SDL_Texture *ice = graphics.loadTexture("ice.PNG");
    SDL_Texture *fruit = graphics.loadTexture("fruit.PNG");
    SDL_Texture *enemy = graphics.loadTexture("enemy.PNG");
    SDL_Texture *player = graphics.loadTexture("player.PNG");




     bool running = true;
    int gameState = 0;
    SDL_Rect startButton = {380, 450, 200, 80};

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {
                    gameState = 1;
                }
            }
        }

        if (gameState == 0) {
            graphics.prepareScene(background);
            graphics.renderTexture(icecreamtitle, 150, 50);
            graphics.renderTexture(startgame, startButton.x, startButton.y);
            graphics.presentScene();
        } else if (gameState == 1) {
            graphics.prepareScene(backgroundPlay);
            drawLevel(&graphics, ice, fruit,enemy,player);
            graphics.presentScene();
        }
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(icecreamtitle);
    SDL_DestroyTexture(startgame);

    graphics.quit();
    return 0;
}

