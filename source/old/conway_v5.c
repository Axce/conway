#include <tonc.h>

// If it's red or white, become white. If it lime of black, become black.
void clean(u16 y, u16 x) {
    u16 tmp = m3_mem[y][x] & CLR_RED; //watch only last 5 bits
    m3_mem[y][x] = tmp | (tmp << 5) | (tmp << 10); //copy last 5 bits into first 10
}


int main()
{
    REG_DISPCNT = DCNT_BG2 | DCNT_MODE3;

    int ix, iy;
    int countoldold = 0;
    int countold = 0;
    int count = 0;
    int countnext = 0;

    m3_fill(CLR_WHITE);

    sqran(1); // set random seed

    for (iy = 1; iy < 159; iy++) {
        for (ix = 1; ix < 239; ix++) {
            if (qran() % 2) { m3_mem[iy][ix] =  CLR_BLACK; }
        }
    }

    while(1) {

        //vid_vsync();
        
        for (iy = 1; iy < 159; iy++) {
            
            countoldold = 0; // leftmost column is always dead
            countold = 0;
            if ( ! (m3_mem[iy-1][1] & CLR_LIME) ) { countold ++; }
            if ( ! (m3_mem[iy+1][1] & CLR_LIME) ) { countold ++; }

            for (ix = 1; ix < 239; ix++) {
                                
                count = 0;
                countnext = 0;

                // check if alive (black or red) by masking
                if ( ! (m3_mem[iy-1][ix+1] & CLR_LIME) ) { countnext++;}

                if ( ! (m3_mem[iy  ][ix-1] & CLR_LIME) ) { count ++; }
                if ( ! (m3_mem[iy  ][ix+1] & CLR_LIME) ) { count ++; }

                if ( ! (m3_mem[iy+1][ix+1] & CLR_LIME) ) { countnext++;}

                count = count + countold + countoldold + countnext;
                countoldold = countold;
                countold = countnext;

                // if it's dead (white) and count=3, it becomes green (born)
                if (count == 3) {
                    m3_mem[iy][ix] &= CLR_LIME;
                }

                // if it's alive (black), and under/overpop, it becomes red (dies)
                if (count < 2 || count > 3) {
                    m3_mem[iy][ix] |= CLR_RED;
                }

                // clean top-left pixel
                clean(iy-1, ix-1);
                
            }

            // clean last column or previous row
            clean(iy-1, 238);

        }

        // clean last row
        for (ix = 1; ix < 239; ix++) {
            clean(158, ix);
        }

    }

    return 0;
}
