-- Controls a DRAGON!
-- TODO: Enable Net Synchronization
dragoncontroller = 
{
	-- properties that show up in the LY editor
	Properties = 
	{
		FlightSpeed = { default = 100.0, description = "How fast the dragon flies.", suffix = " m/sec" },
		YawTurnSpeed = { default = 180.0, description = "How fast the dragon turns left and right.", suffix = " degrees/sec"},
		PitchTurnSpeed = { default = 180.0, description = "How fast the dragon turns up and down.", suffix = " degrees/sec"},
		PitchAngleLimit = { default = 70.0, description = "How far the dragon can pitch up and down.", suffix = " degrees" },
		BoostAcceleration = { default = 50.0, description = "How fast the dragon accelerates while boosting.", suffix = " m/sec^2"},
		BoostMaxSpeed = { default = 300.0, description = "How fast the dragon can fly while boosting.", suffix = " m/sec"},
		EnergyMax = { default = 8.0, description = "How long the dragon can boost at max energy.", suffix = " sec"},
		EnergyRechargeTime = { default = 4.0, description = "How long the dragon takes to recharge all energy.", suffix = " sec"},
		EnergyRechargeDelay = { default = 2.0, description = "How long the dragon must wait after boosting to recharge energy.", suffix = " sec"},
		HealthMax = { default = 4.0, description = "The dragon's max health.", suffix = "" },
		RespawnTime = { default = 5.0, description = "How long the dragon take to respawn.", suffix = " sec" },
		-- TODO: add new editor-exposed properties here
	},
	
	-- values from player input
	InputValues = 
	{
		MainYaw = 0.0,
		MainPitch = 0.0,
		SecondaryYaw = 0.0,
		SecondaryPitch = 0.0,
		Ascend = 0.0,
		Boost = false,
		Shield = false,
		Aim = false,
		MainFire = false,
	},
	
	-- values to keep track of dragon state
	StateValues =
	{
		Yaw = 0.0,					-- current yaw (horizontal) rotation
		Pitch = 0.0,				-- current pitch (vertical) rotation
		IsBoosting = false,			-- is dragon currently boosting?
		IsEnergyExhausted = false,	-- did dragon use all energy and need to recharge
		EnergyRemaining = 0.0,		-- seconds of boost energy remaining
		EnergyRechargeTimer = 0.0,	-- how long since last energy use
		IsDead = false,				-- if the dragon is currently dead
		HealthCurrent = 0.0,		-- how much health the dragon has
		RespawnTimer = 0.0,			-- how long the dragons been dead
		-- TODO: add new state values here
	},
}

-- Dragon Event Handlers:

-- called when dragon initially spawns or respawns
function dragoncontroller:HandleSpawn()
	Debug.Log("dragon is livin");

	-- set state variables
	self.StateValues.Yaw = 0.0;
	self.StateValues.Pitch = 0.0;
	self.StateValues.IsBoosting = false;
	self.StateValues.IsEnergyExhausted = false;
	self.StateValues.EnergyRemaining = self.Properties.EnergyMax;
	self.StateValues.EnergyRechargeTimer = 0.0;
	self.StateValues.HealthCurrent = self.Properties.HealthMax;
	
	-- get respawn transform
	local respawnTranform = Transform.CreateTranslation(Vector3(0,0,500));
	
	-- move to respawn transform
	self.transformSender:SetWorldTM(respawnTranform);
end

-- called when the dragon's hp reaches 0
function dragoncontroller:HandleDeath()
	Debug.Log("dragon is kill");
	
	-- stop movement
	self.physicsSender:SetVelocity(Vector3(0,0,0));
	self.physicsSender:SetAngularVelocity(Vector3(0,0,0));
	
	-- TODO: Add visual death indicators
end


-- Tick Handlers:

