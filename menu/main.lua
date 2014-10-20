btn="button"

levels={
	"hello_level",
	"level1",
	"level2",
	"level_solo"
}
buttons={}

function init()
	menu=getMenu()
	manager=getManager()
	renderer=getRenderer()
	arial=manager:getFont("arial")

	btn_y=100
	btn_size_y=25

--	button_foo=addWidget(menu,btn,vec2(10,btn_y),vec2(200,btn_size_y* 25/30 ))
--	button_foo:setPropString("text","hello foo")


	for i=1,(#levels),1 do
		buttons[i]=addWidget(menu,btn,vec2(10,btn_y),vec2(200,btn_size_y* 25/30 ))
		buttons[i]:setPropString("text",levels[i])
		btn_y=btn_y+btn_size_y
	end
	runtime=0

	--code to load and display images
	image_car=manager:getImg("media/car.png")
	image_light=manager:getImg("media/light.png")
	object_car=renderer:createObject(vec2(100,50))
	object_car:addTexture(image_car)
	object_car:addTexture(image_light)
	node_car=menu:getRootNode():createNode(vec2(0,0),0)
	node_car:setObject(object_car)

	shader=UserClientShader()
	uniform_time=0
	if shader:compileFromFile("shader/distort.glsl") then
		uniform_time=shader:getUniform("time")
		object_car:setShader(shader)
	end

end

function getCarPos(t)
	return vec2(
		150+(math.sin(t*0.2432)*5 +math.sin(t)*10 + math.sin(t*2.1)*4)*6,
		100+(math.sin(t*0.4753)*5 +math.cos(t)*10 + math.cos(t*2)*5)*2
	)
end

function update(delta)

--	my_node:setAngle(renderer:getMousePos().y+renderer:getMousePos().x)
--	my_node:setPos(vec2(0,0)-renderer:getMousePos()*10+vec2(500,500))

--	print("hello menu")

	for i=1,(#buttons) do
		if buttons[i]:mouseClicked() then
			print("Start level "..levels[i]);
			--[[
			if loadLevel(levels[i]) then
				menu:show(false)
			end
			]]

			--[[
			if levels[i]=="level_solo" then
				if loadLevel(levels[i]) then
					openMenu("tweaker")
					menu:show(false)
				end
			else
			]]
				setPropString("level_id",levels[i])
				openMenu("select_car")
				menu:show(false)
			--end
		end
	end
--	if btn_play:mouseClicked() then
--		print("Start game!")
--	end

	runtime=runtime+delta

	p=renderer:getMousePos()

	t=runtime/15000
	car_pos=getCarPos(t)
	car_pos2=getCarPos(t+0.001)
	angle=90+(-math.atan2(car_pos.x-car_pos2.x,car_pos.y-car_pos2.y))*57.325

	object_car:setFloat(uniform_time,runtime)
	node_car:setPos(car_pos)
--	node_car:setPos(p)
	node_car:setAngle(angle)

--	print("mouse pos "..p.x..", "..p.y.." down "..tostring(renderer:getMousePressed(0)))
end

function render()
--	print("render menu")

	renderer:color(0,0,1,1)
	renderer:saveMatrix()
	renderer:transform(vec2(0,0),vec2(1,1),0)

	renderer:text(arial,"Welcome to",40)
	renderer:transform(vec2( (math.sin(runtime/300000)+1)*50 ,50),vec2(1,1),0)
	renderer:text(arial,"Albatross",40)

	renderer:restoreMatrix()
	renderer:color(1,1,1,1)
end

