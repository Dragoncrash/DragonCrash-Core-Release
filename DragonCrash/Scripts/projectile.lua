projectile = {

	properties = {

	}
}

function projectile:OnTick(deltaTime, timePoint)
	local transform = self.TransformSender:GetWorldTM();
	local forwardDirection = transform:GetColumn(1);
	self.PhysicsSender:SetVelocity(forwardDirection * 100);
	
	self.lifespan= self.lifespan + deltaTime;
	--Debug.Log(self.lifespan);

	--if 50 seconds pass then delete itself

	if ( self.lifespan>=self.endlife )  then
		Debug.Log("end");
		self.GameEntitySender:DestroyGameEntity(self.entityId);
	end

end

function projectile:OnActivate()
	--tick handler
	self.tickBusHandler = TickBusHandler(self)

	--Debug.Log("Projectile Activated");
	
	-- physics handler
	self.physicsHandler = PhysicsComponentNotificationBusHandler(self, self.entityId);

	self.TransformSender = TransformBusSender(self.entityId);
	
	 --Make a sender for sending events to the Physics component attached to this entity.
	self.PhysicsSender = PhysicsComponentRequestBusSender(self.entityId);
	
	--GameEntity handler
	self.GameEntitySender=GameEntityContextRequestBusSender(self.entityId);
	
	self.lifespan=0;
	self.endlife=3;

	Debug.Log("test projectile");

	--Debug.Log(self.lifespan);
end

function projectile:OnDeactivate()
	Debug.Log("Projectile Deactivated");

-- Disconnect our tick notification bus handler
    if (self.tickBusHandler ~= nil) then
        self.tickBusHandler:Disconnect();
        self.tickBusHandler = nil;
    end

    -- Disconnect our spawner notificaton 
    if self.PhysicsNotiBusHandler ~= nil then
        self.PhysicsNotiBusHandler:Disconnect();
        self.PhysicsNotiBusHandler = nil;
    end

--Debug.Log(self.lifespan);
end

function projectile:OnCollision(collision)
	Debug.Log("collision");
	self.GameEntitySender:DestroyGameEntity(self.entityId);
end