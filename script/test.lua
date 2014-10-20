function lua_f2(test_obj)
	test("This is object " .. test_obj.id .. ".");
end

function lua_f()
	test("This is function.");
end

test("Hello from Lua " .. 10 );

