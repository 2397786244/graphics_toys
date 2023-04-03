mcu_log = require("engine_log");
_G.sys = require("sys");
local JOY_STICK = {};
-- TODO: JoyStick GPIO Config.
JOY_STICK.L1 = 3;  --A
JOY_STICK.L2 = 4;  --B
JOY_STICK.R1 = 27; --U
JOY_STICK.R2 = 25; --D
JOY_STICK.R3 = 24; --L
JOY_STICK.R4 = 26; --R

-- KeyScanCode
JOY_STICK.A = 0;
JOY_STICK.B = 1;
JOY_STICK.U = 2;
JOY_STICK.L = 3;
JOY_STICK.D = 4;
JOY_STICK.R = 5;
-- 初始化所有连接Key的GPIO
function JOY_STICK.Init()
    if gpio.debounce then
        gpio.debounce(JOY_STICK.L1, 20,1);
        gpio.debounce(JOY_STICK.L2, 20,1);
        gpio.debounce(JOY_STICK.R1, 20,1);
        gpio.debounce(JOY_STICK.R2, 20,1);
        gpio.debounce(JOY_STICK.R3, 20,1);
        gpio.debounce(JOY_STICK.R4, 20,1);
    end

    gpio.setup(JOY_STICK.L1,nil,gpio.PULLUP,gpio.FALLING);
    gpio.setup(JOY_STICK.L2,nil,gpio.PULLUP,gpio.FALLING);
    gpio.setup(JOY_STICK.R1,nil,gpio.PULLUP,gpio.FALLING);
    gpio.setup(JOY_STICK.R2,nil,gpio.PULLUP,gpio.FALLING);
    gpio.setup(JOY_STICK.R3,nil,gpio.PULLUP,gpio.FALLING);
    gpio.setup(JOY_STICK.R4,nil,gpio.PULLUP,gpio.FALLING);

end

function JOY_STICK.Detect()
    if gpio.get(JOY_STICK.L1) == 0 then
        return JOY_STICK.A;
    elseif gpio.get(JOY_STICK.L2) == 0 then
        return JOY_STICK.B;
    elseif  gpio.get(JOY_STICK.R1) == 0 then
        return JOY_STICK.U;
    elseif gpio.get(JOY_STICK.R2) == 0 then
        return JOY_STICK.D;
    elseif  gpio.get(JOY_STICK.R3) == 0 then
        return JOY_STICK.L;
    elseif gpio.get(JOY_STICK.R4) == 0 then
        return JOY_STICK.R;
    else
        return -1;
    end
end
return JOY_STICK;
