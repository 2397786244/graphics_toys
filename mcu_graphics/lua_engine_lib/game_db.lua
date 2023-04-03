GAME_DB = {};

function GAME_DB.Init()
    if fdb.kvdb_init("game", "onchip_fdb") then
        log.info("fdb", "kv数据库初始化成功")
    end
end


function GAME_DB.add_game_file(game_name)
    fdb.kv_set(game_name,0);
end

-- 删除游戏
function GAME_DB.remove_game(game_name)
    fdb.kv_del(game_name);
end

-- 游戏保存数据写入
function GAME_DB.game_data_wr(game_name,config_data)
    fdb.kv_set(game_name,config_data);
end

-- 游戏存档数据读取
function GAME_DB.game_data_rd(game_name)
    return fdb.kv_get(game_name);
end

return GAME_DB;

