typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

typedef u16				COLOR;

#define MEM_IO			0x04000000
#define MEM_VRAM		0x06000000

#define REG_DISPCNT		*((volatile u32*)(MEM_IO + 0x0000))

#define DCNT_MODE0		0x0000
#define DCNT_MODE1		0x0001
#define DCNT_MODE2		0x0002
#define DCNT_MODE3		0x0003
#define DCNT_MODE4		0x0004
#define DCNT_MODE5		0x0005

#define DCNT_BG0		0x0100
#define DCNT_BG1		0x0200
#define DCNT_BG2		0x0400
#define DCNT_BG3		0x0800
#define DCNT_OBJ		0x1000

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	160

#define vid_mem			((COLOR*)MEM_VRAM)

#define CLR_BLACK   0x0000
#define CLR_RED     0x001F
#define CLR_LIME    0x03E0
#define CLR_YELLOW  0x03FF
#define CLR_BLUE    0x7C00
#define CLR_MAG     0x7C1F
#define CLR_CYAN    0x7FE0
#define CLR_WHITE   0x7FFF

static inline void m3_plot(int x, int y, COLOR clr) {
    vid_mem[y*SCREEN_WIDTH + x] = clr;
}

typedef COLOR 		M3LINE[SCREEN_WIDTH];
#define m3_mem		((M3LINE*)MEM_VRAM)

static inline COLOR RGB15(int red, int green, int blue) {
    COLOR clr = (blue << 10) | (green << 5) | red;
    return clr;
}