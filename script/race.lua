released=false
laps=0
racers_done=0

function init()

	level=getLevel()
	client=getUserClient()
	manager=getManager()

	countdown=4000
	race_time=0

	level:lockCars()
	arial=manager:getFont("arial")

	laps=math.max(level:getPropInt("laps"),1)
end
function update(delta)
	if countdown>-2000 then
		countdown=countdown-delta*5
		if countdown<=0 and not released then
			released=true
			level:releaseCars()
			print("Race Start!")
		end
	end
	if released then
		race_time=race_time+delta

		road=level:getTrack():getRoad(0)
		for i=0,level:getCarSize()-1,1 do
			c=level:getCar(i)
			c:setScore( c:getLap()*road:getLength()+c:getRoadClosest().local_pos )

			if(c:getLap()>=laps and not c:isDone()) then
				c:setFinalScore(100-racers_done)
				racers_done=racers_done+1
				print("Racer "..(i+1).." takes place "..racers_done)
			end
		end
	end
end
function gui()
	client:color(1,1,1,1)
	client:saveMatrix()
	client:transform(vec2(150-100/2,100-60/2),vec2(1,1),0)
	client:color(1,0,0,1)

	if countdown>1000 then
		text(""..math.floor(countdown/1000),60)
	elseif countdown>-2000 then
		text("Go!",70)
	end

	client:color(1,1,1,1)
	client:restoreMatrix()

	client:saveMatrix()
	client:transform(vec2(100,0),vec2(1,1),0)

	if getPlayer():isDone() then
		client:color(1,1,0,1)
		text("Finished "..(getPlayer():getRank()+1),30);
		client:color(1,1,1,1)
	else
		text("Rank: "..(getPlayer():getRank()+1),30)
		client:transform(vec2(120,0),vec2(1,1),0)
		text("Lap " .. math.min(getPlayer():getLap()+1,laps) .. "/" .. laps ,20)
	end

	client:restoreMatrix()

	road=level:getTrack():getRoad(0)
	if released and level:getTrack():getRoadSize()>0 then
		client:saveMatrix()
		client:transform(vec2(10,100),vec2(1,1),0)
		for i=0,level:getCarSize()-1,1 do
			c=level:getCar(i)

			if (race_time % 1000) < 500 then
--				client:text(arial,(i+1)..": car - "..math.floor(c:getScore()),30)
			else
--				client:text(arial,(i+1)..": car - "..math.floor(c:getLap()),30)
			end
			client:transform(vec2(0,30),vec2(1,1),0)
		end
		client:restoreMatrix()
	end
end

