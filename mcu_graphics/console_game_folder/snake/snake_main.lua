-- 左侧区域显示Score(吃到的食物数) 以及游戏进行的时间(秒)
-- 右侧以128 * 128 显示游戏区域. (左上角 32,0 为游戏区域 0,0)
-- 游戏结束后跳进一个sub window,显示分数.然后点击A 回到游戏开始界面(一个window). 或者按B就会退出游戏.
-- 每个tile大小为 8 * 8.
GAME = {};

display_device = require("mcu_draw");
key_input = require("joystick");
require("queue");
audio = require("audio")
mcu_log = require("engine_log")
require("tile");
require("ui_listbox");
require("ui_text")
db = require("game_db");
db.add_game_file("SNAKE");
require("sprite");
require("texture")

-- 全局变量的定义
-- 游戏得分
local SCORE = 0;
local BESTSCORE = 0;  -- 尝试根据store_system 进行读取
local INIT_LENGTH = 5;  -- 初始五段长度.
local snake_init_x = 8;
local snake_init_y = 8;  -- 蛇头初始值
local MOVE = 3;
local snake_list = {};
local MAP = {};  -- 16 * 16.
local FOOD_TYPE = {
NORMAL = 1,
GREEN = 2,
RED = 3,
YELLOW = 4
};
CUR_FOOD_TYPE = FOOD_TYPE.NORMAL;
local DIRECTION = {
    UP = 1,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4
};

local DIR = DIRECTION.DOWN;  -- 初始蛇运动的方向.
local PRE_DIR = DIRECTION.DOWN;  -- 前一个运动的方向.

local foodFlag = false;  -- 食物没被吃掉，为true，吃掉为0.
local foodX = 3;
local foodY = 5;

local TIME_S = 0;
local TIME_M = 0;

-- 游戏初始界面(点击START 开始，点击EXIT退出)
local WINDOW_CUR = 1;  -- 0: Game 1 :Start Page. 2 : GAME Over Page.

function GAME_START()
    return function()
        log.info("GAME START!");
        audio.stop();
        WINDOW_CUR = 0;
    end
end

function GAME_EXIT()
    return function()
        mcu_log.prt("GAME EXIT");
        audio.stop();
        sys.publish("GAME_EXIT","SNAKE");
    end
end
function try_again_func()
    return function ()
        WINDOW_CUR = 0;
        system_init();
        snake_init();
        food_generate();
        MapAndScoreInit();
    end
end
local start_page = listbox:new(60,60,10,4);
start_page:add_item("START",GAME_START());
start_page:add_item("EXIT",GAME_EXIT());
local game_over_page = listbox:new(60,80,20,3); -- Try Again / Exit.
local cur_score_text = text:new(55,40,25,palette[1],display_device.FONT10);
local best_score_text = text:new(55,60,25,palette[1],display_device.FONT10);
game_over_page:add_item("Try Again!",try_again_func());
game_over_page:add_item("Exit",GAME_EXIT());

-- 一些游戏显示数据的定义
local FOOD_DATA = {
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2
};
local NORMAL_FOOD = Tile:new(8,FOOD_DATA);

local SNAKE_DATA = {
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3
};
local SNAKE_BODY = Tile:new(8,SNAKE_DATA);

local BK_DATA = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};
local BK = Tile:new(8,BK_DATA);
local get_score_music =
{
{audio.M1,audio.TT/16},{audio.M2,audio.TT/16},{audio.M3,audio.TT/16},{audio.M5,audio.TT/16},
{audio.M7,audio.TT/16},{audio.H1,audio.TT/16},{audio.H1,audio.TT/16},{audio.H3,audio.TT/16},
};

-- 游戏运行的相关函数
sys = require("sys");

-- 判断蛇是否撞到了自己
-- 遍历结点，判断头结点是否和身体某个结点坐标相同。
function hitSelf()
    local HEADX = snake_list[1][1];
    local HEADY = snake_list[1][2];
    for i = 2,#snake_list do
        if (HEADX == snake_list[i][1] and HEADY
            == snake_list[i][2] ) then
                return true;
        end
    end
    return false;
end

