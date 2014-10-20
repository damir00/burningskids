btn="button"

cars={
	"Van",
	"MiniCooper",
	"car1",
	"beetle",
--	"CubeCar",
--	"car1_rd",
--	"testcar"
}

buttons={}

selected_car=cars[1]

runtime=0

function imageForCar(car)
	return "media/"..car.."/color.png"
end

function init()
	menu=getMenu()
	manager=getManager()
	renderer=getRenderer()
	arial=manager:getFont("arial")

	btn_go=addWidget(menu,btn,vec2(10,180),vec2(135,10))
	btn_back=addWidget(menu,btn,vec2(155,180),vec2(135,10))

	btn_go:setPropString("text","GO!")
	btn_back:setPropString("text","Back")

	btn_y=5
	for i=1,(#cars),1 do
		buttons[i]=addWidget(menu,btn,vec2(10,btn_y),vec2(150,25))
		buttons[i]:setPropString("text",cars[i])
		btn_y=btn_y+30
	end

	--code to load and display images
	node_car=menu:getRootNode():createNode(vec2(0,0),0)
	object_car=renderer:createObject(vec2(100,50))
	object_car:addTexture(manager:getImg(imageForCar(selected_car)))
	node_car:setObject(object_car)
	node_car:setPos(vec2(230,80))

end


function update(delta)

	runtime=runtime+delta

	node_car:setAngle(runtime/100)

	for i=1,(#buttons) do
		if buttons[i]:mouseClicked() then
			selected_car=cars[i]
			object_car:setTexture(0,manager:getImg(imageForCar(selected_car)))
		end
	end

	if btn_go:mouseClicked() then
		setPropString("player_car",selected_car)
		if loadLevel(getPropString("level_id")) then
			menu:close()
			openMenu("hud")
		end
	end

	if btn_back:mouseClicked() then
		menu:close()
	end
end


function render()
	renderer:saveMatrix()
	renderer:transform(vec2(30,130),vec2(1,1),0)
	renderer:text(arial,selected_car,30)
	renderer:restoreMatrix()
end

