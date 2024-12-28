-- script.lua
local Math = require("math")

local itterator = 0
local ticks = 0
local new_rotation = 0
local speed = 50

function OnInit()
    -- Log a message with a custom red color
    Engine.Log("This is a red message.", {1.0, 0.0, 0.0, 1.0})

    -- Log a message with a custom green color
    Engine.Log("This is a green message.", {0.0, 1.0, 0.0, 1.0})

    -- Log a message with a custom blue color
    Engine.Log("This is a blue message.", {0.0, 0.0, 1.0, 1.0})

    

    
end


function OnUpdate(deltaTime)
    
    -- Attempt to retrieve the GameObject with the tag "Player"
    local player = Engine.GetGameObjectByTag("Player")
    
    if player ~= nil then
        -- Successfully retrieved the GameObject
        -- Call the GetName method on the GameObject
        local transform = player:GetComponent("Transform")

        
        -- Log the player's name with a white color
        -- local pos = transform:GetRotation()
        -- local x = string.format("%.2f", pos.x)
        -- local y = string.format("%.2f", pos.y)
        -- local z = string.format("%.2f", pos.z)
            
        -- Engine.Log("Player Rot: (" .. x .. ", " .. y .. ", " .. z .. ")", {1, 1, 1, 1}) 



        --local position = {x = 0.0, y = 2.0, z = -12.0} -- Define the new position
        --transform:SetPosition(position) -- Call the SetPosition method
        new_rotation = new_rotation + (deltaTime*speed)

        if (new_rotation > 720) then
            new_rotation = 0
        end

        local rotation = {x = new_rotation, y = new_rotation, z = new_rotation,} -- Define the new position
        transform:SetRotation(rotation) -- Call the SetPosition method




        
        -- (Optional) Perform additional operations on the player GameObject
        -- For example, you might want to move the player, change properties, etc.
        -- player:Move(newPosition)
        -- player:SetHealth(100)
    end
    
end


