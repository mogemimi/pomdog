-- lua
function Update(time)
	--print("hello")
	--print("time = "..time)
	Transform.Position.X = Transform.Position.X + time * 150.5
	--Transform.Rotation = Transform.Rotation + 0.5 * time

	if Transform.Position.X > 500 then
		Transform.Position.X = -10.0
	end
end

function OnKey(key)
	--print("hello")
	if key == "w" then

	elseif key == "s" then

	elseif key == "a" then

	elseif key == "d" then

	end
end


--[[






]]
