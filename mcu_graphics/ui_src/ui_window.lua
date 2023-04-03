-- 窗口控件. 窗口可以是全屏幕的,或者只是一个区域的.
mcu_log = require("engine_log");
display_device = require("mcu_draw");
palette = require("palette");
require("ui_node");
require("ui_header");
ui_window = node:new(0,0);

-- ui_window的属性
-- x,y (node)
-- isFullScrenn: 是否全屏.决定draw的效果. 如果是true,那么宽和高由display_device的w和h决定。
-- background_color : 窗口的背景颜色
-- node_children : 子控件列表. 如window(子window) \ text \ button.
-- window_type : MAIN_WINDOW \ SUB_WINDOW

function ui_window:new(x,y,isFullScreen,w,h,bgc,window_type,process_callback)
    o = {};
    setmetatable(o,self);
    self.__index = ui_window;
    o.x = x;
    o.y = y;
    o.isFullScreen = true;
    o.node_children = {};
    if isFullScreen then
        o.w = display_device.w;
        o.h = display_device.h;
    else
        o.w = w;
        o.h = h;
    end
    o.bgc = bgc;
    o.window_type = window_type;
    o.process_callback = process_callback;
    o.listbox = nil;
    if window_type ~= "MAIN_WINDOW" and window_type ~= "SUB_WINDOW" then
        mcu_log.prt("Wrong Window Type. [Only Support MAIN_WINDOW \\ SUB_WINDOW !]");
        return nil;
    end
    return o;
end

function ui_window:process(event)
    -- 读取按键输入. 如果是SUB_WINDOW，那么会响应关闭事件.
    -- 通过返回值指示是否关闭窗体.
    -- 按到 B 那么关闭 窗口.
    if self.listbox then
        local ret = self.listbox:process(event);
        return ret;
    end

    if self.process_callback and type(self.process_callback) == "function" then
        return self.process_callback(event);
    end
    repeat
        local ev = event:poll();
        if ev == joystick.A then
        elseif ev == joystick.B then
            return UI_HEADER.WINDOW_CLOSE;
        elseif ev == joystick.L then

        elseif ev == joystick.U then

        elseif ev == joystick.D then

        elseif ev == joystick.R then
        end
    until(ev==nil);
    return UI_HEADER.WINDOW_NORMAL;
end

function ui_window:add_listbox(li)
    self.listbox = li;
end

-- 添加node到children列表的最后.
function ui_window:add_child(node)
    table.insert(self.node_children,node);
end

function ui_window:remove_child(ind)
    local len = #self.node_children;
    if len <= 0 then
        return;
    elseif ind < 1 or ind > len then
        return ;
    end
    ind = ind or len;
    table.remove(self.node_children,ind);
end

function ui_window:draw(dt)
    -- 先绘制window (用 bgcolor 进行fill. )
    lcd.fill(self.x,self.y,self.x + self.w ,self.y + self.h ,display_device.rgb565Convert(self.bgc));
    -- 然后根据添加到 node_children 的顺序进行draw.
    for k , v in pairs(self.node_children) do
        v:draw(dt);
    end
end

