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

#define MAX_CELLS   (GRID_MAX_WIDTH / CELL_SIZE) * (GRID_MAX_HEIGHT / CELL_SIZE)
#define MAX_ROWS    GRID_MAX_HEIGHT / CELL_SIZE - 1
#define MAX_COLUMNS GRID_MAX_WIDTH / CELL_SIZE - 1

typedef struct
{
    int x;
    int y;
} Cell;

typedef struct
{
    Cell grid[GRID_MAX_WIDTH / CELL_SIZE][GRID_MAX_HEIGHT / CELL_SIZE];
    struct
    {
        int w;
        int h;
    } size;
} Cells;

typedef struct
{
    int row;
    int column;
} GridPos;

typedef struct
{
    GridPos grid_pos;
} Player;

bool point_exists(Cell **points, int *points_size, Cell *cell)
{
    for (int i = 0; i < *points_size; ++i)
    {
        if (points[i]->x == cell->x && points[i]->y == cell->y)
        {
            return true;
        }
    }

    return false;
}

void save_point(Cell **points, int *points_size, int x, int y)
{
    Cell *cell = malloc(sizeof(Cell));
    cell->x    = x;
    cell->y    = y;

    if (point_exists(points, points_size, cell) == false)
    {
        points[*points_size] = cell;
        *points_size += 1;
        printf("x: %i, y: %i\n", x, y);
    }
}

void build_grid(Cells *cells)
{
    for (int col = GRID_MIN_WIDTH; col < GRID_MAX_WIDTH; col += CELL_SIZE)
    {
        for (int row = GRID_MIN_WIDTH; row < GRID_MAX_WIDTH; row += CELL_SIZE)
        {
            Cell cell = {.x = col, .y = row};
            cells->grid[row / CELL_SIZE][col / CELL_SIZE] = cell;
        }
        cells->size.w++;
        cells->size.h++;
    }
}

void draw_grid(SDL_Renderer *ren, Cells *cells)
{
    for (int col = 0; col < cells->size.w; ++col)
    {
        for (int row = 0; row < cells->size.h; ++row)
        {
            Cell cell = cells->grid[row][col];

            SDL_Rect rect = {
                .x = cell.x, .y = cell.y, .w = CELL_SIZE, .h = CELL_SIZE};

            SDL_SetRenderDrawColor(ren, CELL_COLOR);
            SDL_RenderDrawRect(ren, &rect);
        }
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

    bool is_running = true;
    SDL_Event event;
    bool saving = false;

    Cells cells = {.size = 0};
    build_grid(&cells);

    Cell **points   = malloc(MAX_CELLS * sizeof(Cell));
    int points_size = 0;

    Player player = {
        .grid_pos = {.row = 0, .column = 0}
    };

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
                        save_point(
                            points,
                            &points_size,
                            cells
                                .grid[player.grid_pos.row]
                                     [player.grid_pos.column]
                                .x,

                            cells
                                .grid[player.grid_pos.row]
                                     [player.grid_pos.column]
                                .y
                        );
                    // Move up
                    if (event.key.keysym.sym == 'k')
                    {
                        if (player.grid_pos.row == 0)
                        {
                            player.grid_pos.row = MAX_ROWS;
                        }
                        else
                        {
                            player.grid_pos.row--;
                        }
                    }
                    // Move down
                    if (event.key.keysym.sym == 'j')
                    {
                        if (player.grid_pos.row == MAX_ROWS)
                        {
                            player.grid_pos.row = 0;
                        }
                        else
                        {
                            player.grid_pos.row++;
                        }
                    }
                    // Move left
                    // FIXME: If GRID_MAX_WIDTH and GRID_MAX_HEIGHT are not 800
                    //       going left or right skips one cell
                    if (event.key.keysym.sym == 'h')
                    {
                        if (player.grid_pos.column == 0)
                        {
                            player.grid_pos.column = MAX_COLUMNS;
                        }
                        else
                        {
                            player.grid_pos.column--;
                        }
                    }
                    // Move right
                    if (event.key.keysym.sym == 'l')
                    {
                        if (player.grid_pos.column == MAX_COLUMNS)
                        {
                            player.grid_pos.column = 0;
                        }
                        else
                        {
                            player.grid_pos.column++;
                        }
                    }
                    if (event.key.keysym.sym == 's')
                    {
                        save_point(
                            points,
                            &points_size,
                            cells
                                .grid[player.grid_pos.row]
                                     [player.grid_pos.column]
                                .x,

                            cells
                                .grid[player.grid_pos.row]
                                     [player.grid_pos.column]
                                .y
                        );
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

        printf(
            "player:r(%i)c(%i)\n", player.grid_pos.row, player.grid_pos.column
        );

        int mouse_x, mouse_y;
        Uint32 buttons   = SDL_GetMouseState(&mouse_x, &mouse_y);
        SDL_Point cursor = {mouse_x, mouse_y};

        for (int col = 0; col < cells.size.w; ++col)
        {
            for (int row = 0; row < cells.size.h; ++row)
            {
                Cell cell = cells.grid[row][col];

                SDL_Rect rect = {
                    .x = cell.x, .y = cell.y, .w = CELL_SIZE, .h = CELL_SIZE};

                if (SDL_PointInRect(&cursor, &rect) &&
                    (buttons & SDL_BUTTON_LMASK) != 0)
                {
                    save_point(points, &points_size, cell.x, cell.y);
                }
            }
        }

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderClear(ren);

        draw_grid(ren, &cells);

        for (int i = 0; i < points_size; ++i)
        {
            Cell *point = points[i];

            SDL_Rect rect = {
                .x = point->x, .y = point->y, .w = CELL_SIZE, .h = CELL_SIZE};

            SDL_SetRenderDrawColor(ren, 200, 200, 0, 255);
            SDL_RenderFillRect(ren, &rect);
        }

        SDL_Rect player_rect = {
            .x = cells.grid[player.grid_pos.row][player.grid_pos.column].x,
            .y = cells.grid[player.grid_pos.row][player.grid_pos.column].y,
            .w = CELL_SIZE,
            .h = CELL_SIZE};
        SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
        SDL_RenderFillRect(ren, &player_rect);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}
