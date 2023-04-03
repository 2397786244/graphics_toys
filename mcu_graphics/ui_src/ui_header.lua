UI_HEADER = {
    WINDOW_NORMAL = 0,  -- 默认值.
    WINDOW_CLOSE = 1,  -- 窗体关闭 (B键)
    UI_ACCEPT = 2 ,  -- 按下了 A 键. (比如弹出一个窗口询问是否接受.然后返回给父window这个值.)
    UI_SYSTEM_GAMERUN = 3 ,
    UI_SYSTEM_STARTUP = 4,   -- STARTUP 界面点击了Accept.
    UI_SYSTEM_SETTING = 5,
    WINDOW_TYPE_MAIN = "MAIN_WINDOW",
    WINDOW_TYPE_SUB = "SUB_WINDOW", -- 创建window的时候传入的type参数.

};
