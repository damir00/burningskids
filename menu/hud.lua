btn="button"

function init()
	menu=getMenu()
	manager=getManager()
	renderer=getRenderer()
	arial=manager:getFont("arial")

	btn_continue=addWidget(menu,btn,vec2(10,180),vec2(135,10))
	btn_restart =addWidget(menu,btn,vec2(155,180),vec2(135,10))
	btn_stop=addWidget(menu,btn,vec2(10,180),vec2(135,10))

	btn_continue:setPropString("text","Continue")
	btn_restart:setPropString("text","Restart")
	btn_stop:setPropString("text","Abort")

	btn_continue:show(false)
	btn_restart:show(false)
end

function update(delta)

	if getPlayer():isDone() then
		btn_continue:show(true)
		btn_restart:show(true)
		btn_stop:show(false)
	end

	if btn_continue:mouseClicked() then
		unloadLevel()
		menu:close()
	end
	if btn_stop:mouseClicked() then
		unloadLevel()
		menu:close()
	end
end


function render()

end

