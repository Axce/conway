#include <tonc.h>

int main()
{
    REG_DISPCNT = DCNT_BG2 | DCNT_MODE3;

    /*
    m3_mem[1][1] =  CLR_WHITE;
    m3_mem[1][2] =  CLR_WHITE;
    m3_mem[1][3] =  CLR_WHITE;
    
    m3_mem[80][120] =  CLR_WHITE;
    m3_mem[81][121] =  CLR_WHITE;
    m3_mem[82][121] =  CLR_WHITE;
    m3_mem[82][120] =  CLR_WHITE;
    m3_mem[82][119] =  CLR_WHITE;
    */


    int ix, iy;
    int count;

    sqran(1); // set random seed

    for (iy = 1; iy < 159; iy++) {
        for (ix = 1; ix < 239; ix++) {
            if (qran() % 2) { m3_mem[iy][ix] =  CLR_WHITE; }
            
        }
    }

    while(1) {

        //vid_vsync();
        
        for (iy = 1; iy < 159; iy++) {
            for (ix = 1; ix < 239; ix++) {
                count = 0;
                if (m3_mem[iy-1][ix-1] == CLR_WHITE || m3_mem[iy-1][ix-1] == CLR_RED) { count ++; }
                if (m3_mem[iy-1][ix  ] == CLR_WHITE || m3_mem[iy-1][ix  ] == CLR_RED) { count ++; }
                if (m3_mem[iy-1][ix+1] == CLR_WHITE || m3_mem[iy-1][ix+1] == CLR_RED) { count ++; }
                if (m3_mem[iy  ][ix-1] == CLR_WHITE || m3_mem[iy  ][ix-1] == CLR_RED) { count ++; }
                if (m3_mem[iy  ][ix+1] == CLR_WHITE || m3_mem[iy  ][ix+1] == CLR_RED) { count ++; }
                if (m3_mem[iy+1][ix-1] == CLR_WHITE || m3_mem[iy+1][ix-1] == CLR_RED) { count ++; }
                if (m3_mem[iy+1][ix  ] == CLR_WHITE || m3_mem[iy+1][ix  ] == CLR_RED) { count ++; }
                if (m3_mem[iy+1][ix+1] == CLR_WHITE || m3_mem[iy+1][ix+1] == CLR_RED) { count ++; }

                if (m3_mem[iy][ix] == CLR_BLACK) { // dead
                    if (count == 3) { // born
                        m3_mem[iy][ix] = CLR_GREEN;
                    }
                }
                else { // alive
                    if (count < 2) { // dies (underpop)
                        m3_mem[iy][ix] = CLR_RED;
                    }
                    else if (count < 4) { // lives
                    }
                    else { //dies (overpop)
                        m3_mem[iy][ix] = CLR_RED;
                    }
                }

                // clean top-left pixel
                if (m3_mem[iy-1][ix-1] == CLR_RED) { m3_mem[iy-1][ix-1] = CLR_BLACK; } // die
                if (m3_mem[iy-1][ix-1] == CLR_GREEN) { m3_mem[iy-1][ix-1] = CLR_WHITE; } // born
            }
        }

        // clean last row
        for (ix = 1; ix < 239; ix++) {
            if (m3_mem[158][ix] == CLR_RED) { m3_mem[158][ix] = CLR_BLACK; } // die
            if (m3_mem[158][ix] == CLR_GREEN) { m3_mem[158][ix] = CLR_WHITE; } // born
        }

        // clean last column
        for (iy = 1; iy < 159; iy++) {
            if (m3_mem[iy][238] == CLR_RED) { m3_mem[iy][238] = CLR_BLACK; } // die
            if (m3_mem[iy][238] == CLR_GREEN) { m3_mem[iy][238] = CLR_WHITE; } // born
        }
    }

    return 0;
}

