
function renderQuad(renderer,pos,size)
	renderer:moveTo(pos)
	renderer:lineTo(vec2(pos.x+size.x,pos.y))
	renderer:lineTo(vec2(pos.x+size.x,pos.y+size.y))
	renderer:lineTo(vec2(pos.x,pos.y+size.y))
	renderer:lineTo(pos)
end

function init()
	widget=getWidget()
	renderer=getRenderer()
	manager=getManager()
	value=0
end

function update(delta)
	p=renderer:getMousePos()

	if widget:mouseDown() then
		value=widget:mousePos().x/widget:getSize().x
	end

end

function render()
	text_size=widget:getSize().y
	text_pos=widget:getPos()

	renderQuad(renderer,widget:getPos(),widget:getSize())
	renderQuad(renderer,widget:getPos(),vec2(value*widget:getSize().x,widget:getSize().y))
end

function setPropString(name,param)
end
