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
// fruit
Uint32 moveFruit(Uint32 interval, void* param) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (levelData[row][col] == 'F') { // Nếu tìm thấy trái cây
                int newRow = row, newCol = col;
                int direction = rand() % 4;

                switch (direction) {
                    case 0: newRow--; break; // Lên
                    case 1: newRow++; break; // Xuống
                    case 2: newCol--; break; // Trái
                    case 3: newCol++; break; // Phải
                }

                // Kiểm tra xem vị trí mới có hợp lệ không
                if (newRow >= 0 && newRow < MAP_HEIGHT && newCol >= 0 && newCol < MAP_WIDTH &&
                    levelData[newRow][newCol] == ' ') {
                    // Di chuyển trái cây
                    levelData[row][col] = ' ';   // Xóa vị trí cũ
                    levelData[newRow][newCol] = 'F'; // Đặt vào vị trí mới
                }
            }
        }
    }
    return interval; // Giữ chu kỳ di chuyển
}
void setupFruitTimer() {
    SDL_AddTimer(5000, moveFruit, nullptr); // Không cần truyền &fruitX nữa
}
// player
int playerRow = 1, playerCol = 1;
float playerX = playerCol * TILE_SIZE, playerY = playerRow * TILE_SIZE;
float velocityX = 0, velocityY = 0;
bool gameOver =  false;
bool breakIce = false;
bool placeIce = false;

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
            case SDLK_SPACE: breakIce = true; break;
            case SDLK_b: placeIce = true; break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_UP: case SDLK_DOWN: velocityY = 0; break;
            case SDLK_LEFT: case SDLK_RIGHT: velocityX = 0; break;
            case SDLK_SPACE: breakIce = false; break;
            case SDLK_b: placeIce = false; break; // Reset placeIce khi thả phím b
        }
    }
}
void updatePlayer(bool &running) {
    if (gameOver) return;

    float newX = playerX + velocityX;
    float newY = playerY + velocityY;
    int newCol = newX / TILE_SIZE;
    int newRow = newY / TILE_SIZE;

    // Kiểm tra hướng nhân vật đang đối diện băng
    int checkCol = playerX / TILE_SIZE;
    int checkRow = playerY / TILE_SIZE;

    if (velocityX > 0) checkCol++; // Nếu đi sang phải
    if (velocityX < 0) checkCol--; // Nếu đi sang trái
    if (velocityY > 0) checkRow++; // Nếu đi xuống
    if (velocityY < 0) checkRow--; // Nếu đi lên

    if (breakIce && levelData[checkRow][checkCol] == '#') {
        levelData[checkRow][checkCol] = ' '; // Phá băng ngay trước mặt
    }

    if (placeIce) {
        int iceRow = checkRow; // Sử dụng checkRow thay vì playerRow
        int iceCol = checkCol; // Sử dụng checkCol thay vì playerCol

        // Kiểm tra giới hạn bản đồ
        if (iceRow >= 0 && iceRow < MAP_HEIGHT && iceCol >= 0 && iceCol < MAP_WIDTH) {
            // Chỉ tạo băng nếu ô phía trước là ô trống
            if (levelData[iceRow][iceCol] == ' ') {
                levelData[iceRow][iceCol] = '#';
                placeIce = false; // Reset placeIce sau khi tạo băng
            }
        }
    }

    // Di chuyển nhân vật nếu không va chạm
    if (!isNearWall(newX, newY)) {
        playerX = newX;
        playerY = newY;
    }

    // Xử lý ăn trái cây hoặc thua game
    if (levelData[newRow][newCol] == 'F') {
        levelData[newRow][newCol] = ' ';
    } else if (levelData[newRow][newCol] == 'E') {
        gameOver = true;
    }
    playerCol = playerX / TILE_SIZE;
    playerRow = playerY / TILE_SIZE;
}


// ve map
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

    // Vẽ nhân vật
    graphics->renderTexture(player, (int)playerX, (int)playerY);

    // Vẽ màn hình Game Over nếu cần
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

    setupFruitTimer();
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
