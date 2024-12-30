-- unitTests.lua
-- Unit Tests for C++ Lua API and Game Engine

local Math = require("./assets/scripts/math")
local Engine = require("./assets/scripts/engine")


local function OnInit()
    -- Log a message with a custom red color
    Engine.Log("This is a red message.", {1.0, 0.0, 0.0, 1.0})

    -- Log a message with a custom green color
    Engine.Log("This is a green message.", {0.0, 1.0, 0.0, 1.0})

    -- Log a message with a custom blue color
    Engine.Log("This is a blue message.", {0.0, 0.0, 1.0, 1.0})

    

    
end



-- Simple Test Framework

local Tests = {}
local passed = 0
local failed = 0

-- Assertion Functions
local function assertEquals(expected, actual, message)
    if expected ~= actual then
        error(message or string.format("Assertion Failed: expected '%s', got '%s'", tostring(expected), tostring(actual)))
    end
end

local function assertTableEquals(expected, actual, message)
    if type(expected) ~= "table" or type(actual) ~= "table" then
        error(message or "Assertion Failed: One of the values is not a table.")
    end
    for k, v in pairs(expected) do
        if actual[k] ~= v then
            error(message or string.format("Assertion Failed: For key '%s', expected '%s', got '%s'", tostring(k), tostring(v), tostring(actual[k])))
        end
    end
end

local function assertNotNil(value, message)
    if value == nil then
        error(message or "Assertion Failed: value is nil.")
    end
end

local function assertTrue(condition, message)
    if not condition then
        error(message or "Assertion Failed: condition is not true.")
    end
end

-- Helper Function to Capture Logs
local capturedLogs = {}
local originalLog = Engine.Log

local function mockLog(message, color)
    table.insert(capturedLogs, {message = message, color = color})
end

