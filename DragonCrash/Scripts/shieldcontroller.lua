shieldcontroller = 
{
	Properties =
	{
		ShieldActive = {default = true, description = "Whether shield is active.", suffix = ""},
	},
	
	InputValues = 
	{
		IShield = true,
	},
	
	StateValues = 
	{
		Shield = true,
	},
}



function shieldcontrolller:HandleSpawn()
	Debug.Log("shield is livin");

	-- get respawn transform
	local respawnTranform = Transform.CreateTranslation(Vector3(0,0,500));
	
	-- move to respawn transform
	self.transformSender:SetWorldTM(respawnTranform);
end

function shieldcontroller:OnTick(deltaTime)
	-- TODO: add shielding logic
	Debug.Log("shield is on");
	-- move to respawn transform
	self.physicsSender:SetVelocity(Vector3(5,0,0));
	self.physicsSender:SetAngularVelocity(Vector3(0,0,0));
end