-- math.lua

-- Math constants
local MathConstants = {
    PI = 3.141592653589793,
    E = 2.718281828459045,
    TAU = 6.283185307179586, -- 2 * PI
    HALF_PI = 1.5707963267948966, -- PI / 2
    SQRT2 = 1.4142135623730951,  -- Square root of 2
    LN2 = 0.6931471805599453     -- Natural log of 2
}

-- Basic math functions
local MathFunctions = {}

function MathFunctions.square(x)
    return x * x
end

function MathFunctions.cube(x)
    return x * x * x
end

function MathFunctions.max(...)
    local args = {...}
    local maxVal = args[1]
    for i = 2, #args do
        if args[i] > maxVal then
            maxVal = args[i]
        end
    end
    return maxVal
end

function MathFunctions.min(...)
    local args = {...}
    local minVal = args[1]
    for i = 2, #args do
        if args[i] < minVal then
            minVal = args[i]
        end
    end
    return minVal
end

function MathFunctions.clamp(value, minValue, maxValue)
    return MathFunctions.max(minValue, MathFunctions.min(value, maxValue))
end

function MathFunctions.lerp(a, b, t)
    return a + (b - a) * t
end

function MathFunctions.is_close(a, b, tolerance)
    tolerance = tolerance or 1e-9
    return MathFunctions.abs(a - b) <= tolerance
end

-- Optimized absolute value function
function MathFunctions.abs(x)
    return (x < 0) and -x or x
end

-- Trigonometric functions
local TrigFunctions = {}

-- Manual definitions of hyperbolic functions
function TrigFunctions.sinh(x)
    return (math.exp(x) - math.exp(-x)) / 2
end

function TrigFunctions.cosh(x)
    return (math.exp(x) + math.exp(-x)) / 2
end

function TrigFunctions.tanh(x)
    return (math.exp(x) - math.exp(-x)) / (math.exp(x) + math.exp(-x))
end

function TrigFunctions.asinh(x)
    return math.log(x + math.sqrt(x * x + 1))
end

function TrigFunctions.acosh(x)
    return math.log(x + math.sqrt(x * x - 1))
end

function TrigFunctions.atanh(x)
    return 0.5 * math.log((1 + x) / (1 - x))
end

function TrigFunctions.sin(x)
    return math.sin(x)
end

function TrigFunctions.cos(x)
    return math.cos(x)
end

function TrigFunctions.tan(x)
    return math.tan(x)
end

function TrigFunctions.atan2(y, x)
    return math.atan2(y, x)
end

function TrigFunctions.hypot(x, y)
    return math.sqrt(x * x + y * y)
end

-- Angle normalization and utilities
function TrigFunctions.normalize_angle_rad(angle)
    angle = angle % MathConstants.TAU
    if angle < 0 then
        angle = angle + MathConstants.TAU
    end
    return angle
end

function TrigFunctions.normalize_angle_pi(angle)
    angle = angle % MathConstants.TAU
    if angle <= -MathConstants.PI then
        angle = angle + MathConstants.TAU
    elseif angle > MathConstants.PI then
        angle = angle - MathConstants.TAU
    end
    return angle
end

-- Exponential and logarithmic functions
local ExpFunctions = {}

function ExpFunctions.exp(x)
    return math.exp(x)
end

function ExpFunctions.log(x, base)
    if base then
        return math.log(x) / math.log(base)
    end
    return math.log(x)
end

function ExpFunctions.pow(base, exponent)
    return base ^ exponent
end

function ExpFunctions.sqrt(x)
    return math.sqrt(x)
end

-- Random utility functions
local RandomFunctions = {}

function RandomFunctions.random(min, max)
    if min and max then
        return math.random() * (max - min) + min
    elseif min then
        return math.random() * min
    else
        return math.random()
    end
end

function RandomFunctions.random_int(min, max)
    return math.random(min, max)
end

-- Statistical functions
local StatFunctions = {}