-- movement logic to be handled every tick
function dragoncontroller:HandleMovementTick(deltaTime)
	-- update rotation state
	self.StateValues.Yaw = self.StateValues.Yaw - self.InputValues.MainYaw * self.Properties.YawTurnSpeed * deltaTime;
	self.StateValues.Pitch = self.StateValues.Pitch - self.InputValues.MainPitch * self.Properties.PitchTurnSpeed * deltaTime;
	local roll = 0.0; -- TODO: add banking for horizontal turning

	-- constrain pitch to avoid flipping
	if (self.StateValues.Pitch > self.Properties.PitchAngleLimit) then
		self.StateValues.Pitch = self.Properties.PitchAngleLimit;
	elseif (self.StateValues.Pitch < -self.Properties.PitchAngleLimit) then
		self.StateValues.Pitch = -self.Properties.PitchAngleLimit;
	end
		
	-- get transform
	local transform = self.transformSender:GetWorldTM();
		
	-- extract current rotation
	local currentRotation = Quaternion.CreateFromTransform(transform);
	
	-- create desired rotation using Yaw, Pitch, Roll
	local desiredRotation = Quaternion.CreateRotationZ(self.StateValues.Yaw) 
						  * Quaternion.CreateRotationX(self.StateValues.Pitch) 
						  * Quaternion.CreateRotationY(roll);
						  
	-- compute difference between current and desired
	local difference = desiredRotation * currentRotation:GetInverseFast();
	
	-- convert difference to yaw pitch roll and form angular velocity
	local angularVelocity = Vector3( 
		  math.atan(2 * (difference.w * difference.x + difference.y * difference.z), 1 - 2 * (difference.x^2 + difference.y^2)) -- roll
		, math.asin(2 * (difference.w * difference.y - difference.z * difference.x))											-- yaw
		, math.atan(2 * (difference.w * difference.z + difference.x * difference.y), 1 - 2 * (difference.y^2 + difference.z^2)) -- pitch
		);
	angularVelocity = angularVelocity * 0.8 / deltaTime;
	
	-- apply angular impulse to reach desired rotation
	self.physicsSender:SetAngularVelocity(angularVelocity);
	
	-- apply forward impulse
	local forwardDirection = transform:GetColumn(1);
	forwardDirection:Normalize();
	self.physicsSender:SetVelocity(forwardDirection * self.Properties.FlightSpeed);
end

-- dragoncrash logic to be handled every tick
function dragoncontroller:HandleDragonCrashTick(deltaTime)
	-- TODO: add crashing logic
end

-- shield logic to be handled every tick
function dragoncontroller:HandleShieldingTick(deltaTime)
	-- TODO: add shielding logic
	
	-- use as self-harm button for respawn testing
	if (self.InputValues.Shield) then
		self.StateValues.HealthCurrent = self.StateValues.HealthCurrent - deltaTime;
	end
end

-- projectile firing logic to be handled every tick
function dragoncontroller:HandleFiringTick(deltaTime)
	-- TODO: add firing logic
end


-- Collision Handlers:

-- handler for collision with dragon (runs on both dragons!)
function dragoncontroller:HandleDragonCollision(collision)
	-- TODO: add dragon collision logic
end

-- handler for collision with attack projectile
function dragoncontroller:HandleAttackCollision(collision)
	-- TODO: add attack collision logic
end


-- Bus Events:

-- called when script is activated
function dragoncontroller:OnActivate()
	-- convert rotation speeds to radians/sec
	self.Properties.YawTurnSpeed = Math.DegToRad(self.Properties.YawTurnSpeed);
	self.Properties.PitchTurnSpeed = Math.DegToRad(self.Properties.PitchTurnSpeed);
	self.Properties.PitchAngleLimit = Math.DegToRad(self.Properties.PitchAngleLimit);
	
	-- tick handler
	self.tickHandler = TickBusHandler(self, 0);
	
	-- physics handler (for collisions)
	self.physicsHandler = PhysicsComponentNotificationBusHandler(self, self.entityId);
	
	-- main yaw input handler
	self.mainYawEventId = GameplayNotificationId(self.entityId, "main yaw");
	self.mainYawHandler = FloatGameplayNotificationBusHandler(self, self.mainYawEventId);
	
	-- main pitch input handler
	self.mainPitchEventId = GameplayNotificationId(self.entityId, "main pitch");
	self.mainPitchHandler = FloatGameplayNotificationBusHandler(self, self.mainPitchEventId);
	
	-- secondary yaw input handler
	self.secondaryYawEventId = GameplayNotificationId(self.entityId, "secondary yaw");
	self.secondaryYawHandler = FloatGameplayNotificationBusHandler(self, self.secondaryYawEventId);
	
	-- secondary pitch input handler
	self.secondaryPitchEventId = GameplayNotificationId(self.entityId, "secondary pitch");
	self.secondaryPitchHandler = FloatGameplayNotificationBusHandler(self, self.secondaryPitchEventId);
	
	-- ascend input handler
	self.ascendEventId = GameplayNotificationId(self.entityId, "ascend");
	self.ascendHandler = FloatGameplayNotificationBusHandler(self, self.mainYawEventId);
	
	-- boost input handler
	self.boostId = GameplayNotificationId(self.entityId, "boost");
	self.boostHandler = FloatGameplayNotificationBusHandler(self, self.boostId);
	
	-- shield input handler
	self.shieldId = GameplayNotificationId(self.entityId, "shield");
	self.shieldHandler = FloatGameplayNotificationBusHandler(self, self.shieldId);
	
	-- aim input handler
	self.aimId = GameplayNotificationId(self.entityId, "aim");
	self.aimHandler = FloatGameplayNotificationBusHandler(self, self.aimId);
	
	-- main fire input handler
	self.mainFireId = GameplayNotificationId(self.entityId, "main fire");
	self.mainFireHandler = FloatGameplayNotificationBusHandler(self, self.mainFireId);
	
	-- entity senders
	self.transformSender = TransformBusSender(self.entityId);
	self.physicsSender = PhysicsComponentRequestBusSender(self.entityId);
	
	-- call spawn event
	self:HandleSpawn();
