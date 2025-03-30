#include <iostream>
#include <ctime>
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

char levelData[MAP_HEIGHT][MAP_WIDTH] = {
    "##################",
    "#P F F#F E F# F E#",
    "#F###F######F###F#",
    "#F#F F#F EFF#F F #",
    "#F#F##F######F#F##",
    "#F FFF#FFF #FFF F#",
    "#####F#F##F#####F#",
    "#F E F#F F E F F #",
    "#F##F######F###F #",
    "#F#FF#F E#F F#F F#",
    "#F#E####F#F##FFF #",
    "#FFF EFFF FFEF FF#",
    "##################",
    "###F###F##F###F###"
};

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
                    if (abs(x - wallX) < PLAYER_SIZE && abs(y - wallY) < PLAYER_SIZE) {
                    return true;
                }

                }
            }
        }
    }
    return false;
}
// direction enemy.
struct Character {
    int x,y;
    int dx,dy;
    SDL_Texture* enemytexture;
};
vector<Character> enemies;
void changeDirection(Character &character) {
    int directions[4][2] = {{SPEED, 0}, {-SPEED, 0}, {0, SPEED}, {0, -SPEED}};
    int newDir = rand() % 4;
    character.dx = directions[newDir][0];
    character.dy = directions[newDir][1];
}
bool checkCollision(int x, int y) {
    int left   = x;
    int right  = x + CHARACTER_SIZE - 1;
    int top    = y;
    int bottom = y + CHARACTER_SIZE - 1;
    int leftCol   = left / TILE_SIZE;
    int rightCol  = right / TILE_SIZE;
    int topRow    = top / TILE_SIZE;
    int bottomRow = bottom / TILE_SIZE;

    if (leftCol < 0 || rightCol >= MAP_WIDTH || topRow < 0 || bottomRow >= MAP_HEIGHT) {
        return true;
    }
    for (int row = topRow; row <= bottomRow; ++row) {
        for (int col = leftCol; col <= rightCol; ++col) {
            if (levelData[row][col] == '#') {
                return true;
            }
        }
    }

    return false;
}
void updateEnemy() {
    for (auto &enemy : enemies) {
        int newX = enemy.x + enemy.dx;
        int newY = enemy.y + enemy.dy;

        if (checkCollision(newX, enemy.y) || checkCollision(enemy.x, newY) ||
            newX < 0 || newX + CHARACTER_SIZE > SCREEN_WIDTH ||
            newY < 0 || newY + CHARACTER_SIZE > SCREEN_HEIGHT)
        {
            changeDirection(enemy);
        } else {
            enemy.x = newX;
            enemy.y = newY;
        }
    }
}
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

