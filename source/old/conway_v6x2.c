#include <tonc.h>

// args : even coordinates
void set_four_px(int y, int x, COLOR c) {
    m3_mem[y  ][x  ] = c;
    m3_mem[y  ][x+1] = c;
    m3_mem[y+1][x  ] = c;
    m3_mem[y+1][x+1] = c;
}

// If it's red or white, become white. If it lime of black, become black.
void clean(int y, int x) {
    COLOR tmp = m3_mem[y][x] & CLR_RED; //watch only last 5 bits
    tmp = tmp | (tmp << 5) | (tmp << 10); //copy last 5 bits into first 10
    set_four_px(y, x, tmp);
}

int main() {
    REG_DISPCNT = DCNT_BG2 | DCNT_MODE3;

    int ix, iy;
    int count = 0;

    int count_topbot_left = 0;
    int count_topbot_center = 0;
    int count_topbot_right = 0;
    int count_middle_center = 0;
    int count_middle_right = 0;

    COLOR c;

    m3_fill(CLR_WHITE);

    sqran(1312); // set random seed

    for (iy = 2; iy < 158; iy+=2) {
        for (ix = 2; ix < 238; ix+=2) {
            if (qran() % 2) {
                set_four_px(iy, ix, CLR_BLACK);
            }
        }
    }

    while(1) {

        //vid_vsync();
        
        for (iy = 2; iy < 158; iy+=2) {
            
            count_topbot_left = 0; // leftmost column is always dead

            if ( ! (m3_mem[iy-2][2] & CLR_LIME) ) { count_topbot_center = 1; } else {count_topbot_center = 0; } // top center
            if ( ! (m3_mem[iy  ][2] & CLR_LIME) ) { count_middle_center = 1; } else { count_middle_center = 0; } // middle center
            if ( ! (m3_mem[iy+2][2] & CLR_LIME) ) { count_topbot_center ++; } // bottom center

            for (ix = 2; ix < 238; ix+=2) {
                                
                // check if alive (black or red) by masking
                if ( ! (m3_mem[iy-2][ix+2] & CLR_LIME) ) { count_topbot_right = 1; } else { count_topbot_right = 0; } // top right
                if ( ! (m3_mem[iy  ][ix+2] & CLR_LIME) ) { count_middle_right = 1; } else { count_middle_right = 0; } // middle right
                if ( ! (m3_mem[iy+2][ix+2] & CLR_LIME) ) { count_topbot_right++;} // bottom right

                count = count_topbot_center + count_topbot_left + count_topbot_right + count_middle_right;

                count_topbot_left = count_topbot_center + count_middle_center;
                count_topbot_center = count_topbot_right;
                count_middle_center = count_middle_right;
                

                // if it's dead (white) and count=3, it becomes green (born)
                if (count == 3) {
                    c = m3_mem[iy][ix] & CLR_LIME;
                    set_four_px(iy, ix, c);
                }

                // if it's alive (black), and under/overpop, it becomes red (dies)
                if (count < 2 || count > 3) {
                    c = m3_mem[iy][ix] | CLR_RED;
                    set_four_px(iy, ix, c);
                }

                // clean top-left pixel
                clean(iy-2, ix-2);
                
            }

            // clean last column of previous row
            clean(iy-2, 236);

        }

        // clean last row
        for (ix = 2; ix < 238; ix+=2) {
            clean(156, ix);
        }

    }

    return 0;
}