end

-- called each tick
function dragoncontroller:OnTick(deltaTime)
	-- check if dragon has died
	if (not self.StateValues.IsDead and self.StateValues.HealthCurrent <= 0.0) then
		self:HandleDeath();
		self.StateValues.IsDead = true;
		self.StateValues.RespawnTimer = 0.0;
	end
	
	if (not self.StateValues.IsDead) then
		-- perform dragon logic if alive
		self:HandleMovementTick(deltaTime);
		self:HandleDragonCrashTick(deltaTime);
		self:HandleShieldingTick(deltaTime);
		self:HandleFiringTick(deltaTime);
	else
		-- handle respawn logic
		self.StateValues.RespawnTimer = self.StateValues.RespawnTimer + deltaTime;
		if (self.StateValues.RespawnTimer >= self.Properties.RespawnTime) then
			self:HandleSpawn();
			self.StateValues.IsDead = false;
		end
	end
end

-- called when script is deactivated
function dragoncontroller:OnDeactivate()
	self.tickHandler:Disconnect();
end

-- called on collision
function dragoncontroller:OnCollision(collision)
	-- get tag request bus for entity collided with
	local collisionTagSender = TagComponentRequestBusSender(collision.entity);
	
	-- send to log
	if (self.collisionCount == nil) then
		self.collisionCount = 0;
	end
	self.collisionCount = self.collisionCount + 1;
	Debug.Log("collision " .. self.collisionCount);
	
	
	if (collisionTagSender) then
		-- check for collision between dragons
		if (collisionTagSender:HasTag( Crc32("dragon")) ) then
			self:HandleDragonCollision(collision);
		end
		
		-- check for collision with attack projectile
		if (collisionTagSender:HasTag( Crc32("attack")) ) then
			self:HandleAttackCollision(collision);
		end
	end
end

-- handle input events
function dragoncontroller:OnGameplayEventAction(value)
    if (self.mainYawHandler:GetCurrentBusId() == self.mainYawEventId) then    
        self.InputValues.MainYaw = value;
    elseif (self.mainPitchHandler:GetCurrentBusId() == self.mainPitchEventId) then
        self.InputValues.MainPitch = value;
    elseif (self.secondaryYawHandler:GetCurrentBusId() == self.secondaryYawEventId) then    
        self.InputValues.SecondaryYaw = value;
    elseif (self.secondaryPitchHandler:GetCurrentBusId() == self.secondaryPitchEventId) then
        self.InputValues.SecondaryPitch = value;
    elseif (self.ascendHandler:GetCurrentBusId() == self.ascendEventId) then
        self.InputValues.Ascend = value;
	elseif (self.boostHandler:GetCurrentBusId() == self.boostId) then
        self.InputValues.Boost = value > 0;
	elseif (self.shieldHandler:GetCurrentBusId() == self.shieldId) then
        self.InputValues.Shield = value > 0;
	elseif (self.aimHandler:GetCurrentBusId() == self.aimId) then
        self.InputValues.Aim = value > 0;
	elseif (self.mainFireHandler:GetCurrentBusId() == self.mainFireId) then
        self.InputValues.MainFire = value > 0;
	end
end

-- handle input events part 2
function dragoncontroller:OnGameplayEventFailed()
    if (self.mainYawHandler:GetCurrentBusId() == self.mainYawEventId) then    
        self.InputValues.MainYaw = 0.0;
    elseif (self.mainPitchHandler:GetCurrentBusId() == self.mainPitchEventId) then
        self.InputValues.MainPitch = 0.0;
    elseif (self.secondaryYawHandler:GetCurrentBusId() == self.secondaryYawEventId) then    
        self.InputValues.SecondaryYaw = 0.0;
    elseif (self.secondaryPitchHandler:GetCurrentBusId() == self.secondaryPitchEventId) then
        self.InputValues.SecondaryPitch = 0.0;
    elseif (self.ascendHandler:GetCurrentBusId() == self.ascendEventId) then
        self.InputValues.Ascend = 0.0;
	elseif (self.boostHandler:GetCurrentBusId() == self.boostId) then
        self.InputValues.Boost = false;
	elseif (self.shieldHandler:GetCurrentBusId() == self.shieldId) then
        self.InputValues.Shield = false;
	elseif (self.aimHandler:GetCurrentBusId() == self.aimId) then
        self.InputValues.Aim = false;
	elseif (self.mainFireHandler:GetCurrentBusId() == self.mainFireId) then
        self.InputValues.MainFire = false;
	end
end