button_text="Button"

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

	arial=manager:getFont("arial")
end

function update(delta)
	p=renderer:getMousePos()
--	print("mouse real "..p.x.." "..p.y)
--	print("mouse "..widget:mousePos().x.." "..widget:mousePos().y)
	if widget:mouseClicked() then
		print("button click!")
	end
end

function render()
	text_size=widget:getSize().y
	text_pos=widget:getPos()
	if widget:mouseInside() then
		--text_size=text_size*1.1
		text_pos.x=text_pos.x+10
	end
	renderQuad(renderer,widget:getPos(),widget:getSize())
	renderer:saveMatrix()
	renderer:transform(text_pos,vec2(1,1),0)
	renderer:text(arial,button_text,text_size)
	renderer:restoreMatrix()
end

function setPropString(name,param)
	if name=="text" then
		button_text=param
	end
end
