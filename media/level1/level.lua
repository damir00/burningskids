tin="media/tin1/object.xml"
car="media/car1/car.xml"
car2="media/car1_rd/car.xml"

timeout=0
timer=0

level=0
client=0


--debug functs to see track surface info
function render_cross(pos,size)
	client:moveTo(pos-vec2(size,size))
	client:lineTo(pos+vec2(size,size))
	client:moveTo(pos+vec2(size,-size))
	client:lineTo(pos+vec2(-size,size))
end
function road_surface_info(pos)
	info=level:getTrack():surfaceQuery(pos)

	c=info.x
	client:color(c,0,1-c,1)
	render_cross(pos,0.8)
end
function render_track_surface_info(car)
	road=level:getTrack():getRoad(0)
	meta=car:getRoadClosest()

	grid_size2=20/2
	grid_adv=1

	for x=meta.center.x-grid_size2,meta.center.x+grid_size2,grid_adv do
		for y=meta.center.y-grid_size2,meta.center.y+grid_size2,grid_adv do
			road_surface_info(vec2(x,y))
		end
	end

end


function populateTrack(num_cars)

	if level:getTrack():getRoadSize()==0 then return end

	road=level:getTrack():getRoad(0)
	for i=0,num_cars-1,1 do
		meta=road:getMeta(20+i*1)
		
		if i%2==0 then
			pos=meta.center-meta.left*0.4
			body=car
		else
			pos=meta.center+meta.left*0.4
			body=car2
		end

		angle=math.atan2(meta.left.x,meta.left.y) --*57.325

		level:addCar(body,pos.x,pos.y,angle)
	end
--sensors
--	car->sensorsInit(21,90,30);


end

function init()

	level=getLevel()
	client=getUserClient()

	print ("Hello Track1!")

	cube_size=40
	cube_items=cube_size/5
	cube_x=0
	cube_y=10

	populateTrack(3)

	advance=cube_size/cube_items
	for i=0,cube_size,advance do
		level:addObject(tin,i-cube_size/2+cube_x,cube_size/2+cube_y,0)
	end
	for i=0,cube_size,advance do
		level:addObject(tin,i-cube_size/2+cube_x,-cube_size/2+cube_y,180,true)
	end
	for i=1,cube_size-1,advance do
		level:addObject(tin,cube_size/2+cube_x,i-cube_size/2+cube_y,90,true)
	end
	for i=1,cube_size-1,advance do
		level:addObject(tin,-cube_size/2+cube_x,i-cube_size/2+cube_y,270)
	end

	for i=1,50,1 do
--		level:addCar(car,math.random(),math.random(),0);
--		level:addCar(car,0,0,0);
	end

	road=level:getTrack():getRoad(0)
	for i=0,road:getLength(),10 do
		mpos=road:getMeta(i)
		pos=mpos.center+mpos.left*(math.random()*2-1)
	--	level:addObject(tin,pos.x,pos.y,0)
	end

	c=road:getMeta(10).center
--	level:addCar(car2,c.x,c.y,0)

	print ("init ended!")
end

function update(delta)
--	player:turn(0.2)
	timeout=timeout+delta
	timer=timer+delta
	if timeout>500 then
		level:addObject(tin,30+math.random(),math.random()+10,0);
--		level:addCar(car,math.random(),math.random()+10,math.random()*360);
		timeout=0
	end

	a=math.sin(timer/1000)/2+0.5
	client:setAmbient(vec3(0.2*a,0.2*a,a))
	client:setSunColor(vec3(a,a,a))

	s=timer/10000
	client:setSunDir(vec3(math.sin(s),math.cos(s),1))

end

function gui()
end

function render()
	render_track_surface_info(getPlayer());
end

