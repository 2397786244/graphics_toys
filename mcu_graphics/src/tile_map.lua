-- 图块集包含多个tile对象.
-- TileMap在draw的时候根据ind获取对应的tile.
require("tile");
display_device = require("mcu_draw");

tile_set = {};
function tile_set:new()
    o = {};
    setmetatable(o,self);
    self.__index = tile_set;
    o.set = {};
    -- {ind:obj,ind_2:obj2...};
    return o;
end

function tile_set:add_tile(tile_obj)
    table.insert(self.set,tile_obj);
end

function tile_set:find_tile(ind)
    return self.set[ind];
end

tile_map = {};

-- w 、 h TileMap 绘制区域.
function tile_map:new(x,y,w,h,ind_map,cnt)
    o = {};
    setmetatable(o,self);
    self.__index = tile_map;
    o.w = cnt;
    o.set = nil;
    o.ind_map = ind_map;
    o.start_x = x;
    o.start_y = y;
    o.end_x = x + w;
    o.end_y = y + h;
    return o;
end

function tile_map:set_tile(ind,palette_ind)
    self.ind_map[ind] = palette_ind;
end

function tile_map:add_tile_set(tile_set)
    self.set = tile_set;
end

-- 绘制所有tile.
function tile_map:draw(dt)
    if not self.set then
        return ;
    end
    local ind;
    local i = 1;
    for x = self.start_x ,self.end_x , self.w  do
        for y = self.start_y , self.end_y , self.w do
            ind = self.ind_map[i];
            display_device.draw_tile(x,y,self.set:find_tile(ind));
            i = i + 1;
        end
    end
end
