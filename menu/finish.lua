btn="button"

function init()
	menu=getMenu()
	manager=getManager()
	renderer=getRenderer()
	arial=manager:getFont("arial")


	btn_back=addWidget(menu,btn,vec2(40,175),vec2(200,25))
	btn_back:setPropString("text","Continue")

	runtime=0
end

function update(delta)
	if btn_back:mouseClicked() then
		menu:close()
	end
end

function render()
--	print("render menu")

	renderer:color(1,1,1,1)
	renderer:saveMatrix()
	renderer:transform(vec2(0,0),vec2(1,1),0)

	renderer:text(arial,"Finish!",60)

	renderer:restoreMatrix()
	renderer:color(1,1,1,1)

end

