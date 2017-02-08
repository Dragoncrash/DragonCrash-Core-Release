#include "StdAfx.h"

#include <sstream>
#include <AzCore/Math/Crc.h>
#include <AzCore/EBus/EBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzFramework/Math/MathUtils.h>
#include <IPhysics.h>
#include <MathConversion.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>
#include <LyShine/Bus/World/UiCanvasRefBus.h>
#include <LyShine/Bus/UiCanvasBus.h>
#include <LyShine/Bus/UiTextBus.h>
#include <DragonCrashCollectiblesBus.h>

#include "DragonCrashControllerComponent.h"

namespace DragonCrashController
{
	AZ::Vector3 QuatToYPR(AZ::Quaternion quat)
	{
		return AZ::Vector3(
			atan2(2.0f * (float)(quat.GetW() * quat.GetX() + quat.GetY() * quat.GetZ()), 1.0f - 2.0f * (float)(quat.GetX() * quat.GetX() + quat.GetY() * quat.GetY())),
			asin(2.0f * (float)(quat.GetW() * quat.GetY() - quat.GetZ() * quat.GetX())),
			atan2(2.0f * (float)(quat.GetW() * quat.GetZ() + quat.GetX() * quat.GetY()), 1.0f - 2.0f * (float)(quat.GetY() * quat.GetY() + quat.GetZ() * quat.GetZ()))
		);
	}

