mcu_log = require("engine_log");
display_device = require("mcu_draw");
palette = require("palette");
require("ui_node");
require("ui_header");
listbox = node:new(0,0);

-- ListBox:new
-- w:列表框的宽度.超过宽度的文字不显示,显示 ...
function listbox:new(x,y,w,cnt)
    o = {};
    setmetatable(o,self);
    self.__index = listbox;
    o.x = x;
    o.y = y;
    o.li = {};
    o.func_li = {};
    o.limit_width = w;
    o.limit_count = cnt;  -- 每页显示的最大条目.
    o.cur_item_ptr = 1;  -- 储存现在选择的Item 的索引.
    o.cur_draw_page = 1;  -- 现在显示的页数.
    return o;
end

-- 主要对 A 和 U D 三个按键进行响应。
function listbox:process(event)
    repeat
        local ev = event:poll();
        if ev == joystick.A then
            self.func_li[self.cur_item_ptr]();
        elseif ev == joystick.U then
            if self.cur_item_ptr > 1 then
                self.cur_item_ptr = self.cur_item_ptr - 1;
                -- 如果指针的ind已经小于 (page - 1) * limit_count , 那么往上翻页.
                if (self.cur_item_ptr <= self.limit_count * (self.cur_draw_page- 1) ) then
                    self.cur_draw_page = self.cur_draw_page - 1;
                end
            end
        elseif ev == joystick.D then
            if self.cur_item_ptr < #self.li then
                self.cur_item_ptr = self.cur_item_ptr + 1;
                if (self.cur_item_ptr > self.limit_count * (self.cur_draw_page) ) then
                    self.cur_draw_page = self.cur_draw_page + 1;
                end
            end
        elseif ev == joystick.B then
            return UI_HEADER.WINDOW_CLOSE;
        end
    until(ev==nil);
    return UI_HEADER.WINDOW_NORMAL;
end

-- 当按下A的时候，执行call_back.
function listbox:add_item(item_name,call_back)
    table.insert(self.li,item_name);
    table.insert(self.func_li,call_back);
end

function listbox:remove_item(item_name)
    for k,v in pairs(self.li) do

        if v == item_name then
            self.cur_item_ptr = (#self.li >= 1) and 1 or 0;
            self.cur_draw_page = 1;
            table.remove(self.li,k);
            table.remove(self.func_li,k);
        end
    end
end

function listbox:draw(dt)
    -- 绘制li 中所有的表项.被选中的Item 会显示黄色
    if #(self.li) < 1 then
        return ;
    end

    local start = (self.cur_draw_page - 1) * self.limit_count + 1;
    local remain = #self.li - start + 1;
    local finish = start + ((remain < self.limit_count) and remain or self.limit_count) - 1;
    local str;
    for i = start , finish do
        str = self.li[i];
        if #str <= self.limit_width then
            if i == self.cur_item_ptr then
                display_device.draw_string(self.x,self.y + (i - start) * 12,str,display_device.rgb565Convert(palette[16]),display_device.FONT8);
            else
                display_device.draw_string(self.x,self.y + (i - start) * 12,str,0,display_device.FONT8);
            end
        else
            -- 要截取一部分显示.
            local cut_str = string.sub(str,1,self.limit_width);
            if i == self.cur_item_ptr then
                display_device.draw_string(self.x,self.y + (i - start) * 12,cut_str.."...",display_device.rgb565Convert(palette[16]),display_device.FONT8);
            else
                display_device.draw_string(self.x,self.y + (i - start) * 12,cut_str.."...",0,display_device.FONT8);
            end
        end
    end
end
