mcu_log = require("engine_log");
display_device = require("mcu_draw");
palette = require("palette");
require("ui_node");
button = node:new(0,0);

-- w : width,
-- h : height,
-- c : color.
-- flash : 按钮是否闪烁

function button:new(x,y,w,h,c,button_text,flash)
    o = {};
    setmetatable(o,self);
    self.__index = button;
    o.x = x;
    o.y = y;
    o.w = w;
    o.h = h;
    o.c = c;
    o.isFocused = false;
    o.btn_text = button_text;
    o.call_back = nil;
    o.flash = flash or false;
    o.flash_cnt = 0;
    return o;
end

function button:set_callback(func)
    self.call_back = func;
end

function button:get_color()
    return self.c;
end

function button:get_width()
    return self.w;
end

function button:get_height()
    return self.h;
end

function button:draw(dt)
    -- 把btn_text 绘制在指定位置.
    -- 在button底部绘制rect.
    local string_color = self.c;
    if self.isFocused then
        -- 选中的时候，画一个背景.
        string_color = display_device.rgb565Convert(palette[16]);
    end

    if self.flash then
        self.flash_cnt = self.flash_cnt + 1;
        if self.flash_cnt >= 20 then
            string_color = display_device.rgb565Convert(palette[4]);
            if self.flash_cnt >= 28 then
                self.flash_cnt = 0;
            end
        end
    end
    display_device.draw_string(self.x,self.y + self.h / 2,self.btn_text,string_color);
end

-- button 被选中的时候,会执行一下call_back function.
function button:select()
    if not self.isFocused then
        self.isFocused = true;
    end
    if self.call_back and type(self.call_back) == "function" then
        self.call_back();
    end
end


function button:unselect()
    if self.isFocused then
        self.isFocused = false;
    end
end


