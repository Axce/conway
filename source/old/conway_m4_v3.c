#include <tonc.h>

#define SCALE   2  // scale = 1 or 2
#define SEED    1312

#define FIRST_X SCALE
#define LAST_X  SCREEN_WIDTH-SCALE
#define FIRST_Y SCALE
#define LAST_Y  SCREEN_HEIGHT-SCALE

#define PAL_DEAD   0
#define PAL_ALIVE   1

M4LINE* display_page;

void save_and_flip() {
    display_page = (M4LINE*) vid_page;
    vid_flip();
}

u16 *dst;
u16 *dst1;

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

int main() {
    REG_DISPCNT = DCNT_BG2 | DCNT_MODE4;

    pal_bg_mem[PAL_DEAD] = CLR_WHITE;  //dead
    pal_bg_mem[PAL_ALIVE] = CLR_BLACK;  //alive

    int ix, iy;
    int count = 0;

    int count_topbot_left = 0;
    int count_topbot_center = 0;
    int count_topbot_right = 0;
    int count_middle_center = 0;
    int count_middle_right = 0;

    m4_fill(PAL_DEAD);
    save_and_flip();
    m4_fill(PAL_DEAD);

    sqran(FIRST_SEED); // set random seed

    for (iy = FIRST_Y; iy < LAST_Y; iy+=SCALE) {
        for (ix = FIRST_X; ix < LAST_X; ix+=SCALE) {
            if (qran() % 2) {
                draw(iy, ix, PAL_ALIVE);
            }
        }
    }

    vid_vsync();
    save_and_flip();

    while(1) {
        
        m4_fill(PAL_DEAD);

        for (iy = FIRST_Y; iy < LAST_Y; iy+=SCALE) {

            count_topbot_left = 0; // leftmost column is always dead

            if ( display_page[iy-SCALE][SCALE] ) { count_topbot_center = 1; } else {count_topbot_center = 0; } // top center
            if ( display_page[iy      ][SCALE] ) { count_middle_center = 1; } else { count_middle_center = 0; } // middle center
            if ( display_page[iy+SCALE][SCALE] ) { count_topbot_center ++; } // bottom center

            for (ix = FIRST_X; ix < LAST_X; ix+=SCALE) {
                                
                if ( display_page[iy-SCALE][ix+SCALE] ) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if ( display_page[iy      ][ix+SCALE] ) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if ( display_page[iy+SCALE][ix+SCALE] ) { count_topbot_right++;} // bottom right

                count = count_topbot_center + count_topbot_left + count_topbot_right + count_middle_right;

                if ( (count == 3) || (count == 2 && count_middle_center) ) {
                    draw(iy, ix, PAL_ALIVE);
                }

                count_topbot_left = count_topbot_center + count_middle_center;
                count_topbot_center = count_topbot_right;
                count_middle_center = count_middle_right;

            }

        }

        vid_vsync();
        save_and_flip();

    }

    return 0;
}
