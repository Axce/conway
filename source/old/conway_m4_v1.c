#include <tonc.h>

#define PAL_DEAD   0
#define PAL_ALIVE   1

M4LINE* display_page;

void save_and_flip() {
    display_page = (M4LINE*) vid_page;
    vid_flip();
}

// args : even coordinates
void draw_4_px(int y, int x, u8 clrid) {
    u16 *dst0= &vid_page[(y*M4_WIDTH+x)>>1];
    u16 *dst1= &vid_page[((y+1)*M4_WIDTH+x)>>1];
    *dst0 = (clrid<<8) |  clrid;
    *dst1 = (clrid<<8) |  clrid;
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

    u8 current_cell;

    m4_fill(PAL_DEAD);
    save_and_flip();
    m4_fill(PAL_DEAD);

    sqran(1312); // set random seed

    for (iy = 2; iy < 158; iy+=2) {
        for (ix = 2; ix < 238; ix+=2) {
            if (qran() % 2) {
                draw_4_px(iy, ix, PAL_ALIVE);
            }
        }
    }

    vid_vsync();
    save_and_flip();

    while(1) {
        
        m4_fill(PAL_DEAD);

        for (iy = 2; iy < 158; iy+=2) {

            count_topbot_left = 0; // leftmost column is always dead

            if ( display_page[iy-2][2] ) { count_topbot_center = 1; } else {count_topbot_center = 0; } // top center
            if ( display_page[iy  ][2] ) { count_middle_center = 1; } else { count_middle_center = 0; } // middle center
            if ( display_page[iy+2][2] ) { count_topbot_center ++; } // bottom center

            for (ix = 2; ix < 238; ix+=2) {
                                
                if ( display_page[iy-2][ix+2] ) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if ( display_page[iy  ][ix+2] ) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if ( display_page[iy+2][ix+2] ) { count_topbot_right++;} // bottom right

                count = count_topbot_center + count_topbot_left + count_topbot_right + count_middle_right;

                count_topbot_left = count_topbot_center + count_middle_center;
                count_topbot_center = count_topbot_right;
                count_middle_center = count_middle_right;
                
                current_cell = display_page[iy][ix];
                if ( (count == 3) || (current_cell && count == 2) ) {
                    draw_4_px(iy, ix, PAL_ALIVE);
                }


            }

        }

        vid_vsync();
        save_and_flip();

    }

    return 0;
}
