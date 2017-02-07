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

end

function shieldcontroller:OnDeactivate()
	self.tickHandler:Disconnect();
end



function shieldcontroller:OnTick(deltaTime)
	-- TODO: add shielding logic
	
	if(self.StateValues.Shield) then
		
	end
	
	if (not self.InputValues.IShield) then
		self.transformSender:SetLocalTM(deactivateScaling);
	end


end