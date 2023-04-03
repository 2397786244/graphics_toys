queue = {first = 1,last = 0};

function queue:new()
    o = {};
    setmetatable(o,{__index = queue});
    return o;
end

function queue:offer(...)
    for i, v in ipairs({...}) do
        self.last = self.last + 1
        self[self.last] = v
    end
end

function queue:poll()
    if self.first > self.last then
        return nil
    end

    local v = self[self.first]
    self[self.first] = nil
    self.first = self.first + 1
    return v
end

function queue:tail()
    return self[self.last]
end

function queue:size()
    return self.last - self.first + 1
end

function queue:clear()
    while self:poll() do end
    self.first = 1
    self.last = 0
end

--　轮训队列，结束后返回空
function queue:iter()
    local i = self.first

    return function()
    local v = self[i]
    i = i + 1
    return v
    end
end

function queue:submit(i, item)
    self[i] = item
end

