#include <tonc.h>

#define FIRST_SEED 1312
#define SCALE 2               // scale = 1 or 2
#define RULE_NAME_DURATION 30 // in frames

#define FIRST_X SCALE
#define LAST_X SCREEN_WIDTH - SCALE
#define FIRST_Y SCALE
#define LAST_Y SCREEN_HEIGHT - SCALE

#define RULES_NUMBER 10

enum RULES {
    LIFE,
    HIGHLIFE,
    PEDESTRIANLIFE,
    GEOLOGY,
    DIAMOEBA,
    HOLSTEIN,
    CORAL,
    VOTE,
    LANDRUSH,
    MAZECTRIC
};

char* rules_names[] = {
    "Life",
    "HighLife",
    "Pedestrian Life",
    "Geology",
    "Diamoeba",
    "Holstein",
    "Coral",
    "Vote",
    "Land Rush",
    "Mazectric"
};

enum PAL {
    PAL_DEAD=0,
    PAL_ALIVE,
    PAL_TEXT,
    PAL_TEXT_BG
};

int rule = LIFE;

M4LINE *display_page;

u16 *dst;
u16 *dst1;

int ix, iy;

int seed = FIRST_SEED;

void save_and_flip() {
    display_page = (M4LINE *)vid_page;
    vid_flip();
}

// args : even coordinates
void draw(int y, int x, u8 clrid) {
    if (SCALE == 1) {
        m4_plot(x, y, clrid);
    }
    else {
        dst = &vid_page[(y * M4_WIDTH + x) >> 1];
        dst1 = &vid_page[((y + 1) * M4_WIDTH + x) >> 1];
        *dst = (clrid << 8) | clrid;
        *dst1 = (clrid << 8) | clrid;
    }
}

void init() {

    m4_fill(PAL_DEAD);

    sqran(seed); // set random seed

    for (iy = FIRST_Y; iy < LAST_Y; iy += SCALE) {
        for (ix = FIRST_X; ix < LAST_X; ix += SCALE) {
            if (qran() & 1)
                draw(iy, ix, PAL_ALIVE);
        }
    }
}

void draw_pause() {
    int width = 23;
    int height = 9;
    int left = (SCREEN_WIDTH - width) / 2;
    int top = (SCREEN_HEIGHT - height) / 2;
    int right = left + width;
    int bottom = top + height;

    int pause_text[9][23] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    vid_vsync();

    vid_flip();

    m4_rect(left, top, right, bottom, PAL_TEXT_BG);
    m4_frame(left, top, right, bottom, PAL_TEXT);

    
        for (int y = 0; y < height ; y++) {
            for (int x = 0; x < width ; x++) {
                if (pause_text[y][x])
                    m4_plot(x + left, y + top, PAL_TEXT);
            }
        }
    

    vid_flip();
}

void draw_rule_name() {

    vid_vsync();

    vid_flip();

    m4_rect(0, 74, SCREEN_WIDTH, 84, PAL_TEXT_BG);
    m4_hline(0, 74, 239, PAL_TEXT);
    m4_hline(0, 84, 239, PAL_TEXT);
    m4_puts(8, 76, rules_names[rule], PAL_TEXT);

    vid_flip();

    for (int i=0 ; i < RULE_NAME_DURATION ; i++)
        vid_vsync();

}

int main() {

    txt_init_std();

    vid_vsync();

    REG_DISPCNT = DCNT_BG2 | DCNT_MODE4;

    COLOR pal_tmp;

    pal_bg_mem[PAL_DEAD] = CLR_CREAM; // dead
    pal_bg_mem[PAL_ALIVE] = CLR_DEAD; // alive
    pal_bg_mem[PAL_TEXT] = CLR_BLACK;
    pal_bg_mem[PAL_TEXT_BG] = CLR_WHITE;

    int n = 0; // neighbors

    int count_topmidbot_left = 0;
    int count_topbot_center = 0;
    int count_topbot_right = 0;
    int alive = 0;  // count_middle_center
    int count_middle_right = 0;

    bool draw_cell;

    init();

    vid_vsync();
    save_and_flip();

    while (1) {

        seed++;

        m4_fill(PAL_DEAD);

        for (iy = FIRST_Y; iy < LAST_Y; iy += SCALE) {

            count_topmidbot_left = 0; // leftmost column is always dead

            if (display_page[iy - SCALE][SCALE]) { count_topbot_center = 1; } else { count_topbot_center = 0; } // top center
            if (display_page[iy        ][SCALE]) { alive = 1; } else { alive = 0; } // middle center
            if (display_page[iy + SCALE][SCALE]) { count_topbot_center++; } // bottom center

            for (ix = FIRST_X; ix < LAST_X; ix += SCALE)
            {

                if (display_page[iy - SCALE][ix + SCALE]) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if (display_page[iy        ][ix + SCALE]) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if (display_page[iy + SCALE][ix + SCALE]) { count_topbot_right++; } // bottom right

                n = count_topbot_center + count_topmidbot_left + count_topbot_right + count_middle_right;

                switch (rule) {
                    case LIFE:              draw_cell = (n == 3) || (n == 2 && alive);
                        break;

                    case HIGHLIFE:          draw_cell = (n == 3) || (n == 2 && alive) || (n == 6 && !alive);
                        break;

                    case PEDESTRIANLIFE:    draw_cell = (n == 3) || (n == 2 && alive) || (n == 8 && !alive);
                        break;

                    case GEOLOGY:           draw_cell = (n > 6) || ((n == 3 || n == 5) && !alive) || ((n == 2 || n == 4 || n == 6) && alive);
                        break;

                    case DIAMOEBA:          draw_cell = (n > 4) || (n == 3 && !alive);
                        break;

                    case HOLSTEIN:          draw_cell = (n > 5) || ((n == 3 || n == 5) && !alive) || (n == 4 && alive);
                        break;

                    case CORAL:             draw_cell = (n == 3 && !alive) || (n > 3 && alive);
                        break;

                    case VOTE:              draw_cell = (n + alive) > 4;
                        break;

                    case LANDRUSH:          draw_cell = (n == 3) || (n == 5) || (n > 1 && n != 6 && alive);
                        break;

                    case MAZECTRIC:          draw_cell = (n == 3) || (n < 5 && n && alive);
                        break;
                    
                    default: draw_cell = false;
                }

                if (draw_cell) {
                    draw(iy, ix, PAL_ALIVE);
                }

                count_topmidbot_left = count_topbot_center + alive;
                count_topbot_center = count_topbot_right;
                alive = count_middle_right;
            }
        }

        key_poll();

        vid_vsync();

        if (key_curr_state()) {
            if (key_hit(KEY_START)) {
                draw_pause();

                while (KEY_DOWN_NOW(KEY_START));
                while (KEY_UP_NOW(KEY_START));
            }
            if (key_hit(KEY_RIGHT)) {
                rule = (rule + 1) % RULES_NUMBER;

                draw_rule_name();

                init();
            }
            if (key_hit(KEY_LEFT)) {
                rule = (rule + (RULES_NUMBER - 1)) % RULES_NUMBER;

                draw_rule_name();

                init();
            }
            if (key_hit(KEY_A)) {
                init();
            }
            if (key_hit(KEY_B)) {
                pal_tmp = pal_bg_mem[PAL_DEAD];
                pal_bg_mem[PAL_DEAD] = pal_bg_mem[PAL_ALIVE];
                pal_bg_mem[PAL_ALIVE] = pal_tmp;
            }
        }

        save_and_flip();
    }

    return 0;
}
