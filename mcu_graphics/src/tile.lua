mcu_log = require("engine_log");
palette = require("palette")

-- tile类型，储存w * w个像素点的rgb数据(取自palette.lua)
Tile = {tile_w=0,tile_rgb_data=nil};
function Tile:new(w,index_data)
    o = {};
    buff = zbuff.create({w,w,16});
    buff:seek(0);
    if #index_data ~= w * w then
        mcu_log.prt("RGB_Data数据长度有误");
        return nil;
    end
    if type(index_data) ~= "table" then
        mcu_log.prt("传入的Tile_RGBData应该是一个数组!");
        return nil;
    end
    setmetatable(o,self);
    self.__index = Tile;

    -- 处理index_data数据，通过查找palette_list然后转换为rgb565的数组，赋值给tile_tile_data。
    for i = 1 , #index_data do
        if index_data[i] > palette.TOTAL_COLOR or index_data[i] < 1 then
            mcu_log.prt("错误的Index Img!");
            return nil;
        end
        buff:writeU16(Tile.rgb565Convert(palette[index_data[i]]));
    end
    o.tile_w = w;
    o.tile_rgb_data = buff;
    return o;
end

function Tile:GetTileWidth()
    return self.tile_w;
end

function Tile:GetRGBData()
    return self.tile_rgb_data;
end
function Tile.rgb565Convert(c)
    local c565 = 0;
    local RGB_RED = 0x00ff0000;
    local RGB_BLUE = 0x000000ff;
    local RGB_GREEN = 0x0000ff00;
    local cr = (c & RGB_RED) >> 19;
    local cg = (c & RGB_GREEN) >> 10;
    local cb  = (c & RGB_BLUE)  >> 3;
    c565 = (cr << 11) + (cg << 5) + (cb);
    -- 将高低字节转换顺序
    local MSB = c565 >> 8;
    local LSB = c565;
    c565 = (LSB << 8) | MSB;
    return c565;
end

return Tile;
