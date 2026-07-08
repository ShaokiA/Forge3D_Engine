#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

bool is_running = true;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool initialize_window(void);
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);

int main(int argc, char* argv[]) {

    if (!initialize_window()) {
        return 1;
    }

    while (is_running) {

        process_input();

        update();

        render();
    }

    destroy_window();

    return 0;
}

bool initialize_window(void) {

    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Forge3D Engine",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void process_input(void) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        switch (event.type) {

            case SDL_EVENT_QUIT:
                is_running = false;
                break;

            case SDL_EVENT_KEY_DOWN:

                if (event.key.key == SDLK_ESCAPE) {
                    is_running = false;
                }

                break;
        }
    }
}

void update(void) {

    // Future engine update logic

}

void render(void) {

    // Black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    // Future rendering code

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();
}