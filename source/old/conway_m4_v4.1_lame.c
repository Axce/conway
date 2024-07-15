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

    pal_bg_mem[PAL_DEAD] = CLR_CREAM;  //dead
    pal_bg_mem[PAL_ALIVE] = CLR_DEAD;  //alive

    int i, ix, iy, rowx;
    int count = 0;

    int count_topmidbot_left = 0;
    int count_topbot_center = 0;
    int count_topbot_right = 0;
    int count_middle_center = 0;
    int count_middle_right = 0;

    int row_top_array[120];
    int row_mid_array[120];
    int row_bot_array[120];
    int* row_top = row_top_array;
    int* row_mid = row_mid_array;
    int* row_bot = row_bot_array;
    int* row_tmp;

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
        
        memset32(row_top, 0, 120);
        
        for (i = 0; i < 120; i++) {
            if (display_page[2][i*2])
                row_mid[i] = 1;
            else
                row_mid[i] = 0;
        }

        m4_fill(PAL_DEAD);

        for (iy = FIRST_Y; iy < LAST_Y; iy+=SCALE) {

            // first cell of each line
            count_topmidbot_left = 0; // leftmost column is always dead
            if ( row_top[1] ) { count_topbot_center = 1; } else {count_topbot_center = 0; } // top center
            if ( row_mid[1] ) { count_middle_center = 1; } else { count_middle_center = 0; } // middle center
            if ( display_page[iy+SCALE][SCALE] ) { // bottom center
                count_topbot_center++; 
                row_bot[1] = 1;
            }

            for (ix = FIRST_X; ix < LAST_X; ix+=SCALE) {

                rowx = ix >> 1;

                if ( row_top[rowx+1] ) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if ( row_mid[rowx+1] ) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if ( display_page[iy+SCALE][ix+SCALE] ) { // bottom right
                    count_topbot_right++;
                    row_bot[rowx+1] = 1;
                }

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

            // rotate pointers
            row_tmp = row_top;
            row_top = row_mid;
            row_mid = row_bot;
            row_bot = row_tmp;

            memset32(row_bot, 0, 120);

        }

        vid_vsync();
        save_and_flip();

    }

    return 0;
}
