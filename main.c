#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: Increase and dicrease brush size
// TODO: row 0 is (0,0) even when GRID_MIN_HEIGHT is 80
// TODO: select area click #1 cell and #2 cell and all cells inbetween are
//       selected
// TODO: Take screenshort of the current points export as png

#define WIDTH            800
#define HEIGHT           800
#define BACKGROUND_COLOR 28, 28, 28, 255

#define GRID_MIN_WIDTH  0
#define GRID_MIN_HEIGHT 80
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
} CursorBrush;

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
}

void build_grid(Cells *cells)
{
    for (int col = GRID_MIN_WIDTH; col < GRID_MAX_WIDTH; col += CELL_SIZE)
    {
        for (int row = GRID_MIN_HEIGHT; row < GRID_MAX_HEIGHT; row += CELL_SIZE)
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

void draw_main_bar(
    SDL_Renderer *ren,
    BrushColors *brush_colors,
    Uint32 buttons,
    SDL_Point cursor
)
{
    int padding = 10;
    int x       = padding;
    int y       = padding;

    for (int i = 0; i < brush_colors->size; ++i)
    {
        SDL_Color color = brush_colors->colors[i];

        SDL_Rect rect = {.x = x, .y = y, .w = CELL_SIZE, .h = CELL_SIZE};

        if (brush_colors->selected == i)
        {
            SDL_Rect selected_rect = {
                .x = rect.x - padding / 2,
                .y = rect.y - padding / 2,
                .w = CELL_SIZE + padding,
                .h = CELL_SIZE + padding};

            SDL_SetRenderDrawColor(ren, 255, 0, 0, 0);
            SDL_RenderDrawRect(ren, &selected_rect);
        }

        SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(ren, &rect);

        if (SDL_PointInRect(&cursor, &rect) &&
            (buttons & SDL_BUTTON_LMASK) != 0)
        {
            brush_colors->selected = i;
        }

        x += padding + CELL_SIZE;
    }
}

void draw_info(
    SDL_Renderer *ren,
    TTF_Font *font,
    Cells *cells,
    int points_size,
    int mouse_x,
    int mouse_y
)
{
    int padding = 10;
    int x       = 5;
    int y       = CELL_SIZE + padding * 2;

    {
        char *text = malloc(50 * sizeof(char));
        sprintf(text, "rows/columns: %i/%i", cells->size.h, cells->size.w);

        SDL_Surface *surface =
            TTF_RenderText_Blended(font, text, (SDL_Color){255, 255, 255, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);

        SDL_Rect rect = {.x = x, .y = y, .w = surface->w, .h = surface->h};

        SDL_RenderCopy(ren, texture, NULL, &rect);

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderDrawRect(ren, &rect);

        x += surface->w + padding;

        free(surface);
        SDL_DestroyTexture(texture);

        free(text);
    }

    {
        char *text = malloc(50 * sizeof(char));
        sprintf(text, "Total points: %i", points_size);

        SDL_Surface *surface =
            TTF_RenderText_Blended(font, text, (SDL_Color){255, 255, 255, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);

        SDL_Rect rect = {.x = x, .y = y, .w = surface->w, .h = surface->h};

        SDL_RenderCopy(ren, texture, NULL, &rect);

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderDrawRect(ren, &rect);

        x += surface->w + padding;

        free(surface);
        SDL_DestroyTexture(texture);

        free(text);
    }

    {
        char *text = malloc(50 * sizeof(char));
        sprintf(text, "%ix%i", mouse_x, mouse_y);

        SDL_Surface *surface =
            TTF_RenderText_Blended(font, text, (SDL_Color){255, 255, 255, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface);

        SDL_Rect rect = {.x = x, .y = y, .w = surface->w, .h = surface->h};

        SDL_RenderCopy(ren, texture, NULL, &rect);

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderDrawRect(ren, &rect);

        x += surface->w + padding;

        free(surface);
        SDL_DestroyTexture(texture);

        free(text);
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

    if (TTF_Init() == -1)
    {
        fprintf(
            stderr, "ERROR: Failed to initalize sdl ttf: %s", SDL_GetError()
        );
        exit(1);
    }

    TTF_Font *font = TTF_OpenFont("./yudit.ttf", 18);
    if (font == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open font: %s", SDL_GetError());
        exit(1);
    }

    bool is_running = true;
    SDL_Event event;

    Cells cells = {.size = 0};
    build_grid(&cells);

    Point **points  = malloc(MAX_CELLS * sizeof(Point));
    int points_size = 0;

    CursorBrush cursor_brush = {
        .grid_pos = {
                     .row    = GRID_MIN_HEIGHT / CELL_SIZE,
                     .column = GRID_MIN_WIDTH / CELL_SIZE}
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
                    cursor_brush.grid_pos.row    = cell.y / CELL_SIZE;
                    cursor_brush.grid_pos.column = cell.x / CELL_SIZE;
                }
                else if (SDL_PointInRect(&cursor, &rect))
                {
                    // Follow mouse cursor
                    cursor_brush.grid_pos.row    = cell.y / CELL_SIZE;
                    cursor_brush.grid_pos.column = cell.x / CELL_SIZE;
                }
            }
        }

        SDL_SetRenderDrawColor(ren, BACKGROUND_COLOR);
        SDL_RenderClear(ren);

        draw_grid(ren, &cells);

        draw_main_bar(ren, &brush_colors, buttons, cursor);

        draw_info(ren, font, &cells, points_size, mouse_x, mouse_y);

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
            .x = cells
                     .grid[cursor_brush.grid_pos.row]
                          [cursor_brush.grid_pos.column]
                     .x,
            .y = cells
                     .grid[cursor_brush.grid_pos.row]
                          [cursor_brush.grid_pos.column]
                     .y,
            .w = CELL_SIZE,
            .h = CELL_SIZE};

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &brush_rect);

        SDL_RenderPresent(ren);
    }

    TTF_CloseFont(font);
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}
