local color_palette = require("palette");
display_device = {};

display_device.FONT8 = lcd.font_opposansm8;
display_device.FONT10 = lcd.font_opposansm10;
display_device.FONT12 = lcd.font_opposansm12;

local DEVICE = "MCU";
display_device.cur_bl = 100;
display_device.w = 160;
display_device.h = 128;
display_device.font = lcd.font_opposansm8;
-- lcd 初始化
function display_device.mcu_lcd_init()
    -- gpio init.
    local spi_id = 0;
    local spi_rst = pin.PB03;
    local spi_dc = pin.PB01;
    local spi_cs = pin.PB04;
    local spi_bl = pin.PB00;
    pwm.open(0,1000,display_device.cur_bl,0);
    spi_lcd = spi.deviceSetup(spi_id,spi_cs,0,0,8,40*1000*1000,spi.MSB,1,0);
    lcd.init("st7735v",{port = "device",pin_dc = spi_dc, pin_pwr = spi_bl, pin_rst = spi_rst,direction = 1,w = display_device.w,h = display_device.h,xoffset = 1,yoffset = 2},spi_lcd);
    lcd.invoff();
    lcd.cmd(0x36);
    lcd.data(0x60);

    lcd.setupBuff();
    lcd.autoFlush(false);
end
-- 基本绘制函数
function display_device.draw_rect(x,y,w,h,c)
    if DEVICE == "MCU" then
        lcd.drawRectangle(x,y,x + w,y + h,c);
    end
end

function display_device.draw_circle(x,y,r,c)
    if DEVICE == "MCU" then
        lcd.drawCircle(x,y,r,c);
    end
end

-- @param x0,y0 : 起始点的坐标
-- @param x1,y1 : 结束点坐标
-- @param w : 线的宽度
function display_device.draw_line(x0,y0,x1,y1,w,c)
    if DEVICE == "MCU" then
        local y0_ = 0;
        local y1_ = 0;
        for i = 1 , w do
            if (y0 - i < 0) then
                y0_ = 0;
            else
                y0_ = y0 - i;
            end
            if (y1 - i < 0) then
                y1_ = 0;
            else
                y1_ = y1 - i;
            end
            lcd.drawLine(x0,y0_,x1,y1_,c);
        end
    end
end

-- @param tile:Tile对象.
function display_device.draw_tile(x,y,tile)
    -- 传递给lcd draw_tile 函数的是x0,y0,w,luat_color * color(大小w * w个)
    local a = tile:GetRGBData();
    local w = tile:GetTileWidth();
    if a == nil then
        mcu_log.prt("NIL")
        return ;
    end
    x = math.floor(x);
    y = math.floor(y);
    lcd.draw(x,y,x+w-1,y+w-1,a);
end

function display_device.fill(c)
    if DEVICE == "MCU" then
        lcd.fill(c);
    end
end

function display_device.clear(c)
    if DEVICE == "MCU" then
        lcd.clear(c);
    end
end

function display_device.flush()
    if DEVICE == "MCU" then
        lcd.flush();
    end
end

-- v range [0,100]
function display_device.adjust_bl(v)
    if v > 100 or v < 0 then
        return 0;
    end
    display_device.cur_bl = v;
    pwm.close(0);
    pwm.open(0,1000,v,0);
    return v;
end

function display_device.get_bl()
    return display_device.cur_bl;
end
-- str :ascii string.
function display_device.draw_string(x,y,str,c,size)
    if DEVICE == "MCU" then
        local s = size or display_device.font;
        lcd.setFont(s);
        lcd.drawStr(x,y,str,c);
    end
end

function display_device.rgb565Convert(c)
    local c565 = 0;
    local RGB_RED = 0x00ff0000;
    local RGB_BLUE = 0x000000ff;
    local RGB_GREEN = 0x0000ff00;
    local cr = (c & RGB_RED) >> 19;
    local cg = (c & RGB_GREEN) >> 10;
	local cb  = (c & RGB_BLUE)  >> 3;
    c565 = (cr << 11) + (cg << 5) + (cb);
    return c565;
end
return display_device;
