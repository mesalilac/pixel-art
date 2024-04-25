#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: Take screenshort of the current points export as png

#define WIDTH            800
#define HEIGHT           800
#define BACKGROUND_COLOR 28, 28, 28, 255

#define GRID_MIN_WIDTH  0
#define GRID_MIN_HEIGHT 0
#define GRID_MAX_WIDTH  WIDTH
#define GRID_MAX_HEIGHT HEIGHT
#define GRID_COLOR      32, 32, 32, 255

#define CELL_SIZE 20

#define MAX_CELLS   (GRID_MAX_WIDTH / CELL_SIZE) * (GRID_MAX_HEIGHT / CELL_SIZE)
#define MAX_ROWS    GRID_MAX_HEIGHT / CELL_SIZE - 1
#define MAX_COLUMNS GRID_MAX_WIDTH / CELL_SIZE - 1

#define ADD_COLOR(r, g, b)                                                     \
    brush_colors.colors[brush_colors.size] = (SDL_Color){r, g, b, 255};        \
    brush_colors.size++;

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
} Brush;

typedef struct
{
    SDL_Color colors[99];
    int size;
    int selected;
} BrushColors;

typedef struct
{
    int x;
    int y;
    SDL_Color color;
} Point;

bool point_exists(Point **points, int *points_size, Point *point)
{
    for (int i = 0; i < *points_size; ++i)
    {
        if (points[i]->x == point->x && points[i]->y == point->y)
        {
            return true;
        }
    }

    return false;
}

void save_point(
    Point **points, int *points_size, BrushColors *brush_colors, int x, int y
)
{
    Point *point = malloc(sizeof(Point));
    point->x     = x;
    point->y     = y;
    point->color = brush_colors->colors[brush_colors->selected];

    // TODO if point exists change color
    for (int i = 0; i < *points_size; ++i)
    {
        if (points[i]->x == point->x && points[i]->y == point->y)
        {
            points[i]->color = point->color;
            return;
        }
    }

    points[*points_size] = point;
    *points_size += 1;
    printf("x: %i, y: %i, color_selected: %i\n", x, y, brush_colors->selected);
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

            SDL_SetRenderDrawColor(ren, GRID_COLOR);
            SDL_RenderDrawRect(ren, &rect);
        }
    }
}

int main()
{
    srand(time(0));

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

    Point **points  = malloc(MAX_CELLS * sizeof(Point));
    int points_size = 0;

    Brush brush = {
        .grid_pos = {.row = 0, .column = 0}
    };

    BrushColors brush_colors = {.size = 0, .selected = 0};

    ADD_COLOR(255, 255, 255)
    ADD_COLOR(101, 101, 101)
    ADD_COLOR(20, 20, 20)
    ADD_COLOR(243, 46, 50)
    ADD_COLOR(243, 242, 46)
    ADD_COLOR(105, 243, 46)
    ADD_COLOR(46, 243, 101)
    ADD_COLOR(46, 243, 234)
    ADD_COLOR(46, 151, 243)
    ADD_COLOR(46, 88, 243)
    ADD_COLOR(52, 46, 243)
    ADD_COLOR(77, 46, 243)
    ADD_COLOR(134, 46, 243)
    ADD_COLOR(193, 46, 243)
    ADD_COLOR(243, 46, 226)
    ADD_COLOR(243, 46, 145)

    /*
    for (int i = 0; i < cells.size.w; i++)
    {
        for (int j = 0; j < cells.size.h; j++)
        {
            Cell cell             = cells.grid[j][i];
            brush_colors.selected = rand() % (brush_colors.size - 1 - 0);
            save_point(points, &points_size, &brush_colors, cell.x, cell.y);
        }
    }
    brush_colors.selected = 0;
    */

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
                            &brush_colors,
                            cells
                                .grid[brush.grid_pos.row][brush.grid_pos.column]
                                .x,

                            cells
                                .grid[brush.grid_pos.row][brush.grid_pos.column]
                                .y
                        );
                    // Move up
                    if (event.key.keysym.sym == 'k')
                    {
                        if (brush.grid_pos.row == 0)
                        {
                            brush.grid_pos.row = MAX_ROWS;
                        }
                        else
                        {
                            brush.grid_pos.row--;
                        }
                    }
                    // Move down
                    if (event.key.keysym.sym == 'j')
                    {
                        if (brush.grid_pos.row == MAX_ROWS)
                        {
                            brush.grid_pos.row = 0;
                        }
                        else
                        {
                            brush.grid_pos.row++;
                        }
                    }
                    // Move left
                    // FIXME: If GRID_MAX_WIDTH and GRID_MAX_HEIGHT are not 800
                    //       going left or right skips one cell
                    if (event.key.keysym.sym == 'h')
                    {
                        if (brush.grid_pos.column == 0)
                        {
                            brush.grid_pos.column = MAX_COLUMNS;
                        }
                        else
                        {
                            brush.grid_pos.column--;
                        }
                    }
                    // Move right
                    if (event.key.keysym.sym == 'l')
                    {
                        if (brush.grid_pos.column == MAX_COLUMNS)
                        {
                            brush.grid_pos.column = 0;
                        }
                        else
                        {
                            brush.grid_pos.column++;
                        }
                    }
                    if (event.key.keysym.sym == 's')
                    {
                        save_point(
                            points,
                            &points_size,
                            &brush_colors,
                            cells
                                .grid[brush.grid_pos.row][brush.grid_pos.column]
                                .x,

                            cells
                                .grid[brush.grid_pos.row][brush.grid_pos.column]
                                .y
                        );
                        saving = true;
                    }
                    if (event.key.keysym.sym == 'c')
                        points_size = 0;
                    if (event.key.keysym.sym == 'p')
                    {
                        if (brush_colors.selected == 0)
                        {
                            brush_colors.selected = brush_colors.size - 1;
                        }
                        else
                        {
                            brush_colors.selected--;
                        }
                    }
                    if (event.key.keysym.sym == 'n')
                    {
                        if (brush_colors.selected == brush_colors.size - 1)
                        {
                            brush_colors.selected = 0;
                        }
                        else
                        {
                            brush_colors.selected++;
                        }
                    }
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
                    save_point(
                        points, &points_size, &brush_colors, cell.x, cell.y
                    );
                }
            }
        }

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderClear(ren);

        draw_grid(ren, &cells);

        for (int i = 0; i < points_size; ++i)
        {
            Point *point = points[i];

            SDL_Rect rect = {
                .x = point->x, .y = point->y, .w = CELL_SIZE, .h = CELL_SIZE};

            SDL_SetRenderDrawColor(
                ren,
                point->color.r,
                point->color.g,
                point->color.b,
                point->color.a
            );
            SDL_RenderFillRect(ren, &rect);
        }

        SDL_Rect brush_rect = {
            .x = cells.grid[brush.grid_pos.row][brush.grid_pos.column].x,
            .y = cells.grid[brush.grid_pos.row][brush.grid_pos.column].y,
            .w = CELL_SIZE,
            .h = CELL_SIZE};

        SDL_SetRenderDrawColor(
            ren,
            brush_colors.colors[brush_colors.selected].r,
            brush_colors.colors[brush_colors.selected].g,
            brush_colors.colors[brush_colors.selected].b,
            brush_colors.colors[brush_colors.selected].a
        );
        SDL_RenderDrawRect(ren, &brush_rect);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}
