-- 储存类型:rgb888
-- 以下的类型可以用于mcu_engine的显示
COLOR_PALETTE = {
    TOTAL_COLOR = 24;
    [1] = 0x000000,  --BLACK
    [2] = 0x757575,  --GRAY1
    [3] = 0xbcbcbc,  --GRAY2
    [4] = 0xffffff,  --WHITE
    [5] = 0x9ffff3,  --淡蓝
    [6] = 0x00ebdb,  --深一点的蓝色
    [7] = 0x00838b,  --偏青的蓝色
    [8] = 0x1b3f5f,  --深蓝色/青色
    [9] = 0xb3ffcf,  --淡青色
    [10] = 0x58f898, --好看的绿色
    [11] = 0x00933b, --深一点的绿色
    [12] = 0x003f17, --比较深的绿色
    [13] = 0xe3ffa3, --草绿色
    [14] = 0x004700, --浓一点的绿色
    [15] = 0xffe7a3, --肤色
    [16] = 0xf3bf3f, --鲜艳的黄色
    [17] = 0x432f00, --土黄色
    [18] = 0xff9b3b, --橙色
    [19] = 0xa70000, --深红色
    [20] = 0xff77b7, --粉色
    [21] = 0x8300f3, --紫色
    [22] = 0x233bef, --深蓝
    [23] = 0x3fbfff, --天蓝色
    [24] = 0xf77bff  --紫红色
};

return COLOR_PALETTE;
