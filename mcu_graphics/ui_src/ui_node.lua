mcu_log = require("engine_log");
node = {x=0,y=0};

-- node2d 提供 x , y 和 draw \ getX \ getY
function node:new(x,y)
    o = {};
    setmetatable(o,self);
    self.__index = node;
    o.x = x;
    o.y = y;
    return o;
end

function node:draw(dt)
    mcu_log.prt("Base Class Draw Function.");
end

function node:get_x()
    return self.x;
end

function node:get_y()
    return self.y;
end



