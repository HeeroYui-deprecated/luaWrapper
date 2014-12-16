
local ex = Example.new()

ex:SetBoolean(true)

-- luaU_getset creates a function that acts as both a getter and a setter!
-- For example, I can use a single function, Integer, to set the value to 100
-- then print out that value
ex:Integer(100)
print("Integer = ", ex:Integer());

ex:SetCPPString("This will be automatically conveted into a std::string")

-- Because I've defined helper functions for Vector2D, I can pass this table
-- to a function expecting a Vector2d as an argument. I don't need to register
-- a Vector2D type with luaW_register - this table will automatically be converted
vec = { x = 20; y = 30 }
ex:SetVec(vec)


vec2 = ex:GetVec()
print "Printing values from the vector stored on `ex`:"
for k, v in pairs(vec2) do
    print(k, v)
end

ex:PrintMe()