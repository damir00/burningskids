function set_speed(car,speed)
	if car:getSpeed()<speed then 
		car:accelerate(1)
	else
		if car:getSpeed()>speed*1.4 then car:brake(1) 
		else car:accelerate(0)
		end
	end
end

function ram_stuff(car)
	--check the sensors
	if car:getSensorEnabled() then
		--find closest obstacle
		min_angle=0
		min_dist=1
		for i=0,car:getSensorPixels()-1,1 do
			if (car:getSensorData(i)) <min_dist then
				min_dist=car:getSensorData(i)
				min_angle=car:getSensorAngle(i)
			end
		end
		if min_dist<0.9 then
			car:turn(min_angle/40)
		end
	end
end

function avoid_stuff_sensor_score(car,index,angle,dist,target_angle)
	if index==0 or index>=car:getSensorPixels()-1 then
		return dist * ( 1 / (math.abs(angle-target_angle)/100+1))
	end
	return dist * car:getSensorData(index-1)*car:getSensorData(index+1) * ( 1 / (math.abs(angle-target_angle)/100+1))
end
function avoid_stuff(car,default_turn)
	local damp=10; --35*(car:getSpeed()/0.02)

	if car:getSensorEnabled() then
		angle_dif=math.abs(car:getSensorAngle(0)-car:getSensorAngle(1))/2

		max_score=0;
		max_angle=0;
		for i=0,car:getSensorPixels()-1,1 do
			this_angle=car:getSensorAngle(i)
			this_score=avoid_stuff_sensor_score(car,i,this_angle,car:getSensorData(i),default_turn)
			if this_score > max_score then
				max_score=this_score
				max_angle=this_angle
			end
		end
		if math.abs(max_angle-default_turn) < angle_dif then
			car:turn(default_turn/damp)
		else
			car:turn(max_angle/damp)
		end
	else
		car:turn(default_turn/damp)
	end
--	print("turn "..default_turn)
end

desired_speed=0.03+math.random()*0.01 --math.random()/2+0.25;

function follow_point(car,point)
	heading=car:getHeadingVect()
	pos=car:getPos()
	point=point-pos

	point_rel=point:unrotate(heading)

	--car:turn(point_rel.y/4)

	--ram_stuff(car)
	local turn=math.atan2(point_rel.y,point_rel.x)*57.325
	avoid_stuff(car,turn)

	--set_speed(car, math.min(math.max(math.abs(1/point_rel.y),0.1),1.5) )

	local turn2=1-math.max(0.2,math.min(1,turn/90))
	local speed=desired_speed * turn2

--print("current "..car:getSpeed().." desired "..speed)

	set_speed(car,speed)
end

road_adv=5	--advance from current position in meters

function follow_road(car,road_id)
	road=getLevel():getTrack():getRoad(0)
	road_len=road:getLength()
	my_pos=car:getPos()
	closest_meta=car:getRoadClosest()
	adv2=closest_meta.local_pos+road_adv*3 -- * (car:getSpeed()/0.03*1)
	if adv2>road_len then
		adv2=adv2-road_len
	end
	closest2_meta=road:getMeta( adv2 )

	--print(adv2)

	offset=vec2(0,0)

	if road_adv > 0 then offset=closest2_meta.left*0.5
	else offset=offset-closest2_meta.left*0.5 end

	if road_adv > 0 and closest_meta.local_pos > road_len*0.99 then
		if not road:isCyclic() then
			road_adv=-1
		end
	end

	if road_adv < 0 and closest_meta.local_pos < road_len*0.01 then
		road_adv=1
	end

	follow_point(car,closest2_meta.center+offset)
end

function follow_car(car,target)
	follow_point(car,target:getPos())
end


--reversing when stuck
speed_timer=0
reversing=false


function update(car,delta)

	if(car:isDone()) then
		desired_speed=0.009
		road_adv=3
	end

	if(reversing) then
		speed_timer=speed_timer-delta
		if speed_timer<=0 then
			speed_timer=0
			reversing=false
		else
			car:accelerate(-1); ---0.00016)
		end
	else
		if(car:getSpeed()<0.0005) then
			speed_timer=speed_timer+delta

			if(speed_timer>1000) then
				--reverse for 1 second
				reversing=true
				speed_timer=1000
			end
		else
			speed_timer=0
		end
		follow_road(car,0)
	end

--	follow_car(car,getPlayer())

end