	void DragonCrashControllerComponent::Reflect(AZ::ReflectContext* context)
	{
		AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
		if (serialize)
		{
			serialize->Class<DragonCrashControllerComponent, AZ::Component>()
				->Version(1)
				->Field("HealthMax", &DragonCrashControllerComponent::m_healthMax)
				->Field("RespawnTime", &DragonCrashControllerComponent::m_respawnTime)
				->Field("PitchTurnLimit", &DragonCrashControllerComponent::m_pitchTurnLimit)
				->Field("FlightAcceleration", &DragonCrashControllerComponent::m_flightAcceleration)
				->Field("FlightMaxSpeed", &DragonCrashControllerComponent::m_flightMaxSpeed)
				->Field("FlightAscendAngle", &DragonCrashControllerComponent::m_flightAscendAngle)
				->Field("FlightYawTurnSpeed", &DragonCrashControllerComponent::m_flightYawTurnSpeed)
				->Field("FlightPitchTurnSpeed", &DragonCrashControllerComponent::m_flightPitchTurnSpeed)
				->Field("HoverSpeed", &DragonCrashControllerComponent::m_hoverSpeed)
				->Field("HoverAscendSpeed", &DragonCrashControllerComponent::m_hoverAscendSpeed)
				->Field("HoverYawTurnSpeed", &DragonCrashControllerComponent::m_hoverYawTurnSpeed)
				->Field("HoverPitchTurnSpeed", &DragonCrashControllerComponent::m_hoverPitchTurnSpeed)
				->Field("BoostAcceleration", &DragonCrashControllerComponent::m_boostAcceleration)
				->Field("BoostMaxSpeed", &DragonCrashControllerComponent::m_boostMaxSpeed)
				->Field("BoostTurnFactor", &DragonCrashControllerComponent::m_boostTurnFactor)
				->Field("EnergyMax", &DragonCrashControllerComponent::m_energyMax)
				->Field("EnergyRechargeTime", &DragonCrashControllerComponent::m_energyRechargeTime)
				->Field("EnergyRechargeDelay", &DragonCrashControllerComponent::m_energyRechargeDelay)
				->Field("InputMainYaw", &DragonCrashControllerComponent::m_inputMainYaw)
				->Field("InputMainPitch", &DragonCrashControllerComponent::m_inputMainPitch)
				->Field("InputSecondaryYaw", &DragonCrashControllerComponent::m_inputSecondaryYaw)
				->Field("InputSecondaryPitch", &DragonCrashControllerComponent::m_inputSecondaryPitch)
				->Field("InputAscend", &DragonCrashControllerComponent::m_inputAscend)
				->Field("InputModeSwitch", &DragonCrashControllerComponent::m_inputModeSwitch)
				->Field("InputBoost", &DragonCrashControllerComponent::m_inputBoost)
				->Field("InputShield", &DragonCrashControllerComponent::m_inputShield)
				->Field("InputAim", &DragonCrashControllerComponent::m_inputAim)
				->Field("InputMainFire", &DragonCrashControllerComponent::m_inputMainFire)
				->Field("InputSecondaryFire", &DragonCrashControllerComponent::m_inputSecondaryFire)
				->Field("CurrentState", &DragonCrashControllerComponent::m_currentState)
				->Field("HealthCurrent", &DragonCrashControllerComponent::m_healthCurrent)
				->Field("RespawnTimer", &DragonCrashControllerComponent::m_respawnTimer)
				->Field("ModeSwitchTimer", &DragonCrashControllerComponent::m_modeSwitchTimer)
				->Field("Yaw", &DragonCrashControllerComponent::m_yaw)
				->Field("Pitch", &DragonCrashControllerComponent::m_pitch)
				->Field("IsBoosting", &DragonCrashControllerComponent::m_isBoosting)
				->Field("IsEnergyExhausted", &DragonCrashControllerComponent::m_isEnergyExhausted)
				->Field("EnergyRemaining", &DragonCrashControllerComponent::m_energyRemaining)
				->Field("EnergyRechargeTimer", &DragonCrashControllerComponent::m_energyRechargeTimer)
				->Field("CrystalsCollected", &DragonCrashControllerComponent::m_crystalsCollected)
				->Field("UiCanvusId", &DragonCrashControllerComponent::m_uiCanvasId)
				->Field("UiStatusId", &DragonCrashControllerComponent::m_uiStatusId)
				->Field("UiInfoId", &DragonCrashControllerComponent::m_uiInfoId)
				;

			AZ::EditContext* edit = serialize->GetEditContext();
			if (edit) {
				edit->Class<DragonCrashControllerComponent>("Dragon Crash Controller", "")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "DragonCrashController")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_pitchTurnLimit, "Pitch Limit", "How far the dragon can turn on the pitch axis in degrees.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightAcceleration, "Flight Acceleration", "How fast the dragon accelerates during flight mode in meters/second^2.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightMaxSpeed, "Flight Speed", "How fast the dragon can fly during flight mode in meters/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightAscendAngle, "Flight Ascend Angle", "How much ascending and descending affects direction during flight mode in degrees.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightYawTurnSpeed, "Flight Yaw Speed", "How fast the dragon turns on the yaw axis during flight mode in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightPitchTurnSpeed, "Flight Pitch Speed", "How fast the dragon turns on the pitch axis during flight mode in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_hoverSpeed, "Hover Speed", "How fast the dragon can fly during hover mode in meters/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_hoverAscendSpeed, "Hover Ascend Speed", "How fast the dragon can ascend and descend during hover mode in meters/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_hoverYawTurnSpeed, "Hover Yaw Speed", "How fast the dragon turns on the yaw axis during hover mode in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_hoverPitchTurnSpeed, "Hover Pitch Speed", "How fast the dragon turns on the pitch axis during hover mode in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_boostAcceleration, "Boost Acceleration", "How fast the dragon accelerates while boosting in meters/second^2.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_boostMaxSpeed, "Boost Max Speed", "How fast the dragon can fly while boosting in meters/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_boostTurnFactor, "Boost Turn Factor", "Multiplier for turn speed while boosting.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_energyMax, "Energy Max", "How long the dragon can boost at max energy in seconds.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_energyRechargeTime, "Boost Recharge Time", "How long the dragon takes to recover its max energy in seconds.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_energyRechargeDelay, "Boost Recharge Delay", "How long the dragon must wait after boosting to recharge energy in seconds.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_healthMax, "Health Max", "How many health points the dragon has.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_respawnTime, "Respawn Time", "How long the dragon must wait to respawn after dying.")
					;
			}
		}
	}

	DragonCrashControllerComponent::DragonCrashControllerComponent()
		: m_healthMax(100.0f)
		, m_respawnTime(4.0f)
		, m_pitchTurnLimit(70.0f)
		, m_flightAcceleration(30.0f)
		, m_flightMaxSpeed(100.0f)
		, m_flightAscendAngle(30.0f)
		, m_flightYawTurnSpeed(90.0f)
		, m_flightPitchTurnSpeed(90.0f)
		, m_hoverSpeed(40.0f)
		, m_hoverAscendSpeed(30.0f)
		, m_hoverYawTurnSpeed(180.0f)
		, m_hoverPitchTurnSpeed(180.0f)
		, m_boostAcceleration(80.0f)
		, m_boostMaxSpeed(300.0f)
		, m_boostTurnFactor(0.6f)
		, m_energyMax(8.0f)
		, m_energyRechargeTime(4.0f)
		, m_energyRechargeDelay(2.0f)
		, m_inputMainYaw(0.0f)
		, m_inputMainPitch(0.0f)
		, m_inputSecondaryYaw(0.0f)
		, m_inputSecondaryPitch(0.0f)
		, m_inputAscend(0.0f)
		, m_inputModeSwitch(false)
		, m_inputBoost(false)
		, m_inputShield(false)
		, m_inputAim(false)
		, m_inputMainFire(false)
		, m_inputSecondaryFire(false)
		, m_currentState(States::dead)
		, m_healthCurrent(0.0f)
		, m_respawnTimer(0.0f)
		, m_modeSwitchTimer(0.0f)
		, m_yaw(0.0f)
		, m_pitch(0.0f)
		, m_isBoosting(false)
		, m_isEnergyExhausted(false)
		, m_energyRemaining(0.0f)
		, m_energyRechargeTimer(0.0f)
		, m_crystalsCollected(0)
	{
	}

	DragonCrashControllerComponent::~DragonCrashControllerComponent()
	{
	}

	void DragonCrashControllerComponent::Spawn()
	{
		// Get respawn transform
		AZ::Transform respawnTransform = AZ::Transform::CreateTranslation(AZ::Vector3(0.0f,0.0f,500.0f));

		// Apply respawn transform
		EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTM, respawnTransform);

		// Set yaw and pitch from angles of respawn transform
		AZ::Quaternion rotationQuat = AZ::Quaternion::CreateFromTransform(respawnTransform);
		AZ::Vector3 angles = QuatToYPR(rotationQuat);
		m_yaw = angles.GetZ();
		m_pitch = angles.GetX();

		// Zero out velocities
		pe_action_set_velocity action;
		action.v = AZVec3ToLYVec3(AZ::Vector3(0.0f, 0.0f, 0.0f));
		action.w = AZVec3ToLYVec3(AZ::Vector3(0.0f, 0.0f, 0.0f));
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, action, false);

		// Reset other state variables
		m_isBoosting = false;
		m_isEnergyExhausted = false;
		m_energyRemaining = m_energyMax;
		m_energyRechargeTimer = 0.0;
		m_healthCurrent = m_healthMax;
		m_crystalsCollected = 0;

		// Start in hover mode
		m_currentState = States::hover;
	}

	void DragonCrashControllerComponent::Kill()
	{
		// Change dead flag and reset respawn timer
		m_currentState = States::dead;
		m_respawnTimer = 0.0;
	}

	void DragonCrashControllerComponent::TickFlight(float deltaTime, std::stringstream &debug)
	{
		EBUS_EVENT_ID(m_uiStatusId, UiTextBus, SetText, "flight mode");

		// Query the Transform and CryPhysics system for state information
		AZ::Transform currentTransform;
		EBUS_EVENT_ID_RESULT(currentTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);
		pe_status_dynamics dynamicStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, dynamicStatus);

		// Extract needed information
		AZ::Quaternion currentRotation = AZ::Quaternion::CreateFromTransform(currentTransform);
		AZ::Vector3 forwardDirection = currentTransform.GetBasisY();
		AZ::Vector3 currentVelocity = LYVec3ToAZVec3(dynamicStatus.v);
		float mass = dynamicStatus.mass;

		debug << "Ascend: " << m_inputAscend << "\n";
		debug << "Velocity: " << (float)currentVelocity.GetLengthExact() << "\n";

		// Boost logic
		m_isBoosting = false;
		if (m_inputBoost && !m_isEnergyExhausted) // Check for boost input and energy isn't exhausted
		{
			if (m_energyRemaining > 0.0f) // Dragon has energy remaining
			{
				m_isBoosting = true;
				m_energyRemaining -= deltaTime;
				m_energyRechargeTimer = 0.0f;
			}
			else // Energy has been exhausted
			{
				m_isEnergyExhausted = true;
			}
		}

		// Get attributes based on boosting state
		float yawSpeed = m_flightYawTurnSpeed;
		float pitchSpeed = m_flightPitchTurnSpeed;
		float acceleration = m_flightAcceleration;
		float maxSpeed = m_flightMaxSpeed;
		if (m_isBoosting)
		{
			yawSpeed *= m_boostTurnFactor;
			pitchSpeed *= m_boostTurnFactor;
			acceleration = m_boostAcceleration;
			maxSpeed = m_boostMaxSpeed;
		}

		// Update rotation state
		m_yaw -= m_inputMainYaw * yawSpeed * deltaTime;
		m_pitch -= m_inputMainPitch * pitchSpeed * deltaTime;

		// Calculate air resistance
		float scalar = maxSpeed*maxSpeed / acceleration;
		AZ::Vector3 airResistance = currentVelocity*currentVelocity * -scalar * deltaTime * mass;

		// Add ascend/descend input to forward to get flight direction
		AZ::Vector3 flightDirection = AZ::Quaternion::CreateRotationX(m_inputAscend * m_flightAscendAngle).GetInverseFast() * forwardDirection;

		// Calculate flight impulse
		AZ::Vector3 flightImpulse = flightDirection * acceleration * deltaTime * mass;

		// Apply impulse physics action
		pe_action_impulse impulseAction;
		impulseAction.impulse = AZVec3ToLYVec3(flightImpulse);
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseAction, false);

		if (m_inputModeSwitch && m_modeSwitchTimer <= 0.0f)
		{
			m_currentState = States::hover;
			m_modeSwitchTimer = 5.0f;
		}
	}

	void DragonCrashControllerComponent::TickHover(float deltaTime, std::stringstream &debug)
	{
		EBUS_EVENT_ID(m_uiStatusId, UiTextBus, SetText, "hover mode");

		// Query the Transform and CryPhysics buses for state information
		AZ::Transform currentTransform;
		EBUS_EVENT_ID_RESULT(currentTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);
		pe_status_dynamics dynamicStatus;
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, GetPhysicsStatus, dynamicStatus);

		// Extract needed information
		AZ::Vector3 rightDirection = currentTransform.GetBasisX();
		AZ::Vector3 forwardDirection = AZ::Quaternion::CreateRotationZ(AZ::DegToRad(90.0f)) * rightDirection;
		AZ::Vector3 currentVelocity = LYVec3ToAZVec3(dynamicStatus.v);
		float mass = dynamicStatus.mass;

		debug << "Ascend: " << m_inputAscend << "\n";
		debug << "Velocity: " << (float)currentVelocity.GetLengthExact() << "\n";

		// Update rotation state
		m_yaw -= m_inputSecondaryYaw * m_hoverYawTurnSpeed * deltaTime;
		m_pitch -= m_inputSecondaryPitch * m_hoverPitchTurnSpeed * deltaTime;

		// Calculate impulse to stop current movement (hover)
		AZ::Vector3 stopImpulse = currentVelocity * -mass;
		
		// Calculate horizontal plane impulse
		AZ::Vector3 horizontalImpulse = rightDirection * m_inputMainYaw + forwardDirection * m_inputMainPitch;
		if (horizontalImpulse.GetLengthExact() > 1.0f)
			horizontalImpulse.NormalizeExact();
		horizontalImpulse *= m_hoverSpeed * mass;

		// Calculate vertical impulse
		AZ::Vector3 verticalImpulse(0.0f, 0.0f, m_inputAscend * m_hoverAscendSpeed * mass);

		// Apply impulse physics action
		pe_action_impulse impulseAction;
		impulseAction.impulse = AZVec3ToLYVec3(stopImpulse + horizontalImpulse + verticalImpulse);
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseAction, false);

		if (m_inputModeSwitch && m_modeSwitchTimer <= 0.0f)
		{
			m_currentState = States::flight;
			m_modeSwitchTimer = 5.0f;
		}
	}

	void DragonCrashControllerComponent::TickMovement(float deltaTime, std::stringstream &debug)
	{
		// Set physics system max speed to 1000 if lower
		// Note: Putting this in Activate insta-crashes the editor
		//TODO: Move this somewhere else where its only called once but doesn't cause crashes
		ICVar *maxVelocity = gEnv->pSystem->GetIConsole()->GetCVar("p_max_velocity");
		if (maxVelocity->GetFVal() < 1000.0f)
			maxVelocity->Set(1000.0f);
		
		// Energy recharge logic
		if (m_energyRechargeTimer >= m_energyRechargeDelay)
		{
			// Dragon has waited long enough to recharge energy
			m_energyRemaining += m_energyMax / m_energyRechargeTime * deltaTime;
			if (m_energyRemaining > m_energyMax)
				m_energyRemaining = m_energyMax;

			// Clear exhaustion tag if energy has recharged
			if (m_isEnergyExhausted && m_energyRemaining >= m_energyMax * 0.8f)
			{
				m_isEnergyExhausted = false;
			}
		}
		else
		{
			// Dragon must wait longer to recharge energy
			m_energyRechargeTimer += deltaTime;
		}

		// Mode switch timer logic
		if (m_modeSwitchTimer > 0.0f)
		{
			m_modeSwitchTimer -= deltaTime;
		}

		debug << "Boosting: " << m_isBoosting << "\n";
		debug << "Energy: " << m_energyRemaining / m_energyMax * 100.0 << " %\n";

		// Handle flight mode logic
		if (m_currentState == States::flight)
		{
			TickFlight(deltaTime, debug);
		}
		// Handle hover mode logic
		else if (m_currentState == States::hover || m_currentState == States::hover_zoom)
		{
			TickHover(deltaTime, debug);
		}

		// Constrain pitch
		if (m_pitch > m_pitchTurnLimit)
			m_pitch = m_pitchTurnLimit;
		if (m_pitch < -m_pitchTurnLimit)
			m_pitch = -m_pitchTurnLimit;

		// Get the current transform
		AZ::Transform currentTransform;
		EBUS_EVENT_ID_RESULT(currentTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

		// Extract current rotation
		AZ::Quaternion currentRotation = AZ::Quaternion::CreateFromTransform(currentTransform);

		// Get difference between current rotation and desired rotation
		AZ::Quaternion desiredRotation = AZ::Quaternion::CreateRotationZ(m_yaw) * AZ::Quaternion::CreateRotationX(m_pitch);
		AZ::Quaternion difference = desiredRotation * currentRotation.GetInverseFast();

		// Calculate angular velocity to reach desired rotation with damping factor
		AZ::Vector3 angularVelocity = QuatToYPR(difference) * 0.8f / deltaTime;

		// Apply set angular velocity physics action
		pe_action_set_velocity velocityAction;
		velocityAction.w = AZVec3ToLYVec3(angularVelocity);
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, velocityAction, false);
	}

	void DragonCrashControllerComponent::TickDragonCrash(float deltaTime, std::stringstream &debug)
	{

	}

	void DragonCrashControllerComponent::TickShield(float deltaTime, std::stringstream &debug)
	{

	}

	void DragonCrashControllerComponent::TickAttack(float deltaTime, std::stringstream &debug)
	{

	}

	void DragonCrashControllerComponent::TickDead(float deltaTime, std::stringstream &debug)
	{
		EBUS_EVENT_ID(m_uiStatusId, UiTextBus, SetText, "is kill");

		// Stop movement
		pe_action_set_velocity action;
		action.v = AZVec3ToLYVec3(AZ::Vector3(0.0f, 0.0f, 0.0f));
		action.w = AZVec3ToLYVec3(AZ::Vector3(0.0f, 0.0f, 0.0f));
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, action, false);

		// Increment and check respawn timer
		m_respawnTimer += deltaTime;
		if (m_respawnTimer >= m_respawnTime)
			Spawn();
	}

	void DragonCrashControllerComponent::CollisionDragon(Collision collision)
	{
		//TODO: Handle collisions with other dragons here
	}

	void DragonCrashControllerComponent::CollisionAttack(Collision collision)
	{
		//TODO: Handle collisions with attacks here
	}

	void DragonCrashControllerComponent::CollisionCrystal(Collision collision)
	{
		bool is_crystal = false;
		EBUS_EVENT_ID_RESULT(is_crystal, collision.m_entity, DragonCrashCollectibles::CrystalRequestBus, isCrystal);
		if (is_crystal)
			EBUS_EVENT_ID(collision.m_entity, DragonCrashCollectibles::CrystalRequestBus, Despawn);
		m_crystalsCollected++;
	}

	void DragonCrashControllerComponent::CollisionOther(Collision collision)
	{
		//TODO: Handle any other collisions here (terrain, trees, etc.)
	}

	void DragonCrashControllerComponent::Init()
	{
	}

	void DragonCrashControllerComponent::Activate()
	{
		// Convert degree measures to radians
		m_flightAscendAngle = AZ::DegToRad(m_flightAscendAngle);
		m_flightYawTurnSpeed = AZ::DegToRad(m_flightYawTurnSpeed);
		m_flightPitchTurnSpeed = AZ::DegToRad(m_flightPitchTurnSpeed);
		m_hoverYawTurnSpeed = AZ::DegToRad(m_hoverYawTurnSpeed);
		m_hoverPitchTurnSpeed = AZ::DegToRad(m_hoverPitchTurnSpeed);
		m_pitchTurnLimit = AZ::DegToRad(m_pitchTurnLimit);

		// Connect to Tick Bus
		AZ::TickBus::Handler::BusConnect();

		// Connect to notification buses to handle input
		AZ::GameplayNotificationId mainYawEventID(GetEntityId(), "main_yaw");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(mainYawEventID);
		AZ::GameplayNotificationId mainPitchEventId(GetEntityId(), "main_pitch");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(mainPitchEventId);
		AZ::GameplayNotificationId secondaryYawEventID(GetEntityId(), "secondary_yaw");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(secondaryYawEventID);
		AZ::GameplayNotificationId secondaryPitchEventId(GetEntityId(), "secondary_pitch");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(secondaryPitchEventId);
		AZ::GameplayNotificationId ascendEventId(GetEntityId(), "ascend");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(ascendEventId);
		AZ::GameplayNotificationId modeEventId(GetEntityId(), "mode_switch");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(modeEventId);
		AZ::GameplayNotificationId boostEventId(GetEntityId(), "boost");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(boostEventId);
		AZ::GameplayNotificationId shieldEventId(GetEntityId(), "shield");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(shieldEventId);
		AZ::GameplayNotificationId aimEventId(GetEntityId(), "aim");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(aimEventId);
		AZ::GameplayNotificationId mainFireId(GetEntityId(), "main_fire");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(mainFireId);
		AZ::GameplayNotificationId secondaryFireId(GetEntityId(), "secondary_fire");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(secondaryFireId);
		
		// Get ui canvas
		AZ::EntityId tempId;
		EBUS_EVENT_ID_RESULT(tempId, GetEntityId(), UiCanvasRefBus, GetCanvas);
		m_uiCanvasId = tempId;
		
		// Get ui elements
		AZ::Entity *uiStatusEntity;
		EBUS_EVENT_ID_RESULT(uiStatusEntity, tempId, UiCanvasBus, FindElementById, 2);
		m_uiStatusId = uiStatusEntity->GetId();
		AZ::Entity *uiInfoEntity;
		EBUS_EVENT_ID_RESULT(uiInfoEntity, tempId, UiCanvasBus, FindElementById, 3);
		m_uiInfoId = uiInfoEntity->GetId();

		// Spawn dragon
		//TODO: Handle spawning of dragons in the game manager
		Spawn();
	}

	void DragonCrashControllerComponent::Deactivate()
	{
		// Disconnect from buses
		AZ::TickBus::Handler::BusDisconnect();
		AZ::GameplayNotificationBus<float>::MultiHandler::BusDisconnect();
	}

	void DragonCrashControllerComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
	{
		// Kill dragon if out of health
		if (m_healthCurrent <= 0.0f && m_currentState != States::dead)
			Kill();

		std::stringstream debug;

		if (m_currentState != States::dead)
		{
			// Living dragon logic
			TickMovement(deltaTime, debug);
			TickDragonCrash(deltaTime, debug);
			TickShield(deltaTime, debug);
			TickAttack(deltaTime, debug);
		}
		else
		{
			// Dead dragon logic
			TickDead(deltaTime, debug);
		}

		// Send debug text to ui
		EBUS_EVENT_ID(m_uiInfoId, UiTextBus, SetText, debug.str().c_str());
	}

	void DragonCrashControllerComponent::OnCollision(const Collision& collision)
	{
		// Get tags of collided with entity
		LmbrCentral::Tags collidedTags;
		EBUS_EVENT_ID_RESULT(collidedTags, collision.m_entity, LmbrCentral::TagComponentRequestBus, GetTags);

		// Check for tags and offload logic to helper functions
		if (collidedTags.find(AZ_CRC("dragon")) != collidedTags.end())
			CollisionDragon(collision);
		if (collidedTags.find(AZ_CRC("attack")) != collidedTags.end())
			CollisionAttack(collision);
		if (collidedTags.find(AZ_CRC("crystal")) != collidedTags.end())
			CollisionCrystal(collision);
		else
			CollisionOther(collision);
	}

	void DragonCrashControllerComponent::OnGameplayEventAction(const float& value)
	{
		AZ::GameplayNotificationId actionBusId = *AZ::GameplayNotificationBus<float>::GetCurrentBusId();

		if (actionBusId.m_actionNameCrc == AZ_CRC("main_yaw"))
			m_inputMainYaw = value;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_pitch"))
			m_inputMainPitch = value;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_yaw"))
			m_inputSecondaryYaw = value;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_pitch"))
			m_inputSecondaryPitch = value;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("ascend"))
			m_inputAscend = value;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("mode_switch"))
			m_inputModeSwitch = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("boost"))
			m_inputBoost = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("shield"))
			m_inputShield = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("aim"))
			m_inputAim = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_fire"))
			m_inputMainFire = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_fire"))
			m_inputSecondaryFire = value > 0;
	}

	void DragonCrashControllerComponent::OnGameplayEventFailed()
	{
		AZ::GameplayNotificationId actionBusId = *AZ::GameplayNotificationBus<float>::GetCurrentBusId();

		if (actionBusId.m_actionNameCrc == AZ_CRC("main_yaw"))
			m_inputMainYaw = 0.0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_pitch"))
			m_inputMainPitch = 0.0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_yaw"))
			m_inputSecondaryYaw = 0.0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_pitch"))
			m_inputSecondaryPitch = 0.0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("ascend"))
			m_inputAscend = 0.0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("mode_switch"))
			m_inputModeSwitch = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("boost"))
			m_inputBoost = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("shield"))
			m_inputShield = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("aim"))
			m_inputAim = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_fire"))
			m_inputMainFire = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("secondary_fire"))
			m_inputSecondaryFire = false;
	}
}