-- 包含一个或多个texture的精灵对象.
-- sprite 提供render函数，调用mcu_draw的tile draw.
-- 调用draw次数每次达到limit渲染sprite的下一帧的texture.

sprite = {};

function sprite:new(x,y,texture)
    o = {};
    setmetatable(o,self);
    self.__index = sprite;
    o.x = x;
    o.y = y;
    if type(texture) ~= "table" then
        log.info("texture 参数应该是一个table.");
        return nil;
    end
    o.tex = texture;
    o.cur_frame = 1;
    o.limit = 1;
    o.limit_cnt = 0;
    return o;
end

function sprite:set_x(x)
    self.x = x;
end

function sprite:set_y(y)
    self.y = y;
end

function sprite:get_posx()
    return self.x;
end

function sprite:get_posy()
    return self.y;
end

function sprite:set_texture(tex)
    self.tex = tex;
end

-- 设置渲染每个texture的间隔
function sprite:set_limit(l)
    self.limit = l;
end

function sprite:render()
    self.tex[self.cur_frame]:render(self.x,self.y);
    self.limit_cnt = self.limit_cnt + 1;
    if (self.limit_cnt >= self.limit) then
        self.cur_frame = self.cur_frame + 1;
        if self.cur_frame > #self.tex then
            self.cur_frame = 1;
        end
        self.limit_cnt = 0;
    end
end

