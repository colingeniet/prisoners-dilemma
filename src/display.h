#include <SDL2/SDL.h>
#define LEN_MAX 10
#define CHAR_BY_W 16
#define CHAR_BY_H 6
#define TITLE "Monitor"
#define ASCCI_PATH "new_consolas_ok.bmp"

typedef struct s_sprite s_sprite;
typedef struct s_context s_context;

struct s_sprite {
    SDL_Texture *texture;
    int w, h;
};

struct s_context {
    SDL_Window *window;
    SDL_Renderer *renderer;
    s_sprite *s_alpha;
    double wscale, hscale;
	SDL_bool full_screen;
    int w_char, h_char;
    int w, h;
    int w_case, h_case;
};

SDL_Surface *load_surface(const char path[]) {
    SDL_Surface *tmp = SDL_LoadBMP(path);
    if(NULL == tmp)
        fprintf(stderr, "Erreur SDL_LoadBMP : %s", SDL_GetError());
    return tmp;
}

s_sprite *create_sprite_from_surface(SDL_Renderer *renderer, SDL_Surface *s) {
    s_sprite *sprite = malloc(sizeof(s_sprite));
    if(NULL == sprite) {
        perror("Error creating sprite : ");
        return NULL;
    }
    sprite->texture = SDL_CreateTextureFromSurface(renderer, s);
    if(NULL == sprite->texture) {
        fprintf(stderr, "Error creating sprite : %s.", SDL_GetError());
        free(sprite);
        return NULL;
    }
    sprite->w = surface->w;
    sprite->h = surface->h;
    return sprite;
}

s_sprite *free_sprite(s_sprite *sprite) {
    if(sprite) {
        SDL_DestroyTexture(sprite->texture);
        free(sprite);
    }
    return NULL;
}

int init_SDL(SDL_Window **window, SDL_Renderer **renderer, int w, int h, 
             const char title[]) {
    if(0 != SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erreur SDL_Init : %s", SDL_GetError());
        return -1;
    }
    *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(NULL == *window) {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        return -1;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == &*renderer) {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        return -1;
    }
    return 0;
}

void destroy_context( s_context *c) {
    if(c->window)
        SDL_DestroyWindow(c->window);
    if(c->renderer)
        SDL_DestroyRenderer(c->renderer);
    if(c->s_alpha)
        free_sprite(c->s_alpha);
    SDL_Quit();
}

int init_context( s_context *c, int nb_strategy, int nb_hosts)
{
    SDL_Surface *s = load_surface(ASCCI_PATH);
    if(NULL == s)
        return -1;
    c->hscale = 1.0;
    c->wscale = 1.0;
    c->w_char = s->w / CHAR_BY_W;
    c->h_char = s->h / CHAR_BY_H;
    c->w_case = c->w_char * LEN_MAX;
    c->h_case = c->h_char;
    c->full_screen = SDL_FALSE;
    c->w = (nb_strategy + 1) * c->w_case;
    c->h = (nb_hosts + 1) * c->h_case;  
    if(init_SDL(&c->window, &c->renderer, c->w, c->h, TITLE) < 0)
        return -1;
    c->s_alpha = create_sprite_from_surface(c->renderer, s);
    SDL_FreeSurface(s);
    if(NULL == c->s_alpha)
        return -1;
    SDL_SetRenderDrawColor(c->renderer, 255, 255, 255, 255);
    return 0;
}

void display_sprite(SDL_Renderer *renderer, s_sprite *sprite, int x, int y, 
                    SDL_Rect *src, double wscale, double hscale) {
    SDL_Rect dst = {x * wscale, y * hscale, src->w * wscale, src->h * hscale};
    SDL_RenderCopy(renderer, sprite->texture, src, &dst);
}

void display_string(const char * const s, size_t len,  s_context *c, 
                    int x, int y) {
    for(size_t i = 0; i < len && s[i] != '\0'; i++) {
        char a = (s[i] < 32 || s[i] > 236) ? ' ' : s[i];
        int row = (a - 32) % CHAR_BY_W, column = (a - 32) / CHAR_BY_W;
        SDL_Rect src = {row * c->w_char, column * c->h_char, c->w_char, c->h_char};
        display_sprite(c->renderer, c->s_alpha, x, y, &src, c->wscale, c->hscale);
        x += c->w_char;
    }
}

void display_number(int n, int len,  s_context *c, int x, int y) {
    char s[100] = {0};
    sprintf(s, "%d", n);
    display_string(s, len, c, x, y);
}

int display_window(s_context *c, strategy strategies[], char host_names[], 
                   long **populations, int n_hosts) {
    SDL_RenderClear(c.renderer);
    for(int strat=0; strat<N_STRATEGIES; strat++)
        display_string(strategies[strat].short_name, 10, c, W_CASE * (strat + 1), 0);
    
    for(int host=0; host<n_hosts; host++) { 
        display_string(host_names[host], 10, c, 0, H_CASE * (host + 1));
        for(int strat=0; strat<N_STRATEGIES; strat++)
            display_number(populations[host][strat], 10, c, W_CASE * (strat + 1), H_CASE * (host + 1);
    }
    for(int i=0, i < N_STRATEGIES, i++)
        SDL_RenderDrawLine(c->renderer, i * c->w_case * c->w_scale, 0, 
                           i * c->w_case * c->w_scale, c->h);
    for(int j=0; j < n_hosts; j++)
        SDL_RenderDrawLine(c->renderer, 0, j * c->h_case * c->h_scale,
                          c->w, j * c->h_case * c->h_scale);
    SDL_RenderPresent(c->renderer);
}
