tin="media/tin1/object.xml"
rock="media/rock/object.xml"
car="media/beetle/car.xml"
car2="media/car1_rd/car.xml"
tree="media/tree1/object.xml"
ramp="media/ramp1/object.xml"

cars={
	"MiniCooper",
	"Van",
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

			angle=-math.atan2(meta.left.x,meta.left.y)
			scale=1 --math.abs(math.sin(i/10))*3
			level:addObject(obj,new_pos.x,new_pos.y,angle,static,scale)
			pos=new_pos
			old_i=i
		end
	end
	--last one
	if (first_pos-pos):len_fast()>(spacing2*2) then
		local mi=road:getLength() -( (road:getLength() -old_i) /2 )

		meta=road:getMeta(mi)
		new_pos=meta.center+meta.left*y_offset
		angle=-math.atan2(meta.left.x,meta.left.y)
		level:addObject(obj,new_pos.x,new_pos.y,angle,static)
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
			--b:sensorsInit(21,90,30);
			b:sensorsInit(9,50,50);
		end
	end
end

function init()

	level=getLevel()
	player=getPlayer()
	client=getUserClient()

	populateTrack(50)

	client:setAmbient(vec3(0.0,0.0,0.0))
--	client:setSunColor(vec3(0,0,0))

--[[
	level:addObject("media/ramp1/object.xml",10,0,0)
	level:addObject("media/ramp1/object.xml",-10,0,180)
	level:addObject("media/ramp1/object.xml",0,10,90)
	level:addObject("media/ramp1/object.xml",0,-10,270)
]]--
	meta0=road:getMeta(0);
	offset=0 --meta0.left:len()*0.4
--	roadPlaceObjects(road,ramp,offset,40,false)
--	roadPlaceObjects(road,tin,2,2,false)
--	roadPlaceObjects(road,tin,-2,2,false)

	level:setPropInt("laps",1)

end

function update(delta)
	timeout=timeout+delta
	timer=timer+delta
end

function gui()
end



