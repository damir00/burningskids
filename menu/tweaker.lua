btn="button"

function init()
	menu=getMenu()
	manager=getManager()
	renderer=getRenderer()
	arial=manager:getFont("arial")

--	slider1=addWidget(menu,"slider",vec2(10,100),vec2(100,10))
--	slider2=addWidget(menu,"slider",vec2(10,120),vec2(100,10))
--	slider3=addWidget(menu,"slider",vec2(10,140),vec2(100,10))

	tweaker=GameTweaker()
end

function update(delta)
--	tweaker:setTyreOutForceMult(0.0001)
	tweaker:setTyreGripForceMult(0.0000)
--	tweaker:setTyreSlideTreshold(0.5)
end


function render()

end

