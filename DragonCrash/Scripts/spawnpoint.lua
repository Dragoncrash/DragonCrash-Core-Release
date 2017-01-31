spawnpoint = {

properties = {

}



}




function spawnpoint:OnActivate()
Debug.Log("Spawn active");


 -- Register our tick bus handler
    self.tickBusHandler = TickBusHandler(self)

    -- Make a sender for sending events to the spawner component attached to this entity.
    self.SpawnerSender = SpawnerComponentRequestBusSender(self.entityId)

    -- Register our handlers to receive notification from the spawner attached to this entity.
    self.spawnerNotiBusHandler = SpawnerComponentNotificationBusHandler(self, self.entityId)

-- Used to track regular spawn times
    self.timer = 0
    self.spawnTimer = 3.0

end


function spawnpoint:OnDeactivate()

 -- Disconnect our tick notification bus handler
    if (self.tickBusHandler ~= nil) then
        self.tickBusHandler:Disconnect()
        self.tickBusHandler = nil
    end

    -- Disconnect our spawner notificaton 
    if self.spawnerNotiBusHandler ~= nil then
        self.spawnerNotiBusHandler:Disconnect()
        self.spawnerNotiBusHandler = nil
    end
Debug.Log("Spawn Deactive");
end


function spawnpoint:SpawnSlice()
    -- Use the SpawnerComponentRequestBus to send a Spawn Event to the entity this script is attached to.
    -- If the entity has a spawner component and it has a valid dynamic slice in it's Slice field, the
    -- Slice will be instantiated at the location of the spawner.
    self.SpawnerSender:Spawn()
end 




function spawnpoint:OnTick(deltaTime, timePoint)
 self.timer = self.timer + deltaTime


if self.timer >= self.spawnTimer then
            self:SpawnSlice()
            self.timer = 0
	end
  
Debug.Log(self.timer);
  
end

function spawnpoint:OnEntitySpawned(sliceTicket,entityId)
    -- Let's give our spawned entity a bit of upward velocity, just so we can see that this is indeed working.
    PhysicsSender = PhysicsComponentRequestBusSender(entityId)
    PhysicsSender:SetVelocity(Vector3(0.0,0.0,10.0))
end