mcu_log = require("engine_log")
snake_game = require("snake_main");
painting_soft = require("painting_main");

game_storage = {};
game_storage.cur_game_name = nil;

function game_storage.game_run_draw(dt)
    if not game_storage.cur_game_name then
        return;
    end
    if game_storage.cur_game_name == "SNAKE" then
        snake_game.draw(dt);
    elseif game_storage.cur_game_name == "PAINTING" then
        painting_soft.draw(dt);
    end

end

function game_storage.game_run_process(event)
    if not game_storage.cur_game_name then
        return;
    end
    if game_storage.cur_game_name == "SNAKE" then
        snake_game.process(event);
    elseif game_storage.cur_game_name == "PAINTING" then
        painting_soft.process(event);
    end
end

-- 运行游戏初始化
function game_storage.run_game_init(GAME_NAME)
    if (GAME_NAME == "SNAKE") then
        snake_game.init();
    elseif (GAME_NAME == "PAINTING") then
        painting_soft.init();
    end
    game_storage.cur_game_name = GAME_NAME;
end

return game_storage;
