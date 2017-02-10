shieldcontroller = 
{
	Properties =
	{
		ShieldActive = {default = true, description = "Whether shield is active.", suffix = ""},
	},
	
	InputValues = 
	{
		IShield = false,
	},
	
	StateValues = 
	{
		Shield = true,
		timer = 0.0,
		
	activateScaling = Transform.CreateScale(Vector3(80,80,40)),
	deactivateScaling = Transform.CreateScale(Vector3(0.01,0.01,0.01)),
	},
}


function shieldcontroller:OnActivate()

	self.tickHandler = TickBusHandler(self, 0);
	
	self.transformSender = TransformBusSender(self.entityId);
	
		Debug.Log("shield is on");

	self.shieldEventId = GameplayNotificationId(self.entityId, "IShield");
	self.shieldHandler = FloatGameplayNotificationBusHandler(self, self.shieldId);

	
	self.transformSender:SetLocalTM(activateScaling);
	

end

function shieldcontroller:OnDeactivate()
	self.tickHandler:Disconnect();
end



function shieldcontroller:OnTick(deltaTime)
	-- TODO: add shielding logic
	
	
	
	
	if(self.StateValues.timer > 2.0) then
		self.StateValues.Shield = not self.StateValues.Shield;
		self.StateValues.timer = 0.0;
	end
	
	self.StateValues.timer = self.StateValues.timer + deltaTime;
	
	if(self.StateValues.Shield) then
		Debug.Log("yes shield");
		self.transformSender:SetLocalTM(self.StateValues.activateScaling);
	end
	
	if (not self.StateValues.Shield) then
		Debug.Log("no shield");
		self.transformSender:SetLocalTM(self.StateValues.deactivateScaling);
	end


end

function shieldcontroller:OnGameplayEventAction(value)
    if (self.shieldHandler:GetCurrentBusId() == self.shieldId) then    
        self.InputValues.IShield = value > 0;
   
	end
end

-- handle input events part 2
function shieldcontroller:OnGameplayEventFailed()
    if (self.shieldHandler:GetCurrentBusId() == self.shieldId) then    
        self.InputValues.IShield = false;
    
	end
end