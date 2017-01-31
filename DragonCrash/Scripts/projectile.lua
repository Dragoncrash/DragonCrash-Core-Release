--Needs 



projectile = {

properties = {

}


}


function projectile:OnActivate()
Debug.Log("Projectile Activated");
LyShineLua.ShowMouseCursor(true)
end

function projectile:OnDeactivate()
Debug.Log("Projectile Deactivated");
end