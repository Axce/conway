#include <tonc.h>

#define SCALE   2  // scale = 1 or 2
#define FIRST_SEED    1312

#define FIRST_X SCALE
#define LAST_X  SCREEN_WIDTH-SCALE
#define FIRST_Y SCALE
#define LAST_Y  SCREEN_HEIGHT-SCALE

enum PAL {
    PAL_DEAD,
    PAL_ALIVE,
    PAL_TEXT,
    PAL_TEXT_BG
};

M4LINE* display_page;

u16 *dst;
u16 *dst1;

int ix, iy;

int seed = FIRST_SEED;

void save_and_flip() {
    display_page = (M4LINE*) vid_page;
    vid_flip();
}

// args : even coordinates
void draw(int y, int x, u8 clrid) {
    if (SCALE == 1) {
        m4_plot(x, y, clrid);
    }
    else {
        dst= &vid_page[(y*M4_WIDTH+x)>>1];
        dst1= &vid_page[((y+1)*M4_WIDTH+x)>>1];
        *dst = (clrid<<8) |  clrid;
        *dst1 = (clrid<<8) |  clrid;
    }
}

void init() {

    m4_fill(PAL_DEAD);
    
    sqran(seed); // set random seed

    for (iy = FIRST_Y; iy < LAST_Y; iy+=SCALE) {
        for (ix = FIRST_X; ix < LAST_X; ix+=SCALE) {
            if (qran() % 2) {
                draw(iy, ix, PAL_ALIVE);
            }
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
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

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

int main() {

    vid_vsync();

    REG_DISPCNT = DCNT_BG2 | DCNT_MODE4;

    pal_bg_mem[PAL_DEAD] = CLR_CREAM;  //dead
    pal_bg_mem[PAL_ALIVE] = CLR_DEAD;  //alive
    pal_bg_mem[PAL_TEXT] = CLR_BLACK;
    pal_bg_mem[PAL_TEXT_BG] = CLR_WHITE;

    int count = 0;

    int count_topmidbot_left = 0;
    int count_topbot_center = 0;
    int count_topbot_right = 0;
    int count_middle_center = 0;
    int count_middle_right = 0;

    init();

    vid_vsync();
    save_and_flip();

    while (1) {
        
        seed++;
        
        m4_fill(PAL_DEAD);

        for (iy = FIRST_Y; iy < LAST_Y; iy+=SCALE) {

            count_topmidbot_left = 0; // leftmost column is always dead

            if ( display_page[iy-SCALE][SCALE] ) { count_topbot_center = 1; } else {count_topbot_center = 0; } // top center
            if ( display_page[iy      ][SCALE] ) { count_middle_center = 1; } else { count_middle_center = 0; } // middle center
            if ( display_page[iy+SCALE][SCALE] ) { count_topbot_center ++; } // bottom center

            for (ix = FIRST_X; ix < LAST_X; ix+=SCALE) {
                                
                if ( display_page[iy-SCALE][ix+SCALE] ) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if ( display_page[iy      ][ix+SCALE] ) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if ( display_page[iy+SCALE][ix+SCALE] ) { count_topbot_right++;} // bottom right

                count = count_topbot_center + count_topmidbot_left + count_topbot_right + count_middle_right;

                //if ( (count > 6)
                //    || ((count == 3 || count == 5 ) && !count_middle_center)
                //    || ((count == 2 || count == 4 || count == 6 ) && count_middle_center) ) {                         // Geology
                //if ( (count > 4) || (count == 3 && !count_middle_center) ) {                                          // Diamoeba
                //if ( (count == 3) || (count == 2 && count_middle_center) || (count == 6 && !count_middle_center) ) {  // HighLife
                //if ( (count == 3) || (count == 2 && count_middle_center) || (count == 8 && !count_middle_center) ) {  // Pedestrian Life
                if ( (count == 3) || (count == 2 && count_middle_center) ) {                                          // Life
                    draw(iy, ix, PAL_ALIVE);
                }

                count_topmidbot_left = count_topbot_center + count_middle_center;
                count_topbot_center = count_topbot_right;
                count_middle_center = count_middle_right;

            }

        }

        key_poll();

        if (key_curr_state()) {
            if (key_hit(KEY_START)) {

                draw_pause();

                while (KEY_DOWN_NOW(KEY_START));
                while (KEY_UP_NOW(KEY_START));
            }
            if (key_hit(KEY_SELECT)) {
                init();
            }
        }

        vid_vsync();
        save_and_flip();

    }

    return 0;
}