-- Register Test Cases
function Tests.testEngineLog()
    -- Replace Engine.Log with mockLog
    Engine.Log = mockLog

    -- Call OnInit to generate logs
    OnInit()

    -- Restore original Engine.Log
    Engine.Log = originalLog

    -- Assertions
    assertEquals(3, #capturedLogs, "OnInit should log exactly 3 messages.")

    -- Check first log (Red Message)
    assertEquals("This is a red message.", capturedLogs[1].message)
    assertTableEquals({1.0, 0.0, 0.0, 1.0}, capturedLogs[1].color, "First log color mismatch.")

    -- Check second log (Green Message)
    assertEquals("This is a green message.", capturedLogs[2].message)
    assertTableEquals({0.0, 1.0, 0.0, 1.0}, capturedLogs[2].color, "Second log color mismatch.")

    -- Check third log (Blue Message)
    assertEquals("This is a blue message.", capturedLogs[3].message)
    assertTableEquals({0.0, 0.0, 1.0, 1.0}, capturedLogs[3].color, "Third log color mismatch.")
end

function Tests.testGetGameObjectByTag()
    -- Assume "Player" and "Gun" GameObjects exist
    local player = Engine.GetGameObjectByTag("Player")
    assertNotNil(player, "Player GameObject should exist.")

    local gun = Engine.GetGameObjectByTag("Gun")
    assertNotNil(gun, "Gun GameObject should exist.")

    -- Test for a non-existent tag
    local nonExistent = Engine.GetGameObjectByTag("NonExistentTag")
    assertEquals(nil, nonExistent, "Non-existent GameObject should return nil.")
end

function Tests.testGameObjectGetName()
    local player = Engine.GetGameObjectByTag("Player")
    assertNotNil(player, "Player GameObject should exist.")

    local name = player:GetName()
    assertEquals("Player", name, "Player GameObject name should be 'Player'.")
end

function Tests.testGameObjectGetComponent()
    local player = Engine.GetGameObjectByTag("Player")
    assertNotNil(player, "Player GameObject should exist.")

    local transform = player:GetComponent("Transform")
    assertNotNil(transform, "Player should have a Transform component.")

    local nonExistentComponent = player:GetComponent("NonExistentComponent")
    assertEquals(nil, nonExistentComponent, "Non-existent component should return nil.")
end

function Tests.testTransformComponentPosition()
    local player = Engine.GetGameObjectByTag("Player")
    assertNotNil(player, "Player GameObject should exist.")

    local transform = player:GetComponent("Transform")
    assertNotNil(transform, "Player should have a Transform component.")

    -- Get initial position
    local position = transform:GetPosition()
    assertNotNil(position, "Transform:GetPosition should return a position table.")
    assertEquals(0.0, position.x, "Initial X position should be 0.0.")
    assertEquals(2.0, position.y, "Initial Y position should be 2.0.")
    assertEquals(-12.0, position.z, "Initial Z position should be -12.0.")

    -- Set new position
    local newPosition = {x = 5.0, y = 10.0, z = -15.0}
    transform:SetPosition(newPosition)

    -- Get updated position
    local updatedPosition = transform:GetPosition()
    assertTableEquals(newPosition, updatedPosition, "Transform:SetPosition did not update position correctly.")
end

function Tests.testTransformComponentRotation()
    local player = Engine.GetGameObjectByTag("Player")
    assertNotNil(player, "Player GameObject should exist.")

    local transform = player:GetComponent("Transform")
    assertNotNil(transform, "Player should have a Transform component.")

    -- Get initial rotation
    local rotation = transform:GetRotation()
    assertNotNil(rotation, "Transform:GetRotation should return a rotation table.")

    -- Set new rotation
    local newRotation = {x = 45.0, y = 90.0, z = 135.0}
    transform:SetRotation(newRotation)

    -- Get updated rotation
    local updatedRotation = transform:GetRotation()
    assertTableEquals(newRotation, updatedRotation, "Transform:SetRotation did not update rotation correctly.")
end

function Tests.testOnUpdateFunctionality()
    -- Mock GameObject and Transform
    local mockPlayer = {
        GetName = function() return "Player" end,
        GetComponent = function(componentName)
            if componentName == "Transform" then
                return {
                    GetRotation = function()
                        return {x = new_rotation, y = new_rotation, z = new_rotation}
                    end,
                    SetRotation = function(rotation)
                        new_rotation = rotation.x
                    end
                }
            end
            return nil
        end
    }

    -- Replace Engine.GetGameObjectByTag to return mockPlayer
    local originalGetGameObjectByTag = Engine.GetGameObjectByTag
    Engine.GetGameObjectByTag = function(tag)
        if tag == "Player" then
            return mockPlayer
        end
        return originalGetGameObjectByTag(tag)
    end

    -- Initialize rotation
    new_rotation = 0
    speed = 50

    -- Call OnUpdate with deltaTime = 1
    OnUpdate(1)

    -- Check new_rotation
    assertEquals(50, new_rotation, "After first OnUpdate, new_rotation should be 50.")

    -- Call OnUpdate with deltaTime = 14 (to exceed 720)
    OnUpdate(14)

    -- new_rotation should reset to 0 after exceeding 720
    assertEquals(720 + 50 - 720, new_rotation, "After exceeding 720, new_rotation should reset appropriately.")

    -- Restore original Engine.GetGameObjectByTag
    Engine.GetGameObjectByTag = originalGetGameObjectByTag
end

-- Add more test functions below as needed

-- Test Runner
local function runTests()
    print("Starting Unit Tests...\n")
    for testName, testFunc in pairs(Tests) do
        io.write(string.format("Running %s... ", testName))
        local status, err = pcall(testFunc)
        if status then
            print("PASSED")
            passed = passed + 1
        else
            print("FAILED")
            print("  Error:", err)
            failed = failed + 1
        end
    end
    print(string.format("\nUnit Tests Completed: %d Passed, %d Failed", passed, failed))
end

-- Execute Tests
runTests()
