audio = {};

-- 一些预设的音乐数据
-- 定义低音音名F (单位是Hz)
audio.L1 =349;
audio.L2 =392;
audio.L3 =440;
audio.L4 =466;
audio.L5 =523;
audio.L6 =587;
audio.L7 =659;
-- 定义中音音名F
audio.M1 =698;
audio.M2 =784;
audio.M3 =880;
audio.M4 =932;
audio.M5 =1047;
audio.M6 =1175;
audio.M7 =1319;
--定义高音音名F
audio.H1 =1397;
audio.H2 =1568;
audio.H3 =1760;
audio.H4 =1865;
audio.TT = 2000;
-- end

-- Main Page Music.
local MAIN_PAGE_MUSIC = {
    {audio.M1,audio.TT/16},{audio.M1,audio.TT/16},{audio.M1,audio.TT/16},{audio.M1,audio.TT/16},
    {audio.M3,audio.TT/16},{audio.M3,audio.TT/16},{audio.M3,audio.TT/16},{audio.M3,audio.TT/16},
    {audio.H1,audio.TT/16},{audio.H1,audio.TT/16},{audio.M3,audio.TT/16},{audio.M3,audio.TT/16},
    {audio.H1,audio.TT/16},{audio.H2,audio.TT/16},{audio.H2,audio.TT/16},{audio.H2,audio.TT/16}
};

function audio.init()
    audio.audio_play_channel = 24;
    audio.am = 50;  -- 音量
    audio.cur_table = nil;
    audio.cur_music_loop = false;
    audio.MAIN_PAGE_TABLE = MAIN_PAGE_MUSIC;
end

-- 播放指定编号的音乐.
-- table格式: 每个元素 {音名,延时时长}.
-- 返回播放音乐的id.在stop的时候传入该id.
function audio.play(music_table,loop)
    audio.cur_table = music_table;
    audio.cur_music_loop = loop;
    sys.publish("MUSIC_PLAY");
end

-- 停止播放当前音乐.
function audio.stop()
    pwm.close(24);
    sys.publish("MUSIC_PAUSE");
end

function audio.volume_add()
    audio.am = audio.am + 1;
    if audio.am > 100 then
        audio.am = 100;
    end
    return audio.am;
end


function audio.volume_sub()
    audio.am = audio.am - 1;
    if audio.am < 0 then
        audio.am = 0;
    end
    return audio.am;
end

return audio;
