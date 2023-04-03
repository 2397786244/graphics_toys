display_device = require("mcu_draw");
key_input = require("joystick");
require("queue");
mcu_log = require("engine_log")
require("ui_button");
require("ui_header");
require("ui_window");
palette = require("palette");
db = require("game_db");
db.add_game_file("PAINTING");
require("sprite");
require("texture");

PAINTING = {};
local WINDOW_CUR = 0;  -- 0：起始界面，可以退出  / 选择图像大小
-- 1 : 画布.
-- 2 : 点击了菜单按钮。可以退出.

-- 鼠标 Texture.
local mouse_data = {
    1,4,1,1,1,1,4,1,
    1,1,4,4,4,4,1,1,
    1,1,4,4,4,4,1,1,
    1,1,4,4,4,4,1,1,
    1,1,4,4,4,4,1,1,
    1,4,1,1,1,4,4,1,
    1,1,1,1,1,4,4,4,
    1,1,1,1,1,1,4,4,
};

mouse = Tile:new(8,mouse_data);
mouse_tex = texture:new({mouse},texture.DOWN,0);
mouse_sprite = sprite:new(50,50,{mouse_tex});

local new_file_button = button:new(40,50,30,10,palette[4],"New File",false);
local exit_button = button:new(40,60,30,10,palette[4],"Exit",false);
local size_8x8_button = button:new(100,50,30,10,palette[4],"Size:8x8",false);
local size_16x16_button = button:new(100,60,30,10,palette[4],"Size:16x16",false);
local size8_rect = {100,50,10,10};
local size16_rect = {100,60,10,10};
local size_sel = 0;

-- 生成指定大小的ind_map
function generate_ind_map(size_w,init_value)
    o = {};
    for i = 1 , size_w * size_w do
        o[i] = init_value;
    end
    return o;
end

-- 判断鼠标位置
-- mouse_pos : {x,y}
-- button_rect :{x,y,w,h}
function inRect(mouse_pos,button_rect)
    if (mouse_pos[1] >= button_rect[1] and mouse_pos[1] <= button_rect[1] + button_rect[3]
    and mouse_pos[2] >= button_rect[2] and mouse_pos[2] <= button_rect[2] + button_rect[4]) then
        return true;
    else
        return false;
    end
end

function mouse_control(ev)
    if ev == joystick.L then
    local x = mouse_sprite:get_posx() - 8;
    if x < 0 then
        x = 0;
    end
    mouse_sprite:set_x(x);
    elseif ev == joystick.U then
    local y = mouse_sprite:get_posy() - 8;
    if y < 0 then
        y = 0;
    end
    mouse_sprite:set_y(y);
    elseif ev == joystick.D then
    local y = mouse_sprite:get_posy() + 8;
    if y > 128 then
        y = 128;
    end
    mouse_sprite:set_y(y);
    elseif ev == joystick.R then
    local x = mouse_sprite:get_posx() + 8;
    if x > 160 then
        x = 160;
    end
    mouse_sprite:set_x(x);
    end
end
function start_page_select_process()
    return function (event)
        local mousex = mouse_sprite:get_posx();
        local mousey = mouse_sprite:get_posy();
        local exit_rect = {40,60,10,10};
        local newfile_rect = {40,50,30,10};

        repeat
            local ev = event:poll();
            if ev == joystick.A then
                if inRect({mousex,mousey},exit_rect) then
                    sys.publish("GAME_EXIT","PAINTING");
                elseif inRect({mousex,mousey},size8_rect) then
                    size_8x8_button:select();
                    size_16x16_button:unselect();
                    size_sel = 1;
                elseif inRect({mousex,mousey},size16_rect) then
                    size_8x8_button:unselect();
                    size_16x16_button:select();
                    size_sel = 2;
                elseif inRect({mousex,mousey},newfile_rect) then
                    if size_sel ~= 0 then
                        WINDOW_CUR = 1;
                    end
                end
            elseif ev == joystick.B then
            end
            mouse_control(ev);
        until(ev==nil);
        return UI_HEADER.WINDOW_NORMAL;
    end
end

start_page = ui_window:new(0,0,true,0,0,palette[1],
UI_HEADER.WINDOW_TYPE_MAIN,start_page_select_process()
);

function PAINTING.init()
    start_page:add_child(exit_button);
    start_page:add_child(new_file_button);
    start_page:add_child(size_8x8_button);
    start_page:add_child(size_16x16_button);
end

function PAINTING.process(event)
    if WINDOW_CUR == 0 then
        start_page:process(event);
    elseif WINDOW_CUR == 1 then
        -- x坐标在 0 - 32 : 取得颜色.
        -- 否则按下A的时候就是上色(设置tile_map对应的palette_ind);
        repeat
            local ev = event:poll();
            if ev == joystick.B then
                sys.publish("GAME_EXIT","PAINTING");
            end
        until(ev==nil);
    end
end

function PAINTING.draw(dt)
    if WINDOW_CUR == 0 then
        start_page:draw(dt);
    elseif WINDOW_CUR == 1 then
        lcd.fill(0,0,160,128,display_device.rgb565Convert(palette[1]));
        display_device.draw_string(5,50,"Features are being improved",palette[4],display_device.FONT8);
        display_device.draw_string(40,75,"Coming Soon...",palette[4],display_device.FONT10);
    end
    mouse_sprite:render();
end

return PAINTING;
