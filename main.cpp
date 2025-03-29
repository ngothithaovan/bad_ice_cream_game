#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "defs.h"
#include "graphics.h"
#include "audio.h"

using namespace std;

// Biến toàn cục
int playerRow = 1, playerCol = 1;
float playerX = playerCol * TILE_SIZE, playerY = playerRow * TILE_SIZE;
int lastDirection = 0;
float velocityX = 0, velocityY = 0;
bool gameOver = false;
bool breakIce = false;
bool gameWin = false;
bool gameOverSoundPlayed = false;
bool bottom_startSound = false;
Uint32 lastBreakTime = 0;


SDL_Texture* winTexture = nullptr;
SDL_Texture* gameOverTexture = nullptr;
struct Enemy {
    float x, y;
    int direction;
    float speed = 3.5f;
};

vector<Enemy> enemies;

char levelData[MAP_HEIGHT][MAP_WIDTH] = {
    "##################",
    "#P   F #  E  #  E#",
    "# ### ###### ### #",
    "# #  F#   E  #F  #",
    "# # ## ###### # ##",
    "# F   # F  #  F  #",
    "##### # ## ##### #",
    "#  E  # F   E   F#",
    "# ## ###### ###  #",
    "# # F#  E #F  #  #",
    "# #E #### # ## F #",
    "#F    E F    E   #",
    "##################"
};

//check win
bool checkWinCondition() {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (levelData[row][col] == 'F') {
                return false;
            }
        }
    }
    return true;
}

// di chuyen theo huong
void handleMovement(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:    lastDirection = 0; break;
        case SDLK_DOWN:  lastDirection = 1; break;
        case SDLK_LEFT:  lastDirection = 2; break;
        case SDLK_RIGHT: lastDirection = 3; break;
        default: return;
    }
    velocityX = dx[lastDirection] * playerSpeed;
    velocityY = dy[lastDirection] * playerSpeed;
}
// pha bang theo huong
void breakIceBlock(Audio &audio) {
    int iceRow = playerY / TILE_SIZE + dy[lastDirection];
    int iceCol = playerX / TILE_SIZE + dx[lastDirection];

    if (iceRow >= 0 && iceRow < MAP_HEIGHT &&
        iceCol >= 0 && iceCol < MAP_WIDTH &&
        levelData[iceRow][iceCol] == '#') {

        levelData[iceRow][iceCol] = ' ';
        if (!breakIce) {
            audio.playSound("break_ice");
            Mix_VolumeChunk(audio.getChunk("break_ice"), 128 );
            breakIce = true;
        }
    }
}

//handleinput
void handleInput(SDL_Event& event,Audio &audio) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;

        if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) {
            handleMovement(key);
        }
        else if (key == SDLK_SPACE) {
            Uint32 now = SDL_GetTicks();
            if (!breakIce && now - lastBreakTime >= breakCooldown) {
                breakIceBlock(audio);
                breakIce = true;
                lastBreakTime = now;
            }
        }
    }
    else if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;

        if (key == SDLK_UP || key == SDLK_DOWN) {
            velocityY = 0;
        } else if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            velocityX = 0;
        } else if (key == SDLK_SPACE) {
            breakIce = false;
        }
    }
}

// Fruit move
Uint32 moveFruit(Uint32 interval, void* param) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (levelData[row][col] == 'F') {
                int newRow = row, newCol = col;
                int direction = rand() % 4;

                switch (direction) {
                    case 0: newRow--; break;
                    case 1: newRow++; break;
                    case 2: newCol--; break;
                    case 3: newCol++; break;
                }

                if (newRow >= 0 && newRow < MAP_HEIGHT && newCol >= 0 && newCol < MAP_WIDTH &&
                    levelData[newRow][newCol] == ' ') {
                    levelData[row][col] = ' ';
                    levelData[newRow][newCol] = 'F';
                }
            }
        }
    }
    return interval;
}

void setupFruitTimer() {
    SDL_AddTimer(5000, moveFruit, nullptr);
}
//move enemy
void updateEnemies() {
    for (auto& e : enemies) {
        int currentRow = e.y / TILE_SIZE;
        int currentCol = e.x / TILE_SIZE;

        int targetRow = currentRow + dy[e.direction];
        int targetCol = currentCol + dx[e.direction];

        float targetX = targetCol * TILE_SIZE;
        float targetY = targetRow * TILE_SIZE;

        if (targetRow >= 0 && targetRow < MAP_HEIGHT &&
            targetCol >= 0 && targetCol < MAP_WIDTH &&
            levelData[targetRow][targetCol] == ' ') {

            if (abs(e.x - targetX) > e.speed) e.x += (targetX > e.x) ? e.speed : -e.speed;
        if (abs(e.y - targetY) > e.speed) e.y += (targetY > e.y) ? e.speed : -e.speed;
        if (!(targetRow >= 0 && targetRow < MAP_HEIGHT && targetCol >= 0 && targetCol < MAP_WIDTH &&
              levelData[targetRow][targetCol] == ' ')) {
            e.direction = rand() % 4;  // Đổi hướng ngay
        }

        } else {
            e.direction = rand() % 4;
        }

        if (abs(e.x - playerX) < TILE_SIZE / 2 && abs(e.y - playerY) < TILE_SIZE / 2) {
            gameOver = true;
        }
    }
}



// Kiểm tra gần tường
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


