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
	},
}


function shieldcontroller:OnActivate()

	self.tickHandler = TickBusHandler(self, 0);
	
	self.transformSender = TransformBusSender(self.entityId);
	
		Debug.Log("shield is on");

	
	activateScaling = Transform.CreateScale(Vector3(80,80,40));
	deactivateScaling = Transform.CreateScale(Vector3(0.01,0.01,0.01));
	self.transformSender:SetLocalTM(activateScaling);
	timer = 0.0;

end

function shieldcontroller:OnDeactivate()
	self.tickHandler:Disconnect();
end



function shieldcontroller:OnTick(deltaTime)
	-- TODO: add shielding logic
	
	if(timer > 1000) then
		self.StateValues.Shield = not self.StateValues.Shield;
		timer = 0;
	end
	timer = timer + 1;
	
	if(self.StateValues.Shield) then
		Debug.Log("yes shield");
		self.transformSender:SetLocalTM(activateScaling);
	end
	
	if (not self.StateValues.Shield) then
		Debug.Log("no shield");
		self.transformSender:SetLocalTM(deactivateScaling);
	end


end