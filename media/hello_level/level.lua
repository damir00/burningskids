tin="media/tin1/object.xml"
rock="media/rock/object.xml"
car="media/beetle/car.xml"
car2="media/car1_rd/car.xml"
tree="media/tree1/object.xml"

cars={
	"Van",
	"MiniCooper",
	"MiniCooper",
	"car1",
	"beetle",
	"CubeCar",
	"car1_rd",
	"testcar"
}

timeout=0
timer=0

level=0
player=0
client=0

function roadPlaceObjects(road,obj,y_offset,spacing,static)
--[[
	for i=0,road:getLength(),spacing do
		meta=road:getMeta(i)
		pos=meta.center+meta.left*y_offset
		level:addObject(obj,pos.x,pos.y,0,static)
	end
]]

--smarter way
	local resolution=spacing/10
	local spacing2=spacing*spacing
	local pos=vec2(0,0)
	local first_pos=vec2(0,0)
	local old_i=0
	for i=0,road:getLength()-spacing,resolution do
		meta=road:getMeta(i)
		local new_pos=meta.center+meta.left*y_offset

		local dist=new_pos-pos
		if i==0 or dist:len_fast()>spacing2 then
			if i==0 then
				first_pos=pos
			end
			level:addObject(obj,new_pos.x,new_pos.y,0,static)
			pos=new_pos
			old_i=i
		end
	end
	--last one
	if (first_pos-pos):len_fast()>(spacing2*2) then
		local mi=road:getLength() -( (road:getLength() -old_i) /2 )

		meta=road:getMeta(mi)
		new_pos=meta.center+meta.left*y_offset
		level:addObject(obj,new_pos.x,new_pos.y,0,static)
	end

end

function populateTrack(num_cars)

	if level:getTrack():getRoadSize()==0 then return end

	road=level:getTrack():getRoad(0)
	for i=0,num_cars-1,1 do
		meta=road:getMeta(20+i*4)

		if i%2==0 then
			pos=meta.center-meta.left*0.4
			body=car
		else
			pos=meta.center+meta.left*0.4
			body=car2
		end

		body="media/"..cars[(i)%(#cars)+1].."/car.xml"

		angle=math.pi-math.atan2(meta.left.x,meta.left.y) --*57.325

		b=level:addCar(body,pos.x,pos.y,angle)
		if i>0 then
			b:sensorsInit(21,90,30);
		end
	end
end

function init()

	level=getLevel()
	player=getPlayer()
	client=getUserClient()

	print ("Welcome to Hello Track!")

	player_car_id=getPropString("player_car")
	if not (player_car_id=="") then
		cars[1]=player_car_id
	end

	populateTrack(6)

	cube_size=35
	cube_items=cube_size/5
	cube_x=0
	cube_y=0

	level:setPropInt("laps",2)

--[[
	advance=cube_size/cube_items
	for i=0,cube_size,advance do
		level:addObject(tin,i-cube_size/2+cube_x,cube_size/2+cube_y,0)
	end
	for i=0,cube_size,advance do
		level:addObject(tin,i-cube_size/2+cube_x,-cube_size/2+cube_y,180)
	end
	for i=1,cube_size-1,advance do
		level:addObject(tin,cube_size/2+cube_x,i-cube_size/2+cube_y,90)
	end
	for i=1,cube_size-1,advance do
		level:addObject(tin,-cube_size/2+cube_x,i-cube_size/2+cube_y,270)
	end
]]
	for i=1,50,1 do
--		level:addCar(car,math.random(),math.random(),0);
--		level:addCar(car,0,0,0);
	end

	road=level:getTrack():getRoad(0)
--	for i=0,road:getLength(),10 do
--		mpos=road:getMeta(i)
--		pos=mpos.center+mpos.left*(math.random()*2-1)
	--	level:addObject(tin,pos.x,pos.y,0)
--	end

	roadPlaceObjects(road,tin,-2,2,true)
	roadPlaceObjects(road,tree,2,10,true)

	for i=1,10,1 do
		mpos=road:getMeta(road:getLength()*i/10)
		pos=mpos.center+mpos.left*(2+math.random()*2)
		level:addObject(rock,pos.x,pos.y,math.random(360))
	end	

--	c=road:getMeta(10).center
--	level:addCar(car2,c.x,c.y,0)

	amb=0.0
	sun=1
--	client:setAmbient(vec3(amb,amb,amb))
--	client:setSunColor(vec3(sun,sun,sun))
--	client:setSunDir(vec3(0.0,0.5,0.5))

end

function update(delta)
	timeout=timeout+delta
	timer=timer+delta

--	a=math.sin(timer/1000)/2+0.5
--	client:setAmbient(vec3(0.2*a,0.2*a,a))
--	client:setSunColor(vec3(a,a,a))

--	s=timer/10000
--	client:setSunDir(vec3(math.sin(s),math.cos(s),1))

end

function gui()
end



