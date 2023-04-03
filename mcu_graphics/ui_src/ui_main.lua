display_device = require("mcu_draw");
joystick = require("joystick");
palette = require("palette");
require('queue');
require("ui_button");
require("ui_text");
require("ui_window");
require("ui_listbox")
audio = require("audio");
audio.init();
ui = {};

-- 维护一个主界面的UI窗口列表.
local ui_window_list = {};

-- 在MAIN界面处理各按钮的响应等.
-- 返回一些UI_HEADER定义的值.
function start_page_process()
    return function (event)
    repeat
        local ev = event:poll();
        if ev == joystick.A then
            mcu_log.prt("Enter Game Console.");
            return UI_HEADER.UI_SYSTEM_STARTUP;
        elseif ev == joystick.B then
        elseif ev == joystick.L then
        elseif ev == joystick.U then

        elseif ev == joystick.D then

        elseif ev == joystick.R then
        end
    until(ev==nil);
    return UI_HEADER.WINDOW_NORMAL;
end

end

-- GameRun Function.
function snake_game_run()
    return function ()
        sys.publish("GAME_RUN","SNAKE");
    end
end
function pac_game_run()
    return function ()
       sys.publish("GAME_RUN","PACMAN");
    end
end

function painting_game_run()
    return function ()
       sys.publish("GAME_RUN","PAINTING");
    end
end

-- Settings 功能
-- 调节音量和屏幕亮度


local volume_msg_box = text:new(60,10,15,palette[19],display_device.FONT8);
volume_msg_box:set_string("Current VOL: "..tostring(audio.am));
local lcd_msg_box = text:new(60,20,15,palette[19],display_device.FONT8);
lcd_msg_box:set_string("Current BRI: "..tostring(display_device.get_bl()));

function Setting_Volume_Add()
    return function ()
        local ret = audio.volume_add();
        volume_msg_box:set_string("Current VOL: "..tostring(ret));
    end
end

function Setting_Volume_Sub()
    return function ()
        local ret = audio.volume_sub();
        volume_msg_box:set_string("Current VOL: "..tostring(ret));
    end
end

function Setting_Brightness_Add()
    return function ()
        local ret = display_device.adjust_bl(display_device.cur_bl + 1);
        lcd_msg_box:set_string("Current BRI: "..tostring(ret));
    end
end

function Setting_Brightness_Sub()
    return function ()
        local ret = display_device.adjust_bl(display_device.cur_bl - 1);
        lcd_msg_box:set_string("Current BRI: "..tostring(ret));
    end
end

-- STARTUP Page.
local ui_test_main = ui_window:new(0,0,true,0,0,palette[15],UI_HEADER.WINDOW_TYPE_MAIN,start_page_process());
table.insert(ui_window_list,ui_test_main);
local game_console_main_tile = text:new(40,40,15,palette[19],display_device.FONT10);
game_console_main_tile:set_string("DREAM BOY");
local sub_title = text:new(120,60,10,palette[1],display_device.FONT8);
sub_title:set_string("V1.0");
local start_btn = button:new(40,100,40,10,palette[1],"Press A to START",true);
-- GameSelect Page.
local ui_test_gamesel_page = ui_window:new(0,0,true,0,0,palette[15],UI_HEADER.WINDOW_TYPE_SUB,nil);
local back_to_main_label = text:new(10,120,20,palette[1]);
back_to_main_label:set_string("Press B To Back.");

-- 在GameSelect Page 添加一个List Box.
local game_list = listbox:new(10,15,35,8);
game_list:add_item("Snake",snake_game_run());
-- game_list:add_item("Pac Man",pac_game_run());
game_list:add_item("Painting",painting_game_run());
game_list:add_item("Settings -- VOL ADD",Setting_Volume_Add());
game_list:add_item("Settings -- VOL DEC",Setting_Volume_Sub());
game_list:add_item("Settings -- BRI ADD",Setting_Brightness_Add());
game_list:add_item("Settings -- BRI DEC",Setting_Brightness_Sub());

function ui.init()

    -- 往ui_test_main 中添加一些控件.
    if ui_test_main then
        ui_test_main:add_child(game_console_main_tile);
        ui_test_main:add_child(sub_title);
        ui_test_main:add_child(start_btn);
    end

    if ui_test_gamesel_page then
        ui_test_gamesel_page:add_child(back_to_main_label);
        ui_test_gamesel_page:add_child(game_list);
        ui_test_gamesel_page:add_listbox(game_list);
        ui_test_gamesel_page:add_child(volume_msg_box);
        ui_test_gamesel_page:add_child(lcd_msg_box);

    end
end

function ui.process(event)
    -- 给ui_list最后一个(也就是画出来的那个 window 先进行处理).
    -- 这个window可能是 MAIN_WINDOW （也就是最底层窗口)
    local list_len = #ui_window_list;

    if list_len >= 1 then
        local ret = ui_window_list[list_len]:process(event);

        -- 窗体关闭事件
        if ret == UI_HEADER.WINDOW_CLOSE then
            if ui_window_list[list_len].window_type == UI_HEADER.WINDOW_TYPE_SUB then
                table.remove(ui_window_list);  -- 移除掉最后一个window.(如果不是MAIN_WINDOW的话)
            end
        end
        if ret == UI_HEADER.UI_SYSTEM_STARTUP then
            table.insert(ui_window_list,ui_test_gamesel_page);
        end
    end
end

function ui.draw(dt)
    -- 窗口的绘制
    local list_len = #ui_window_list;
    if ui_window_list[list_len].isFullScreen then
        ui_window_list[list_len]:draw(dt);
    else
        -- 窗口化的window，那么把ui_window_list的每一个window都要执行draw.
        for k , v in pairs(ui_window_list) do
            v:draw(dt);
        end
    end
end
return ui;