// di chuyen theo huong cua player
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
// pha bang theo huong cua player
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
    if (newX >= 0 && newX < MAP_WIDTH * TILE_SIZE &&
        newY >= 0 && newY < MAP_HEIGHT * TILE_SIZE) {
        if (!isNearWall(newX, newY)) {
            playerX = newX;
            playerY = newY;
        }
    }
    if (levelData[newRow][newCol] == 'F') {
        levelData[newRow][newCol] = ' ';
        audio.playSound("eat");
        Mix_VolumeChunk(audio.getChunk("eat"), 110);
        if (checkWinCondition()) {
            gameWin = true;
        }
    }
    SDL_Rect playerRect = {(int)playerX + (TILE_SIZE - PLAYER_SIZE) / 2,(int)playerY + (TILE_SIZE - PLAYER_SIZE) / 2,PLAYER_SIZE,PLAYER_SIZE};
    for (auto& enemy : enemies) {
        SDL_Rect enemyRect = {enemy.x, enemy.y, CHARACTER_SIZE, CHARACTER_SIZE};
        if (SDL_HasIntersection(&playerRect, &enemyRect)) {
            gameOver = true;
            break;
        }
    }


    playerCol = playerX / TILE_SIZE;
    playerRow = playerY / TILE_SIZE;
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
    int offset = (TILE_SIZE - PLAYER_SIZE) / 2;
    SDL_Rect playerRect = {0, 0, PLAYER_SIZE, PLAYER_SIZE};
    graphics->blitRect(player, &playerRect, (int)playerX + offset, (int)playerY + offset);
    for (auto& e : enemies) {
    graphics->renderTexture(enemy, (int)e.x, (int)e.y);
}
}
bool isMouseClickedInRect(SDL_Event& e, const SDL_Rect& rect) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;

        return (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                mouseY >= rect.y && mouseY <= rect.y + rect.h);
    }
    return false;
}
void resetGame() {
    playerRow = 1;
    playerCol = 1;
    playerX = playerCol * TILE_SIZE;
    playerY = playerRow * TILE_SIZE;
    lastDirection = 0;
    velocityX = 0;
    velocityY = 0;
    gameOver = false;
    breakIce = false;
    gameWin = false;
    gameOverSoundPlayed = false;
    bottom_startSound = false;
    lastBreakTime = 0;
   const char originalMap[MAP_HEIGHT][MAP_WIDTH] = {
    "##################",
    "#P F F#F E F# F E#",
    "#F###F######F###F#",
    "#F#F F#F EFF#F F #",
    "#F#F##F######F#F##",
    "#F FFF#FFF #FFF F#",
    "#####F#F##F#####F#",
    "#F E F#F F E F F #",
    "#F##F######F###F #",
    "#F#FF#F E#F F#F F#",
    "#F#E####F#F##FFF #",
    "#FFF EFFF FFEF FF#",
    "##################",
    "###F###F##F###F###",
};
    memcpy(levelData, originalMap, sizeof(levelData));
    enemies.clear();
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (levelData[row][col] == 'E') {
                int enemyX = col * TILE_SIZE + TILE_SIZE / 2 - CHARACTER_SIZE / 2;
                int enemyY = row * TILE_SIZE + TILE_SIZE / 2 - CHARACTER_SIZE / 2;

                if (!checkCollision(enemyX, enemyY)) {
                    Character enemy = {enemyX, enemyY, 0, 0};
                    changeDirection(enemy);
                    enemies.push_back(enemy);
                    levelData[row][col] = ' ';
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(0));
    Graphics graphics;
    graphics.init();
    Audio audio;
    audio.init();

    // Tải hình ảnh
    SDL_Texture* background = graphics.loadTexture("assets/images/background_start.JPG");
    SDL_Texture* icecreamtitle = graphics.loadTexture("assets/images/icecreamtitle.PNG");
    SDL_Texture* startgame = graphics.loadTexture("assets/images/startgame.PNG");
    SDL_Texture* backgroundPlay = graphics.loadTexture("assets/images/background_play.PNG");
    SDL_Texture* icetexture = graphics.loadTexture("assets/images/ice.PNG");
    SDL_Texture* fruittexture = graphics.loadTexture("assets/images/fruit.PNG");
    SDL_Texture* playertexture = graphics.loadTexture("assets/images/player.PNG");
    SDL_Texture* enemytexture = graphics.loadTexture("assets/images/enemy.PNG");
    SDL_Texture* menutexture = graphics.loadTexture("assets/images/menu.PNG");
    gameOverTexture = graphics.loadTexture("assets/images/gameover.JPG");
    winTexture = graphics.loadTexture("assets/images/win.JPG");

    // Âm thanh
    audio.loadMusic("assets/music/music.mp3");
    audio.loadSound("lose", "assets/sounds/lost.wav");
    audio.loadSound("eat", "assets/sounds/eat.wav");
    audio.loadSound("bottom_start", "assets/sounds/bottom_start.wav");
    audio.loadSound("break_ice", "assets/sounds/break_ice.wav");

    enum GameState { MENU, PLAYING, WIN, GAMEOVER };
    GameState currentState = MENU;
    SDL_Rect startButton = {370, 450, 200, 80};
    SDL_Rect menuButton = {370, 420, 180, 60};
    setupFruitTimer();
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (levelData[row][col] == 'E') {
                int enemyX = col * TILE_SIZE + TILE_SIZE / 2 - CHARACTER_SIZE / 2;
                int enemyY = row * TILE_SIZE + TILE_SIZE / 2 - CHARACTER_SIZE / 2;

                if (!checkCollision(enemyX, enemyY)) {
                    Character enemy = {enemyX, enemyY, 0, 0};
                    changeDirection(enemy);
                    enemies.push_back(enemy);
                    levelData[row][col] = ' ';
                }
            }
        }
    }

    bool running = true;
    while (running) {
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (currentState == MENU && event.type == SDL_MOUSEBUTTONDOWN) {
            if (isMouseClickedInRect(event, startButton)) {
                audio.playSound("bottom_start");
                currentState = PLAYING;
                audio.playMusic();
                Mix_VolumeMusic(40);
            }
        }
        if (currentState == GAMEOVER && event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Rect yesButton1 = {270, 520, 180, 60};
            SDL_Rect yesButton2 = {540, 520, 180, 60};
            if (isMouseClickedInRect(event, yesButton1) || isMouseClickedInRect(event, yesButton2)) {
                audio.playSound("bottom_start");
                currentState = MENU;
                audio.playMusic();
                Mix_VolumeMusic(40);
                gameOver = false;
                gameOverSoundPlayed = false;
                resetGame();
            }
        }
        if (currentState == WIN && event.type == SDL_MOUSEBUTTONDOWN) {
            if (isMouseClickedInRect(event, menuButton)) {
                audio.playSound("bottom_start");
                currentState = MENU;
                audio.playMusic();
                Mix_VolumeMusic(40);
                gameWin = false;
                gameOverSoundPlayed = false;
                resetGame();
            }
        }
        handleInput(event, audio);
    }
        if (currentState == MENU) {
            graphics.prepareScene(background);
            graphics.renderTexture(icecreamtitle, 150, 50);
            graphics.renderTexture(startgame, startButton.x, startButton.y);
            graphics.presentScene();
        }
        else if (currentState == PLAYING) {
            updateEnemy();
            updatePlayer(running, audio);
            graphics.prepareScene(backgroundPlay);
            drawLevel(&graphics, icetexture, fruittexture, enemytexture, playertexture);
            graphics.presentScene();

            if (gameOver) {
                if (!gameOverSoundPlayed) {
                    audio.stopMusic();
                    audio.playSound("lose");
                    Mix_VolumeChunk(audio.getChunk("lose"), 100);
                    gameOverSoundPlayed = true;
                    currentState = GAMEOVER;
                }
            }
            if (gameWin) {
                if (!gameOverSoundPlayed) {
                    audio.stopMusic();
                    audio.playSound("lose");
                    Mix_VolumeChunk(audio.getChunk("lose"), 100);
                    gameOverSoundPlayed = true;
                    currentState = WIN;
                }
            }
        }
        else if (currentState == GAMEOVER) {
            graphics.prepareScene(gameOverTexture);
            graphics.presentScene();
        }
        else if (currentState == WIN) {
            graphics.prepareScene(winTexture);
            graphics.renderTexture(menutexture, menuButton.x, menuButton.y);
            graphics.presentScene();
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
