-- 由多个tile组成.
d = require("mcu_draw");
texture = {};
texture.UP = 1;
texture.DOWN = 2;
texture.LEFT = 3;
texture.RIGHT = 4;

-- 参数 : tile 为按照 render_dir 顺序的一个table.
function texture:new(tile,render_dir,tile_w)
    o = {};
    setmetatable(o,self);
    self.__index = texture;
    o.dir = render_dir;
    o.tile = tile;
    o.w = tile_w;
    return o;
end

-- 将一个texture渲染在指定位置.
function texture:render(x,y)
    local len = #self.tile;
    if (self.dir == texture.UP) then
        for i = 1 , len do
            d.draw_tile(x,y - (i - 1) * self.w,self.tile[i]);
        end
    elseif (self.dir == texture.DOWN) then
        for i = 1 , len do
            d.draw_tile(x,y + (i - 1) * self.w,self.tile[i]);
        end
    elseif (self.dir == texture.LEFT) then
        for i = 1 , len do
            d.draw_tile(x - (i - 1) * self.w,y,self.tile[i]);
        end
    elseif (self.dir == texture.RIGHT) then
        for i = 1 , len do
            d.draw_tile(x + (i - 1) * self.w , y,self.tile[i]);
        end
    end
end
