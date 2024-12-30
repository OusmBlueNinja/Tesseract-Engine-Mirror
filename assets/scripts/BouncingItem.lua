-- script.lua
local Math = require("./assets/scripts/math") -- Require the enhanced math module
local Engine = require("./assets/scripts/engine")


-- Variables to track elapsed time and rotation
local elapsedTime = 0
local rotationSpeed = 45 -- Degrees per second for spinning
local new_rotation = 0

-- Variables for bobbing effect
local initial_position = {x = 0, y = 0, z = 0} -- To store the gun's initial position
local bobAmplitude = 0.5 -- Amplitude of the bobbing (units)
local bobFrequency = 0.1 -- Frequency of the bobbing (oscillations per second)

-- Reference to the Gun GameObject and its Transform component
local gun = nil
local transform = nil

local TAU = Math.constants.TAU

function OnInit()
    local startTime = os.clock()
    Engine.Log("Init START", {1.0,1.0,1.0,1.0})



    if not gun then
        gun = Engine.GetGameObjectByTag("Gun")
        if gun then
            transform = gun:GetComponent("Transform")
            if transform then
                local pos = transform:GetPosition()
                initial_position = {x = pos.x, y = pos.y, z = pos.z}
                Engine.Log("Gun found and initial position updated.", {1, 1, 1, 1})
            else
                Engine.Log("Transform component not found on Gun.", {1, 1, 0, 1})
                return
            end
        else
            Engine.Log("Gun GameObject still not found.", {1, 1, 0, 1})
            return
        end
    elseif not transform then
        transform = gun:GetComponent("Transform")
        if transform then
            local pos = transform:GetPosition()
            initial_position = {x = pos.x, y = pos.y, z = pos.z}
            Engine.Log("Transform component found and initial position updated.", {1, 1, 1, 1})
        else
            Engine.Log("Transform component still not found on Gun.", {1, 1, 0, 1})
            return
        end
    end
    Engine.Log("Init OK", {0.0,1.0,0.0,1.0})

    
    local endTime = os.clock()


    Engine.Log(string.format("Time to load: %f", endTime-startTime))



end

-- Update function called every frame
function OnUpdate(deltaTime)
    -- Ensure that the Gun and its Transform component are valid
    if not gun then
        gun = Engine.GetGameObjectByTag("Gun")
        if gun then
            transform = gun:GetComponent("Transform")
            if transform then
                local pos = transform:GetPosition()
                initial_position = {x = pos.x, y = pos.y, z = pos.z}
                Engine.Log("Gun found and initial position updated.", {1, 1, 1, 1})
            else
                Engine.Log("Transform component not found on Gun.", {1, 1, 0, 1})
                return
            end
        else
            return
        end
    elseif not transform then
        transform = gun:GetComponent("Transform")
        if transform then
            local pos = transform:GetPosition()
            initial_position = {x = pos.x, y = pos.y, z = pos.z}
            Engine.Log("Transform component found and initial position updated.", {1, 1, 1, 1})
        else
            return
        end
    end

    -- Increment elapsed time
    elapsedTime = elapsedTime + deltaTime

    -- === Spinning the Gun ===
    -- Update the rotation angle based on rotationSpeed and deltaTime
    new_rotation = new_rotation + (deltaTime * rotationSpeed)

    -- Keep the rotation angle within 0-360 degrees to prevent overflow
    if new_rotation >= 360 then
        new_rotation = new_rotation - 360
    end

    -- Define the new rotation (spinning around the Y-axis)
    local rotation = {
        x = 0, -- Preserving existing rotation on X-axis
        y = new_rotation, -- Updated rotation on Y-axis for spinning
        z = 0 -- Preserving existing rotation on Z-axis
    }

    -- Apply the new rotation to the Transform component
    transform:SetRotation(rotation)

    -- === Bobbing the Gun Up and Down ===
    -- Calculate the bobbing offset using a sine wave
    local bobOffset = bobAmplitude * math.sin(TAU * bobFrequency * elapsedTime)

    -- Define the new position by adding the bobbing offset to the initial Y position
    local new_position = {
        x = initial_position.x, -- No change on X-axis
        y = initial_position.y + bobOffset, -- Bouncing up and down on Y-axis
        z = initial_position.z -- No change on Z-axis
    }

    -- Apply the new position to the Transform component
    transform:SetPosition(new_position)

    -- === Optional: Log Current Rotation and Position ===
    -- Uncomment the following lines if you wish to log the gun's current rotation and position
    -- local current_rotation = transform:GetRotation()
    -- Engine.Log(string.format("Gun Rotation: (X: %.2f, Y: %.2f, Z: %.2f)", current_rotation.x, current_rotation.y, current_rotation.z), {1, 1, 1, 1})

    -- local current_position = transform:GetPosition()
    -- Engine.Log(string.format("Gun Position: (X: %.2f, Y: %.2f, Z: %.2f)", current_position.x, current_position.y, current_position.z), {1, 1, 1, 1})
end
