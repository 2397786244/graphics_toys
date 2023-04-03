PROJECT = "GAME_CONSOLE"
VERSION = "1.0.0"

log.info("GAME_CONSOLE:", PROJECT, VERSION)

_G.sys = require("sys")
if wdt then
    wdt.init(10000);
    sys.timerLoopStart(wdt.feed, 2500);
end

-- include library.
mcu_log = require("engine_log");
key_input = require("joystick");
display_device = require("mcu_draw");
palette = require("palette");
Tile = require("tile");
require("queue");
audio = require("audio")
game_storage = require("game_system");
game_db = require("game_db");
game_db.Init();
-- game console init.

    -- hardware init.
display_device.mcu_lcd_init();
display_device.adjust_bl(10);
mcu.setClk(240);    -- 设置MCU主频.
key_input.Init();
audio = require("audio");
-- audio play test.

    -- GameEngine Init.

-- EventQueue Init.
ping_pong_evq = {};
ping_pong_evq[1] = queue:new();
ping_pong_evq[2] = queue:new();
local cur_draw_evq = 0;   --当前渲染使用cur_draw_evq,那么key_input_Detect得到的event放到另一个evq中。
local cur_offer_evq = 1;

-- User function init.
ui = require("ui_main");
ui.init();

sys.taskInit(function ()
    local v = -1;
    while true do
        v = key_input.Detect();
        if v ~= -1 then
                ping_pong_evq[cur_offer_evq+1]:offer(v);
        end
        sys.wait(120);
    end
end)

local isPLAYING = false;
local playInit = false;

function music_play_callback()
    return function ()
        mcu_log.prt("PLAYING!");
        playInit = true;
    end
end

function music_stop_callback()
    return function ()
        mcu_log.prt("STOP!");
        playInit = false;
        isPLAYING = false;
    end
end

sys.subscribe("MUSIC_PLAY",music_play_callback())
sys.subscribe("MUSIC_PAUSE",music_stop_callback())

sys.taskInit(
    function ()
        local music_play_table;  -- 播放的音频数据.
        local music_play_ptr;  -- 播放指针.
        local len;
        local music_delay = 0;
        while true do
        if playInit then
            music_play_table = audio.cur_table;
            music_play_ptr = 1;
            playInit = false;
            len = #music_play_table;
            isPLAYING = true;
        end

        if isPLAYING then
            local v = music_play_table[music_play_ptr];
            local f = v[1];
            music_delay = v[2];
            pwm.open(audio.audio_play_channel,f,audio.am,0,256);
            music_play_ptr = music_play_ptr + 1;
            if music_play_ptr >  len then
                if audio.cur_music_loop then
                    isPLAYING = true;
                else
                    audio.stop();
                end
                music_play_ptr = 1;
            end
        else
            music_delay = 0;
            music_play_table = nil;
            -- 清除音乐播放标志.
        end
            sys.wait(10+music_delay);
        end

    end
)

local GAME_RUN_STATE = false;

function run_func ()
    return function (GAME_NAME)
        GAME_RUN_STATE = true;
        game_storage.run_game_init(GAME_NAME);
    end
end
function exit_func()
    return function (GAME_NAME)
        GAME_RUN_STATE = false;
    end
end

sys.subscribe("GAME_RUN",run_func());
sys.subscribe("GAME_EXIT",exit_func());

sys.taskInit(function ()
    local dt = 0.0;
    local i = 0;
    local tick_prev = mcu.tick64();
    while true do

        display_device.clear(0xffff);
        -- 取出evq 给user game file 进行处理。
        if GAME_RUN_STATE then
            game_storage.game_run_process(ping_pong_evq[cur_draw_evq+1]);
        else
            ui.process(ping_pong_evq[cur_draw_evq+1]);
        end

        ping_pong_evq[cur_draw_evq+1]:clear();
        -- 处理完成，然后evq 进行交换。
        cur_draw_evq = 1 - cur_draw_evq;
        cur_offer_evq = 1 - cur_offer_evq;

        -- run user process function.

        -- calc delta_time
        cur_tick = mcu.tick64();
        local result, diff_tick = mcu.dtick64(cur_tick,tick_prev);
        dt = diff_tick * 0.000001;
        tick_prev = cur_tick;
        if GAME_RUN_STATE then
            game_storage.game_run_draw(dt);
        else
            ui.draw(dt);
        end

        -- run user draw function.
        -- draw finish.
        display_device.flush();
        sys.wait(10);
    end
end)

sys.run();
