
local ex = Example.new()

ex:setBoolean(true)

-- luaU_getset creates a function that acts as both a getter and a setter!
-- For example, I can use a single function, Integer, to set the value to 100
-- then print out that value
ex:m_integer(-100)
print("Integer: ", ex:m_integer());
ex:m_uinteger(1000)
print("m_uinteger: ", ex:m_uinteger());
ex:setUInteger(10000)
print("m_uinteger: ", ex:m_uinteger());

ex:setCPPString("This will be automatically conveted into a std::string")

-- Because I've defined helper functions for Vector2D, I can pass this table
-- to a function expecting a Vector2d as an argument. I don't need to register
-- a Vector2D type with luaW_register - this table will automatically be converted
vec = { x = 20; y = 30 }
ex:setVec(vec)


vec2 = ex:getVec()
print "Printing values from the vector stored on `ex`:"
for k, v in pairs(vec2) do
    print(k, v)
end

ex:printMe()