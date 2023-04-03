local RUN_MODE = "MCU";  -- or MCU
local mcu_log = {};

function mcu_log.prt(str)
    if RUN_MODE == "MCU" then
        log.info(str);
    else
        print(str);
    end

end
return mcu_log;