-- 蛇的身体显示
local CNT = 0;
function snake_body_process()
    local temp_ptr;
    local pre_ptr_x = snake_list[1][1];
    local pre_ptr_y = snake_list[1][2];
    local pre_ptr;
    local head = snake_list[1];
    CNT = CNT + 1;
    if CNT >= 6 then
        MOVE = 1;
    else
        MOVE = 0;
    end

    if(DIR == DIRECTION.UP) then
        if (PRE_DIR ~= DIRECTION.DOWN) then
            head[2] = head[2] - MOVE;
            PRE_DIR = DIR;
        else
            head[2] = head[2] + MOVE;
        end
    elseif (DIR == DIRECTION.DOWN) then
        if (PRE_DIR ~= DIRECTION.UP) then
            head[2] = head[2] + MOVE;
            PRE_DIR = DIR;
        else
            head[2] = head[2] - MOVE;
        end
    elseif (DIR == DIRECTION.LEFT) then
        if (PRE_DIR ~= DIRECTION.RIGHT) then
            head[1] = head[1] - MOVE;
            PRE_DIR = DIR;
        else
            head[1] = head[1] + MOVE;
        end
    elseif (DIR == DIRECTION.RIGHT) then
        if (PRE_DIR ~= DIRECTION.LEFT) then
            head[1] = head[1] + MOVE;
            PRE_DIR = DIR;
        else
            head[1] = head[1] - MOVE;
        end
    end

    -- 判断是否碰到边界
    if (head[1] < 0 or head[1] >= 16 or head[2] < 0 or head[2] >= 16 ) or hitSelf() then
        -- GAMEOVER.
        WINDOW_CUR = 2;
        -- 游戏结束，记录Score.
        cur_score_text:set_string("SCORE:"..tostring(SCORE));
        best_score_text:set_string("BEST SCORE:"..tostring(BESTSCORE));
        if SCORE > BESTSCORE then
            db.game_data_wr("SNAKE",tostring(SCORE));
        end
    end

    pre_ptr = {pre_ptr_x,pre_ptr_y};  -- 存储前一个结点的位置。
    -- 处理身体部分
    local tempx,tempy;
    if CNT >= 6 then
        for i = 2,#snake_list do
            temp_ptr = pre_ptr;
            tempx,tempy = snake_list[i][1],snake_list[i][2];
            snake_list[i] = temp_ptr;
            pre_ptr = {tempx,tempy};
        end
        CNT = 0;
    end

    if math.abs(head[1] -  foodX + 1) < 0.1 and math.abs(head[2] - foodY + 1) < 0.1 then
        foodFlag = false;
        SCORE = SCORE + CUR_FOOD_TYPE;
        audio.play(get_score_music,false);
        -- add snake node
        new_node = {pre_ptr[1],pre_ptr[2]};
        table.insert(snake_list,new_node);
    end

end

-- 游戏内容初始化.
-- 蛇的初始化
function snake_init()
    snake_list = {};
    for i = 1 , INIT_LENGTH do
        node = {};
        node[1] = snake_init_x;
        node[2] = snake_init_y - i + 1;
        table.insert(snake_list,node);
        end

end
-- 其他全局变量清除
function system_init()
    TIME_S = 0;
    TIME_M = 0;
    DIR = DIRECTION.DOWN;
    PRE_DIR = DIRECTION.DOWN;
    MOVE = 3;
end

-- 初始化地图.

function MapAndScoreInit()
    for i = 1 , 16 do
        COL = {};
        for j = 1 , 16 do
            -- 一开始地图上全都是 BK 的Tile.
            COL[j] = BK;
        end
        table.insert(MAP,COL);
    end
    -- 从存储系统中载入最高分数.
    SCORE = 0;
    local str = db.game_data_rd("SNAKE");
    BESTSCORE = tonumber(str);
end

-- 食物生成
function food_generate()
    foodX = math.random(1,16);
    foodY = math.random(1,16);
    foodFlag = true;
end

function GAME.init()
    audio.play(audio.MAIN_PAGE_TABLE,false);
    math.randomseed(1215);
    system_init();
    snake_init();
    MapAndScoreInit();
    food_generate();
    WINDOW_CUR = 1;
end

-- 游戏主循环

-- 按键处理
function GAME.process(event)
    if WINDOW_CUR == 1 then
        start_page:process(event);

        return;
    end
    if WINDOW_CUR == 2 then
        -- TODO:绘制游戏结束的ListBox Window.
        game_over_page:process(event);
    end
    repeat
        local ev = event:poll();
        if ev == joystick.A then
        elseif ev == joystick.B then
        elseif ev == joystick.L then
            DIR = DIRECTION.LEFT;
        elseif ev == joystick.U then
            DIR = DIRECTION.UP;
        elseif ev == joystick.D then
            DIR = DIRECTION.DOWN;
        elseif ev == joystick.R then
            DIR = DIRECTION.RIGHT;
        end
    until(ev==nil);
    snake_body_process();
    if not foodFlag then
        -- update food.
        food_generate();
    end
end

-- 渲染主循环

local main_page_snake_state = 1;
-- 1 :Left. 2:Down  3:Right.  4.Up.

function GAME.draw(dt)
    if WINDOW_CUR == 1 then

        start_page:draw(dt);
        return ;
    end
    if WINDOW_CUR == 2 then
        display_device.draw_string(35,15,"Game Over!",palette[1],display_device.FONT12);
        game_over_page:draw(dt);
        cur_score_text:draw(dt);
        best_score_text:draw(dt);
        return ;
    end
    TIME_S = TIME_S + dt;
    -- 游戏内容的绘制.
    -- 左侧分数的绘制
    display_device.draw_string(0,20,"SCO",palette[21]);
    display_device.draw_string(0,35,tostring(SCORE),palette[21]);
    display_device.draw_string(0,55,"TIME",palette[21]);
    if TIME_S >= 60 then
        TIME_M = TIME_M + 1;
        TIME_S = 0;
    end
    if TIME_M > 0 then
        display_device.draw_string(0,70,tostring(math.floor(TIME_M)).."m",palette[21]);
    end
        display_device.draw_string(0,80,tostring(math.floor(TIME_S)).."s",palette[21]);
    --MAP绘制
    local COL;
    for i = 1 , 16 do
        for j = 1 , 16 do
            COL = MAP[i];
            display_device.draw_tile(32+(i-1)*8,(j-1)*8,COL[j]);
        end
    end
    -- FOOD的绘制

    display_device.draw_tile(32 + 8 * (foodX-1),8* (foodY-1),NORMAL_FOOD);
    -- 蛇的绘制

    for k , v in pairs(snake_list) do
        -- v:{snake_x,snake_y};
        display_device.draw_tile(32 + 8 * math.floor(v[1]),8 * math.floor(v[2]),SNAKE_BODY);
    end
end

return GAME;

