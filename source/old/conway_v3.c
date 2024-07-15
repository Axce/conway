#include <tonc.h>

int main()
{
    REG_DISPCNT = DCNT_BG2 | DCNT_MODE3;

    /*
    m3_mem[1][1] =  CLR_BLACK;
    m3_mem[1][2] =  CLR_BLACK;
    m3_mem[1][3] =  CLR_BLACK;
    
    m3_mem[80][120] =  CLR_BLACK;
    m3_mem[81][121] =  CLR_BLACK;
    m3_mem[82][121] =  CLR_BLACK;
    m3_mem[82][120] =  CLR_BLACK;
    m3_mem[82][119] =  CLR_BLACK;
    */


    int ix, iy;
    int count;

    sqran(1); // set random seed

    m3_fill(CLR_WHITE);

    for (iy = 1; iy < 159; iy++) {
        for (ix = 1; ix < 239; ix++) {
            if (qran() % 2) { m3_mem[iy][ix] =  CLR_BLACK; }
            
        }
    }

    while(1) {

        //vid_vsync();
        
        for (iy = 1; iy < 159; iy++) {
            for (ix = 1; ix < 239; ix++) {
                
                count = 0;
                if ( ! (m3_mem[iy-1][ix-1] & CLR_LIME) ) { count ++; } // check if black or red (by masking)
                if ( ! (m3_mem[iy-1][ix  ] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy-1][ix+1] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy  ][ix-1] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy  ][ix+1] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy+1][ix-1] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy+1][ix  ] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy+1][ix+1] & CLR_LIME) ) { count ++; }

                // if it's dead (white) and count=3, it becomes green (born)
                if (count == 3) {
                    m3_mem[iy][ix] &= CLR_LIME;
                }

                // if it's alive (black), and under/overpop, it becomes red (dies)
                if (count < 2 || count > 3) {
                    m3_mem[iy][ix] |= CLR_RED;
                }

                // clean top-left pixel
                if (m3_mem[iy-1][ix-1] == CLR_RED) { m3_mem[iy-1][ix-1] = CLR_WHITE; } // die
                if (m3_mem[iy-1][ix-1] == CLR_LIME) { m3_mem[iy-1][ix-1] = CLR_BLACK; } // born
            }

            // clean last column or previous row
            if (m3_mem[iy-1][238] == CLR_RED) { m3_mem[iy-1][238] = CLR_WHITE; } // die
            if (m3_mem[iy-1][238] == CLR_LIME) { m3_mem[iy-1][238] = CLR_BLACK; } // born

        }

        // clean last row
        for (ix = 1; ix < 239; ix++) {
            if (m3_mem[158][ix] == CLR_RED) { m3_mem[158][ix] = CLR_WHITE; } // die
            if (m3_mem[158][ix] == CLR_LIME) { m3_mem[158][ix] = CLR_BLACK; } // born
        }

    }

    return 0;
}