-- Calculate the mean of a list of numbers
function StatFunctions.mean(numbers)
    local sum = 0
    for _, num in ipairs(numbers) do
        sum = sum + num
    end
    return sum / #numbers
end

-- Calculate the median of a list of numbers
function StatFunctions.median(numbers)
    table.sort(numbers)
    local n = #numbers
    if n % 2 == 1 then
        return numbers[math.ceil(n / 2)]
    else
        return (numbers[n / 2] + numbers[(n / 2) + 1]) / 2
    end
end

-- Calculate the mode of a list of numbers
function StatFunctions.mode(numbers)
    local counts = {}
    local max_count = 0
    local mode_val = numbers[1]
    for _, num in ipairs(numbers) do
        counts[num] = (counts[num] or 0) + 1
        if counts[num] > max_count then
            max_count = counts[num]
            mode_val = num
        end
    end
    return mode_val
end

-- Calculate the variance of a list of numbers
function StatFunctions.variance(numbers)
    local mean_val = StatFunctions.mean(numbers)
    local sum_sq_diff = 0
    for _, num in ipairs(numbers) do
        sum_sq_diff = sum_sq_diff + (num - mean_val) ^ 2
    end
    return sum_sq_diff / #numbers
end

-- Calculate the standard deviation of a list of numbers
function StatFunctions.stddev(numbers)
    return math.sqrt(StatFunctions.variance(numbers))
end

-- Advanced mathematical functions
local AdvancedMath = {}

-- Calculate factorial of n
function AdvancedMath.factorial(n)
    assert(n >= 0 and math.floor(n) == n, "Factorial is only defined for non-negative integers.")
    if n == 0 or n == 1 then
        return 1
    else
        return n * AdvancedMath.factorial(n - 1)
    end
end

-- Calculate permutations P(n, k)
function AdvancedMath.permutation(n, k)
    assert(n >= 0 and k >= 0 and math.floor(n) == n and math.floor(k) == k, "Permutation requires non-negative integers.")
    if k > n then
        return 0
    end
    return AdvancedMath.factorial(n) / AdvancedMath.factorial(n - k)
end

-- Calculate combinations C(n, k)
function AdvancedMath.combination(n, k)
    assert(n >= 0 and k >= 0 and math.floor(n) == n and math.floor(k) == k, "Combination requires non-negative integers.")
    if k > n then
        return 0
    end
    return AdvancedMath.factorial(n) / (AdvancedMath.factorial(k) * AdvancedMath.factorial(n - k))
end

-- Calculate the Greatest Common Divisor using Euclidean algorithm
function AdvancedMath.gcd(a, b)
    a = math.abs(a)
    b = math.abs(b)
    while b ~= 0 do
        a, b = b, a % b
    end
    return a
end

-- Calculate the Least Common Multiple
function AdvancedMath.lcm(a, b)
    a = math.abs(a)
    b = math.abs(b)
    if a == 0 or b == 0 then
        return 0
    end
    return (a * b) / AdvancedMath.gcd(a, b)
end

-- Numerical approximation of the derivative of a function f at point x
function AdvancedMath.derivative(f, x, h)
    h = h or 1e-5
    return (f(x + h) - f(x - h)) / (2 * h)
end

-- Numerical approximation of the integral of a function f from a to b using the trapezoidal rule
function AdvancedMath.integral(f, a, b, n)
    n = n or 1000
    local h = (b - a) / n
    local sum = 0.5 * (f(a) + f(b))
    for i = 1, n - 1 do
        sum = sum + f(a + i * h)
    end
    return sum * h
end

-- Statistical and Advanced Math Functions can be grouped similarly

-- Export the math module
local Math = {
    constants = MathConstants,
    functions = MathFunctions,
    trig = TrigFunctions,
    exp = ExpFunctions,
    random = RandomFunctions,
    stats = StatFunctions,
    advanced = AdvancedMath
}

return Math