// cap nhat
void updatePlayer(bool& running,Audio& audio) {
    if (gameOver) return;

    float newX = playerX + velocityX;
    float newY = playerY + velocityY;
    int newCol = newX / TILE_SIZE;
    int newRow = newY / TILE_SIZE;

    int checkCol = playerX / TILE_SIZE;
    int checkRow = playerY / TILE_SIZE;

    if (velocityX > 0) checkCol++;
    if (velocityX < 0) checkCol--;
    if (velocityY > 0) checkRow++;
    if (velocityY < 0) checkRow--;

    if (breakIce && levelData[checkRow][checkCol] == '#') {
        levelData[checkRow][checkCol] = ' ';
    }

    if (!isNearWall(newX, newY)) {
        playerX = newX;
        playerY = newY;
    }

    if (levelData[newRow][newCol] == 'F') {
        levelData[newRow][newCol] = ' ';
        audio.playSound("eat");
        Mix_VolumeChunk(audio.getChunk("eat"), 110);
        if (checkWinCondition()) {
            gameWin = true;
        }
    } else if (levelData[newRow][newCol] == 'E') {
        gameOver = true;
    }

    playerCol = playerX / TILE_SIZE;
    playerRow = playerY / TILE_SIZE;
}
//
void drawLevel(Graphics* graphics, SDL_Texture* ice, SDL_Texture* fruit, SDL_Texture* enemy, SDL_Texture* player) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int pixelX = x * TILE_SIZE;
            int pixelY = y * TILE_SIZE;

            switch (levelData[y][x]) {
                case '#':
                    graphics->renderTexture(ice, pixelX, pixelY);
                    break;
                case 'F':
                    graphics->renderTexture(fruit, pixelX, pixelY);
                    break;
                case 'E':
                    graphics->renderTexture(enemy, pixelX, pixelY);
                    break;
            }
        }
    }

    graphics->renderTexture(player, (int)playerX, (int)playerY);
    for (auto& e : enemies) {
    graphics->renderTexture(enemy, (int)e.x, (int)e.y);
}
    if (gameOver && gameOverTexture) {
        graphics->renderTexture(gameOverTexture, 200, 100);
    }
    if (gameWin && winTexture) {
    graphics->renderTexture(winTexture, 200, 100);
}
}
// MAIN
int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();
    Audio audio;
    audio.init();
    SDL_Texture* background = graphics.loadTexture("assets/images/background_start.JPG");
    SDL_Texture* icecreamtitle = graphics.loadTexture("assets/images/icecreamtitle.PNG");
    SDL_Texture* startgame = graphics.loadTexture("assets/images/startgame.PNG");
    SDL_Texture* backgroundPlay = graphics.loadTexture("assets/images/background_play.PNG");
    SDL_Texture* icetexture = graphics.loadTexture("assets/images/ice.PNG");
    SDL_Texture* fruittexture = graphics.loadTexture("assets/images/fruit.PNG");
    SDL_Texture* enemytexture = graphics.loadTexture("assets/images/enemy.PNG");
    SDL_Texture* playertexture = graphics.loadTexture("assets/images/player.PNG");
    gameOverTexture = graphics.loadTexture("assets/images/gameover.JPG");
    winTexture = graphics.loadTexture("assets/images/win.JPG");

    // am thanh
    audio.loadMusic("assets/music/music.mp3");
    audio.loadSound("lose", "assets/sounds/lost.wav");
    audio.loadSound("eat", "assets/sounds/eat.wav");
    audio.loadSound("bottom_start", "assets/sounds/bottom_start.wav");
    audio.loadSound("break_ice", "assets/sounds/break_ice.wav");
    int gameState = 0;
    SDL_Rect startButton = {380, 450, 200, 80};

    setupFruitTimer();
    bool running= true;
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
                    if(!bottom_startSound){
                        audio.playSound("bottom_start");
                        SDL_Delay(1000);
                        bottom_startSound= true;
                    }
                    gameState = 1;
                    audio.playMusic();
                    Mix_VolumeMusic(40);
                }
            }
            handleInput(event,audio);
        }

        if (gameState == 0) {
            graphics.prepareScene(background);
            graphics.renderTexture(icecreamtitle, 150, 50);
            graphics.renderTexture(startgame, startButton.x, startButton.y);
            graphics.presentScene();
        } else if (gameState == 1) {
            if (enemies.empty()) {
                for (int row = 0; row < MAP_HEIGHT; row++) {
                    for (int col = 0; col < MAP_WIDTH; col++) {
                        if (levelData[row][col] == 'E') {
                            Enemy e;
                            e.x = col * TILE_SIZE;
                            e.y = row * TILE_SIZE;
                            e.direction = rand() % 4;
                            enemies.push_back(e);
                            levelData[row][col] = ' ';
                        }
                    }
                }
}
            updatePlayer(running,audio);
            updateEnemies();
            graphics.prepareScene(backgroundPlay);
            drawLevel(&graphics, icetexture, fruittexture, enemytexture, playertexture);
            graphics.presentScene();
            if(gameOver)
            {
                if(!gameOverSoundPlayed){

                    audio.stopMusic();
                    audio.playSound("lose");
                    Mix_VolumeChunk(audio.getChunk("lose"), 100);
                    gameOverSoundPlayed=true;
                }
            }
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(icecreamtitle);
    SDL_DestroyTexture(startgame);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(winTexture);
    graphics.quit();
    audio.cleanup();

    return 0;
}
