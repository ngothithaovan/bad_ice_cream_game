#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "defs.h"
#include "graphics.h"


using namespace std;
/*
# : Tường

P : Người chơi

E : Kẻ thù

F : Trái cây
*/
char levelData[MAP_HEIGHT][MAP_WIDTH] = {
    "##################",
    "#P      #    F   #",
    "#  #### #  ####  #",
    "#  #       #  #  #",
    "#  #  ######  #  #",
    "#     #       #  #",
    "##### #  ######  #",
    "#     #    E     #",
    "#  ###########   #",
    "#  #  E  #   #   #",
    "#  ####  #  ###  #",
    "#     E     F    #",
    "##################"
};
int playerRow = 1, playerCol = 1;
int playerX = playerCol * TILE_SIZE, playerY = playerRow * TILE_SIZE;
// Biến để kiểm soát tốc độ di chuyển
Uint32 lastMoveTime = 0;  // Lưu thời gian lần di chuyển cuối

// kiem tra co the di chuyen hay khong
bool canMove(int newX, int newY) {
    int newRow = newY / TILE_SIZE;
    int newCol = newX / TILE_SIZE;

    // Kiểm tra không vượt giới hạn bản đồ
    if (newRow < 0 || newRow >= MAP_HEIGHT || newCol < 0 || newCol >= MAP_WIDTH) return false;

    // Kiểm tra có tường cản đường không
    return levelData[newRow][newCol] != '#';
}
// Xu ly hap phim di chuyen
void handleInput(SDL_Event& event) {
    Uint32 currentTime = SDL_GetTicks();  // Lấy thời gian hiện tại

    if (event.type == SDL_KEYDOWN && (currentTime - lastMoveTime >= moveCooldown)) {
       int newX = playerX, newY = playerY;

        switch (event.key.keysym.sym) {
            case SDLK_UP: newY --; break;
            case SDLK_DOWN: newY++; break;
            case SDLK_LEFT: newX--; break;
            case SDLK_RIGHT: newX++; break;
        }

        if (canMove(newX, newY)) {
            // Xóa nhân vật khỏi vị trí cũ
            levelData[playerRow][playerCol] = ' ';
            // Cập nhật vị trí mới
            playerX = newX;
            playerY = newY;
            levelData[playerRow][playerCol] = 'P';

            lastMoveTime = currentTime;  // Cập nhật thời gian di chuyển cuối
        }
    }
}
void drawLevel(Graphics* graphics,SDL_Texture* ice ,SDL_Texture* fruit,SDL_Texture* enemy, SDL_Texture* player) {

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect rect = {x * TILE_SIZE, y *TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_Texture* texture = NULL;

            switch (levelData[y][x]) {
                case '#': texture = ice; break;
                case 'F': texture = fruit; break;
                case 'E': texture = enemy; break;
            }

            if (texture != NULL) {
                graphics->renderTexture( texture,rect.x,rect.y);
            }
        }
    }
    graphics->renderTexture(player, playerX, playerY);
}

int main(int argc, char* argv[]) {
    Graphics graphics;
    //Player player(&graphics);
    graphics.init();

    SDL_Texture *background = graphics.loadTexture("background_start.JPG");
    SDL_Texture *icecreamtitle = graphics.loadTexture("icecreamtitle.PNG");
    SDL_Texture *startgame = graphics.loadTexture("startgame.PNG");
    SDL_Texture *backgroundPlay = graphics.loadTexture("background_play.PNG");

    //cac nhan vat trong game
    SDL_Texture *icetexture = graphics.loadTexture("ice.PNG");
    SDL_Texture *fruittexture = graphics.loadTexture("fruit.PNG");
    SDL_Texture *enemytexture = graphics.loadTexture("enemy.PNG");
    SDL_Texture *playertexture = graphics.loadTexture("player.PNG");




     bool running = true;
    int gameState = 0;
    SDL_Rect startButton = {380, 450, 200, 80};

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
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
            graphics.prepareScene(backgroundPlay);
            drawLevel(&graphics, icetexture, fruittexture,enemytexture,playertexture);
            graphics.presentScene();
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(icecreamtitle);
    SDL_DestroyTexture(startgame);

    graphics.quit();
    return 0;
}


