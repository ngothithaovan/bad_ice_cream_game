/*
# : Tường

P : Người chơi

E : Kẻ thù

F : Trái cây
*/
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "defs.h"
#include "graphics.h"

using namespace std;

char levelData[MAP_HEIGHT][MAP_WIDTH] = {
    "##################",
    "#P  F   #    F   #",
    "#  #### #  ####  #",
    "#  #   F   #  # F#",
    "#F #  ###### F#  #",
    "#  F  #F      #F #",
    "##### #  ######  #",
    "#     #    E     #",
    "#  ########### F #",
    "#  #  E  #   #   #",
    "#  ####  #  ###  #",
    "# F   E     F    #",
    "##################"
};

int playerRow = 1, playerCol = 1;
float playerX = playerCol * TILE_SIZE, playerY = playerRow * TILE_SIZE;
float velocityX = 0, velocityY = 0;
bool gameOver =  false;
bool breakIce = false;
SDL_Texture* gameOverTexture = nullptr;

bool isNearWall(float x, float y) {
    int col = x / TILE_SIZE;
    int row = y / TILE_SIZE;
    if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT) return true;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int checkRow = row + dy;
            int checkCol = col + dx;
            if (checkRow >= 0 && checkRow < MAP_HEIGHT && checkCol >= 0 && checkCol < MAP_WIDTH) {
                if (levelData[checkRow][checkCol] == '#') {
                    float wallX = checkCol * TILE_SIZE;
                    float wallY = checkRow * TILE_SIZE;
                    if (abs(x - wallX) < 50 && abs(y - wallY) < 50) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP: velocityY = -playerSpeed; break;
            case SDLK_DOWN: velocityY = playerSpeed; break;
            case SDLK_LEFT: velocityX = -playerSpeed; break;
            case SDLK_RIGHT: velocityX = playerSpeed; break;
            case SDLK_SPACE:
                int frontRow = playerRow;
                int frontCol = playerCol;

                if (velocityX > 0) frontCol += 1; // Đang đi sang phải
                else if (velocityX < 0) frontCol -= 1; // Đang đi sang trái
                else if (velocityY > 0) frontRow += 1; // Đang đi xuống
                else if (velocityY < 0) frontRow -= 1; // Đang đi lên

                if (frontRow >= 0 && frontRow < MAP_HEIGHT && frontCol >= 0 && frontCol < MAP_WIDTH) {
                    if (levelData[frontRow][frontCol] == '#') {
                        levelData[frontRow][frontCol] = ' ';
                    }
                }
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_UP: case SDLK_DOWN: velocityY = 0; break;
            case SDLK_LEFT: case SDLK_RIGHT: velocityX = 0; break;
        }
    }
}

void updatePlayer(bool &running) {
    if (gameOver) return;

    float newX = playerX + velocityX;
    float newY = playerY + velocityY;
    int newCol = newX / TILE_SIZE;
    int newRow = newY / TILE_SIZE;

    if (!isNearWall(newX, newY)) {
        playerX = newX;
        playerY = newY;
    }

    if (levelData[newRow][newCol] == 'F') {
        levelData[newRow][newCol] = ' ';
    } else if (levelData[newRow][newCol] == 'E') {
        gameOver = true;
    }
}
void drawLevel(Graphics* graphics, SDL_Texture* ice, SDL_Texture* fruit, SDL_Texture* enemy, SDL_Texture* player) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_Texture* texture = NULL;
            switch (levelData[y][x]) {
                case '#': texture = ice; break;
                case 'F': texture = fruit; break;
                case 'E': texture = enemy; break;
            }
            if (texture != NULL) {
                graphics->renderTexture(texture, rect.x, rect.y);
            }
        }
    }
    graphics->renderTexture(player, (int)playerX, (int)playerY);
    if (gameOver && gameOverTexture) {
        graphics->renderTexture(gameOverTexture, 200, 200);
    }
}

int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();

    SDL_Texture *background = graphics.loadTexture("background_start.JPG");
    SDL_Texture *icecreamtitle = graphics.loadTexture("icecreamtitle.PNG");
    SDL_Texture *startgame = graphics.loadTexture("startgame.PNG");
    SDL_Texture *backgroundPlay = graphics.loadTexture("background_play.PNG");
    SDL_Texture *icetexture = graphics.loadTexture("ice.PNG");
    SDL_Texture *fruittexture = graphics.loadTexture("fruit.PNG");
    SDL_Texture *enemytexture = graphics.loadTexture("enemy.PNG");
    SDL_Texture *playertexture = graphics.loadTexture("player.PNG");
    gameOverTexture = graphics.loadTexture("gameover.JPG");

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
            handleInput(event);
        }

        if (gameState == 0) {
            graphics.prepareScene(background);
            graphics.renderTexture(icecreamtitle, 150, 50);
            graphics.renderTexture(startgame, startButton.x, startButton.y);
            graphics.presentScene();
        } else if (gameState == 1) {
            updatePlayer(running);
            graphics.prepareScene(backgroundPlay);
            drawLevel(&graphics, icetexture, fruittexture, enemytexture, playertexture);
            graphics.presentScene();
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(icecreamtitle);
    SDL_DestroyTexture(startgame);
    SDL_DestroyTexture(gameOverTexture);
    graphics.quit();
    return 0;
}
