-- math.lua

-- Math constants
local MathConstants = {
    PI = 3.14159,
    E = 2.71828,
    TAU = 6.28318  -- 2 * PI
}

-- Basic math functions
local MathFunctions = {}

function MathFunctions.square(x)
    return x * x
end

function MathFunctions.cube(x)
    return x * x * x
end

function MathFunctions.max(a, b)
    return (a > b) and a or b
end

function MathFunctions.min(a, b)
    return (a < b) and a or b
end

function MathFunctions.clamp(value, minValue, maxValue)
    if value < minValue then
        return minValue
    elseif value > maxValue then
        return maxValue
    else
        return value
    end
end

-- Trigonometric functions
local TrigFunctions = {}

function TrigFunctions.deg_to_rad(degrees)
    return degrees * (MathConstants.PI / 180)
end

function TrigFunctions.rad_to_deg(radians)
    return radians * (180 / MathConstants.PI)
end

-- Export the math module
local Math = {
    constants = MathConstants,
    functions = MathFunctions,
    trig = TrigFunctions
}

return Math
