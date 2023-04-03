-- 文本框. 可以设置每行显示的最长文本，设置显示的颜色.
mcu_log = require("engine_log");
display_device = require("mcu_draw");
palette = require("palette");
require("ui_node");
text = node:new(0,0);

function text:new(x,y,w,c,font)
    -- 设置text
    o = {};
    setmetatable(o,self);
    self.__index = text;
    o.x = x;
    o.y = y;
    o.w = w;
    o.c = c;
    o.font = font or lcd.font_opposansm8;
    o.str = {};
    o.substr_length = 0;

    if o.font == lcd.font_opposansm8 then
        o.font_size = 8;
    elseif o.font == lcd.font_opposansm10 then
        o.font_size = 10;
    elseif o.font == lcd.font_opposansm12 then
        o.font_size = 12;
    else
        mcu_log.prt("To Large!");
        o.font_size = 10;
    end

    return o;
end

-- 给定要显示的字符串。
function text:set_string(str)
    -- 分解str为多个w长度的子string.
    -- 然后放到str表中.

    local pass = string.len(str) // self.w ;
    for i = 1 , pass do
        self.str[i] = string.sub(str,(i - 1) * self.w + 1, i * self.w);
    end
    -- 取出剩余不满足self.w的.
    local re = string.sub(str,pass * self.w + 1);
    if re ~= nil then
        self.str[pass+1] = re;
    end
    self.substr_length = #(self.str);
end

function text:draw(dt)
    if o.substr_length ~= 0 then
        local i = 0;
        for k ,v in pairs(self.str) do
            display_device.draw_string(self.x,self.y + (i * (self.font_size + 4)),v,display_device.rgb565Convert(self.c),self.font);
            i = i + 1;
        end
    end
end
