-- script.lua
local itterator = 0
local ticks = 0
function OnInit()
    -- Log a message with a custom red color
    Engine.Log("This is a red message.", {1.0, 0.0, 0.0, 1.0})

    -- Log a message with a custom green color
    Engine.Log("This is a green message.", {0.0, 1.0, 0.0, 1.0})

    -- Log a message with a custom blue color
    Engine.Log("This is a blue message.", {0.0, 0.0, 1.0, 1.0})

    
end


function OnUpdate(deltaTime)

    ticks = ticks + 1


    if itterator >= 1 then
        Engine.Log("TPS: ".. ticks)
        ticks = 0
        itterator = 0
    end
    itterator = itterator + deltaTime
end


