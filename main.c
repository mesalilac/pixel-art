#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH            800
#define HEIGHT           800
#define BACKGROUND_COLOR 28, 28, 28, 255

#define GRID_MIN_WIDTH  0
#define GRID_MIN_HEIGHT 0
#define GRID_MAX_WIDTH  WIDTH
#define GRID_MAX_HEIGHT HEIGHT

#define CELL_SIZE  20
#define CELL_COLOR 32, 32, 32, 255

#define MAX_CELLS (GRID_MAX_WIDTH / CELL_SIZE) * (GRID_MAX_HEIGHT / CELL_SIZE)

/*
    Cell grid[WIDTH / CELL_SIZE][HEIGHT / CELL_SIZE];
    int w_size;
    int h_size;

    printf("Building grid\n");
    for (int i = 0; i < WIDTH; i += CELL_SIZE)
    {
        for (int j = 0; j < WIDTH; j += CELL_SIZE)
        {
            Cell cell                          = {.x = i, .y = j};
            grid[i / CELL_SIZE][j / CELL_SIZE] = cell;
            printf("cell: row: %i, col: %i\n", i / CELL_SIZE, j / CELL_SIZE);
        }
        w_size++;
        h_size++;
    }
    printf("Done\n");

    printf("w_size: %i, h_size: %i\n", w_size, h_size);

    for (int i = 0; i < w_size; ++i)
    {
        for (int j = 0; j < h_size; ++j)
        {
            printf("#");
        }
        printf("\n");
    }
*/

typedef struct
{
    int x;
    int y;
} Cell;

typedef struct
{
    Cell list[MAX_CELLS];
    int size;
} Cells;

void build_grid(Cells *cells)
{
    for (int i = GRID_MIN_WIDTH; i < GRID_MAX_WIDTH; i += CELL_SIZE)
    {
        for (int j = GRID_MIN_HEIGHT; j < GRID_MAX_WIDTH; j += CELL_SIZE)
        {
            Cell cell                = {.x = i, .y = j};
            cells->list[cells->size] = cell;
            cells->size++;
        }
    }
}

void draw_grid(SDL_Renderer *ren, Cells *cells)
{
    for (int i = 0; i < cells->size; ++i)
    {
        Cell cell = cells->list[i];

        SDL_Rect rect = {
            .x = cell.x, .y = cell.y, .w = CELL_SIZE, .h = CELL_SIZE};

        SDL_SetRenderDrawColor(ren, CELL_COLOR);
        SDL_RenderDrawRect(ren, &rect);
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(
            stderr, "ERROR: Failed to initalize SDL VIDEO: %s", SDL_GetError()
        );
        exit(1);
    }

    SDL_Window *win =
        SDL_CreateWindow("Grid", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL)
    {
        fprintf(stderr, "ERROR: Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL)
    {
        fprintf(stderr, "ERROR: Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }

    int index       = 0;
    bool is_running = true;
    SDL_Event event;
    bool saving = false;

    Cells cells = {.size = 0};
    build_grid(&cells);

    Cell *points    = malloc(MAX_CELLS * sizeof(Cell));
    int points_size = 0;

    SDL_Rect player = {
        .x = cells.list[index].x,
        .y = cells.list[index].y,
        .w = CELL_SIZE,
        .h = CELL_SIZE};

    while (is_running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    if (saving == true)
                    {
                        Cell cell = {.x = player.x, .y = player.y};

                        points[points_size] = cell;
                        points_size++;
                    }
                    // Move up
                    if (event.key.keysym.sym == 'k')
                    {
                        index--;
                        player.x = cells.list[index].x;
                        player.y = cells.list[index].y;
                        printf("index: %i\n", index);
                    }
                    // Move down
                    if (event.key.keysym.sym == 'j')
                    {
                        index++;
                        player.x = cells.list[index].x;
                        player.y = cells.list[index].y;
                        printf("index: %i\n", index);
                    }
                    // Move left
                    // FIXME: If GRID_MAX_WIDTH and GRID_MAX_HEIGHT are not 800
                    //       going left or right skips one cell
                    if (event.key.keysym.sym == 'h')
                    {
                        // TODO go to the right when out of bound
                        // index = index % (GRID_MAX_WIDTH/CELL_SIZE);
                        index = index - (GRID_MAX_WIDTH / CELL_SIZE);

                        player.x = cells.list[index].x;
                        player.y = cells.list[index].y;
                        printf("index: %i\n", index);
                    }
                    // Move right
                    if (event.key.keysym.sym == 'l')
                    {
                        index = index + (GRID_MAX_WIDTH / CELL_SIZE);

                        player.x = cells.list[index].x;
                        player.y = cells.list[index].y;
                        printf("index: %i\n", index);
                    }
                    if (event.key.keysym.sym == 's')
                    {
                        Cell cell = {.x = player.x, .y = player.y};

                        points[points_size] = cell;
                        points_size++;
                        saving = true;
                    }
                    if (event.key.keysym.sym == 'c')
                        points_size = 0;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == 's')
                        saving = false;
                    break;
            }
        }

        int mouse_x, mouse_y;
        Uint32 buttons   = SDL_GetMouseState(&mouse_x, &mouse_y);
        SDL_Point cursor = {mouse_x, mouse_y};

        for (int i = 0; i < cells.size; ++i)
        {
            Cell cell = cells.list[i];

            SDL_Rect rect = {
                .x = cell.x, .y = cell.y, .w = CELL_SIZE, .h = CELL_SIZE};

            if (SDL_PointInRect(&cursor, &rect) &&
                (buttons & SDL_BUTTON_LMASK) != 0)
            {
                points[points_size] = cell;
                points_size++;
            }
        }

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderClear(ren);

        draw_grid(ren, &cells);

        for (int i = 0; i < points_size; ++i)
        {
            Cell point = points[i];

            SDL_Rect rect = {
                .x = point.x, .y = point.y, .w = CELL_SIZE, .h = CELL_SIZE};

            SDL_SetRenderDrawColor(ren, 200, 200, 0, 255);
            SDL_RenderFillRect(ren, &rect);
        }

        SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
        SDL_RenderFillRect(ren, &player);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}
