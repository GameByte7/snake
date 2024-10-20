//"Do not use this game without my permission."  :D
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
int score = 0;  // 

#define win_width 300 //
#define win_height 300 // 
#define cell_size 20 //

SDL_Rect snake_rect[2 + (win_width * win_height) / cell_size]; // 
int snake_len = 5;  //


int direction_x = cell_size;  //
int direction_y = 0;

bool quit = false;    
bool gameOver = false;  

void DeInit(int error)
{
    if (ren != NULL) SDL_DestroyRenderer(ren);
    if (win != NULL) SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    exit(error);
}

void Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        system("pause");
        DeInit(1);
    }

    win = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (win == NULL)
    {
        printf("Could not create window: %s\n", SDL_GetError());
        system("pause");
        DeInit(1);
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL)
    {
        printf("Could not create renderer: %s\n", SDL_GetError());
        system("pause");
        DeInit(1);
    }

    if (TTF_Init() == -1)
    {
        printf("Could not initialize TTF: %s\n", TTF_GetError());
        DeInit(1);
    }
}

SDL_Texture* loadTextureFromFile(const char* filename, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL)
    {
        printf("Could not load image: %s\n", SDL_GetError());
        system("pause");
        DeInit(1);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void shift_parts_1()
{
    for (int i = snake_len; i > 0; i--)
    {
        snake_rect[i].x = snake_rect[i - 1].x;
        snake_rect[i].y = snake_rect[i - 1].y;
    }
}

void teleport()
{
    if (snake_rect[0].x >= win_width)
    {
        snake_rect[0].x = 0;
    }
    else if (snake_rect[0].x < 0)
    {
        snake_rect[0].x = win_width - cell_size;
    }

    if (snake_rect[0].y >= win_height)
    {
        snake_rect[0].y = 0;
    }
    else if (snake_rect[0].y < 0)
    {
        snake_rect[0].y = win_height - cell_size;
    }
}

void renderScore(SDL_Renderer* ren, TTF_Font* font, int score)
{
    SDL_Color textColor = { 255, 255, 255 };
    char scoreText[50];
    sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    if (scoreSurface == NULL)
    {
        printf("Failed to render text surface: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(ren, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    if (scoreTexture == NULL)
    {
        printf("Failed to create text texture: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect scoreRect = { 10, 10, 100, 30 };  
    SDL_RenderCopy(ren, scoreTexture, NULL, &scoreRect);

    SDL_DestroyTexture(scoreTexture);
}

void generateApplePosition(int& apple_x, int& apple_y)
{
    apple_x = (rand() % (win_width / cell_size)) * cell_size;
    apple_y = (rand() % (win_height / cell_size)) * cell_size;
}

void check_collision()
{
    for (int i = snake_len; i > 0; i--)
    {
        if (snake_rect[0].x == snake_rect[i].x && snake_rect[0].y == snake_rect[i].y)
        {
            gameOver = true;  
        }
    }
}

void renderGameOverMessage(SDL_Renderer* renderer, TTF_Font* font)
{
    SDL_Color textColor = { 255, 255, 255 };  
    SDL_Surface* messageSurface = TTF_RenderText_Solid(font, "Try Again. Press Enter", textColor);
    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);

    SDL_Rect messageRect;
    messageRect.x = win_width / 2 - 150;
    messageRect.y = win_height / 2 - 50;
    messageRect.w = 300;
    messageRect.h = 40;

    SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
    SDL_FreeSurface(messageSurface);
    SDL_DestroyTexture(messageTexture);
}

void resetGame(int& apple_x, int& apple_y)
{
  
    snake_len = 5;
    direction_x = cell_size;  
    direction_y = 0;

   
    for (int i = 0; i <= (1 + (win_width * win_height) / cell_size); i++)
    {
        snake_rect[i].x = 0;
        snake_rect[i].y = 0;
        snake_rect[i].w = cell_size;
        snake_rect[i].h = cell_size;
    }


    generateApplePosition(apple_x, apple_y);

  
    score = 0;
    gameOver = false;
}

void gameOverLoop(SDL_Renderer* renderer, TTF_Font* font, int& apple_x, int& apple_y)
{
    SDL_Event event;
    bool waiting = true;

    while (waiting)
    {
      
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                waiting = false;
                quit = true;  
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
            {
                waiting = false;  
                resetGame(apple_x, apple_y);  
            }
        }

        SDL_RenderClear(renderer);
        renderGameOverMessage(renderer, font);  
        SDL_RenderPresent(renderer);
        SDL_Delay(100);  
    }
}

int main(int argc, char* args[])
{
    Init(); 

    TTF_Font* font = TTF_OpenFont("impact.ttf", 25); 
    if (font == NULL)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        DeInit(1);
    }

    int apple_x = 0;
    int apple_y = 0;
    generateApplePosition(apple_x, apple_y);

    resetGame(apple_x, apple_y);  

    SDL_Texture* snakehead_tex = loadTextureFromFile("snakehead.bmp", ren);
    SDL_Texture* back_tex = loadTextureFromFile("back_snake.jpg", ren);
    SDL_Texture* player_tex = loadTextureFromFile("snake5.bmp", ren);
    SDL_Texture* apple_tex = loadTextureFromFile("apple.bmp", ren);

    SDL_Event event;

    while (!quit)  
    {
        if (!gameOver) 
        {
          
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
                        if (direction_y == 0) {  
                            direction_x = 0;
                            direction_y = -cell_size;
                        }
                        break;
                    case SDLK_DOWN:
                        if (direction_y == 0) {
                            direction_x = 0;
                            direction_y = cell_size;
                        }
                        break;
                    case SDLK_LEFT:
                        if (direction_x == 0) {
                            direction_x = -cell_size;
                            direction_y = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (direction_x == 0) {
                            direction_x = cell_size;
                            direction_y = 0;
                        }
                        break;
                    }
                }
            }

           
            shift_parts_1();
            snake_rect[0].x += direction_x;
            snake_rect[0].y += direction_y;
            teleport();

            check_collision();  

            
            for (int i = 0; i < snake_len; i++)
            {
                if (snake_rect[i].x == apple_x && snake_rect[i].y == apple_y)
                {
                    snake_len++;
                    score += 10;
                    generateApplePosition(apple_x, apple_y);
                }
            }

          
            SDL_RenderClear(ren);

            SDL_Rect back_rect = { 0, 0, win_width, win_height };
            SDL_RenderCopy(ren, back_tex, NULL, &back_rect);

           
            for (int i = 0; i < snake_len; i++)
            {
                if (i == 0)
                {
                    SDL_RenderCopy(ren, snakehead_tex, NULL, &snake_rect[i]);
                }
                else
                {
                    SDL_RenderCopy(ren, player_tex, NULL, &snake_rect[i]);
                }
            }

            SDL_Rect apple_rect = { apple_x, apple_y, cell_size, cell_size };
            SDL_RenderCopy(ren, apple_tex, NULL, &apple_rect);

            renderScore(ren, font, score);

            SDL_RenderPresent(ren);

            SDL_Delay(150);  
        }
        else 
        {
            
            gameOverLoop(ren, font, apple_x, apple_y);
        }
    }

    SDL_DestroyTexture(player_tex);
    SDL_DestroyTexture(back_tex);
    SDL_DestroyTexture(snakehead_tex);
    SDL_DestroyTexture(apple_tex);
    TTF_CloseFont(font);
    DeInit(0);

    return 0;
}
