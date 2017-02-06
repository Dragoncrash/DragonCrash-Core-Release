#include "StdAfx.h"

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
				->Field("FlightSpeed", &DragonCrashControllerComponent::m_flightSpeed)
				->Field("YawTurnSpeed", &DragonCrashControllerComponent::m_yawTurnSpeed)
				->Field("PitchTurnSpeed", &DragonCrashControllerComponent::m_pitchTurnSpeed)
				->Field("PitchTurnLimit", &DragonCrashControllerComponent::m_pitchTurnLimit)
				->Field("BoostAcceleration", &DragonCrashControllerComponent::m_boostAcceleration)
				->Field("BoostMaxSpeed", &DragonCrashControllerComponent::m_boostMaxSpeed)
				->Field("EnergyMax", &DragonCrashControllerComponent::m_energyMax)
				->Field("EnergyRechargeTime", &DragonCrashControllerComponent::m_energyRechargeTime)
				->Field("EnergyRechargeDelay", &DragonCrashControllerComponent::m_energyRechargeDelay)
				->Field("HealthMax", &DragonCrashControllerComponent::m_healthMax)
				->Field("RespawnTime", &DragonCrashControllerComponent::m_respawnTime)
				->Field("InputMainYaw", &DragonCrashControllerComponent::m_inputMainYaw)
				->Field("InputMainPitch", &DragonCrashControllerComponent::m_inputMainPitch)
				->Field("InputSecondaryYaw", &DragonCrashControllerComponent::m_inputSecondaryYaw)
				->Field("InputSecondaryPitch", &DragonCrashControllerComponent::m_inputSecondaryPitch)
				->Field("InputAscend", &DragonCrashControllerComponent::m_inputAscend)
				->Field("InputBoost", &DragonCrashControllerComponent::m_inputBoost)
				->Field("InputShield", &DragonCrashControllerComponent::m_inputShield)
				->Field("InputAim", &DragonCrashControllerComponent::m_inputAim)
				->Field("InputMainFire", &DragonCrashControllerComponent::m_inputMainFire)
				->Field("Yaw", &DragonCrashControllerComponent::m_yaw)
				->Field("Pitch", &DragonCrashControllerComponent::m_pitch)
				->Field("IsBoosting", &DragonCrashControllerComponent::m_isBoosting)
				->Field("IsEnergyExhausted", &DragonCrashControllerComponent::m_isEnergyExhausted)
				->Field("EnergyRemaining", &DragonCrashControllerComponent::m_energyRemaining)
				->Field("EnergyRechargeTimer", &DragonCrashControllerComponent::m_energyRechargeTimer)
				->Field("IsDead", &DragonCrashControllerComponent::m_isDead)
				->Field("HealthCurrent", &DragonCrashControllerComponent::m_healthCurrent)
				->Field("RespawnTimer", &DragonCrashControllerComponent::m_respawnTimer)
				;

			AZ::EditContext* edit = serialize->GetEditContext();
			if (edit) {
				edit->Class<DragonCrashControllerComponent>("Dragon Crash Controller", "")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "DragonCrashController")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_flightSpeed, "Flight Speed", "How fast the dragon flies in meters/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_yawTurnSpeed, "Yaw Speed", "How fast the dragon turns on the yaw axis in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_pitchTurnSpeed, "Pitch Speed", "How fast the dragon turns on the pitch axis in degrees/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_pitchTurnLimit, "Pitch Limit", "How far the dragon can turn on the pitch axis in degrees.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_boostAcceleration, "Boost Acceleration", "How fast the dragon accelerates while boosting in meters^2/second.")
					->DataElement(AZ::Edit::UIHandlers::Default, &DragonCrashControllerComponent::m_boostMaxSpeed, "Boost Max Speed", "How fast the dragon can fly while boosting in meters/second.")
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
		: m_flightSpeed(100.0f)
		, m_yawTurnSpeed(180.0f)
		, m_pitchTurnSpeed(180.0f)
		, m_pitchTurnLimit(70.0f)
		, m_boostAcceleration(50.0f)
		, m_boostMaxSpeed(300.0f)
		, m_energyMax(8.0f)
		, m_energyRechargeTime(4.0f)
		, m_energyRechargeDelay(2.0f)
		, m_healthMax(100.0f)
		, m_respawnTime(4.0f)
		, m_inputMainYaw(0.0f)
		, m_inputMainPitch(0.0f)
		, m_inputSecondaryYaw(0.0f)
		, m_inputSecondaryPitch(0.0f)
		, m_inputAscend(0.0f)
		, m_inputBoost(false)
		, m_inputShield(false)
		, m_inputAim(false)
		, m_inputMainFire(false)
		, m_yaw(0.0f)
		, m_pitch(0.0f)
		, m_isBoosting(false)
		, m_isEnergyExhausted(false)
		, m_energyRemaining(0.0f)
		, m_energyRechargeTimer(0.0f)
		, m_isDead(true)
		, m_healthCurrent(0.0f)
		, m_respawnTimer(0.0f)
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

		// Reset other state variables
		m_isBoosting = false;
		m_isEnergyExhausted = false;
		m_energyRemaining = m_energyMax;
		m_energyRechargeTimer = 0.0;
		m_healthCurrent = m_healthMax;

		// Change dead flag
		m_isDead = false;
	}

	void DragonCrashControllerComponent::Kill()
	{
		// Change dead flag and reset respawn timer
		m_isDead = true;
		m_respawnTimer = 0.0;
	}

	void DragonCrashControllerComponent::TickMovement(float deltaTime)
	{
		// Update rotation state
		m_yaw -= m_inputMainYaw * m_yawTurnSpeed * deltaTime;
		m_pitch -= m_inputMainPitch * m_pitchTurnSpeed * deltaTime;
		float roll = 0.0f;

		// Constrain pitch
		if (m_pitch > m_pitchTurnLimit)
			m_pitch = m_pitchTurnLimit;
		else if (m_pitch < -m_pitchTurnLimit)
			m_pitch = -m_pitchTurnLimit;

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

		// Print information
		if (mass > 0.0f)
		{
			CryLog("Velocity: %f", (float)currentVelocity.GetLengthExact());
		}

		// Calculate impulse in forward direction
		AZ::Vector3 impulse = currentVelocity * mass + forwardDirection * m_flightSpeed * mass;

		// Print information
		if (mass > 0.0f)
		{
			CryLog("Velocity: %f", (float)currentVelocity.GetLengthExact());
			CryLog("Impulse: %f", (float)impulse.GetLengthExact());
		}

		// Get difference between current rotation and desired rotation
		AZ::Quaternion desiredRotation =
			AZ::Quaternion::CreateRotationZ(m_yaw) *
			AZ::Quaternion::CreateRotationX(m_pitch) *
			AZ::Quaternion::CreateRotationZ(roll);
		AZ::Quaternion difference = desiredRotation * currentRotation.GetInverseFast();

		// Calculate angular velocity to reach desired rotation
		AZ::Vector3 angularVelocity = QuatToYPR(difference) * 0.8f / deltaTime;

		// Apply angular velocity physics action
		pe_action_set_velocity velocityAction;
		velocityAction.w = AZVec3ToLYVec3(angularVelocity);
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, velocityAction, false);

		// Apply impulse physics action
		pe_action_impulse impulseAction;
		impulseAction.impulse = AZVec3ToLYVec3(impulse);
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, impulseAction, false);
	}

	void DragonCrashControllerComponent::TickDragonCrash(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::TickShield(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::TickAttack(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::TickDead(float deltaTime)
	{
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

	}

	void DragonCrashControllerComponent::CollisionAttack(Collision collision)
	{

	}

	void DragonCrashControllerComponent::CollisionOther(Collision collision)
	{

	}

	void DragonCrashControllerComponent::Init()
	{
	}

	void DragonCrashControllerComponent::Activate()
	{
		// Convert degree measures to radians
		m_yawTurnSpeed = AZ::DegToRad(m_yawTurnSpeed);
		m_pitchTurnSpeed = AZ::DegToRad(m_pitchTurnSpeed);
		m_pitchTurnLimit = AZ::DegToRad(m_pitchTurnLimit);

		// Set max speed to 1000
		gEnv->pSystem->GetIConsole()->GetCVar("p_max_velocity")->Set(1000.0f);

		AZ::TickBus::Handler::BusConnect();

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

		AZ::GameplayNotificationId boostEventId(GetEntityId(), "boost");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(boostEventId);

		AZ::GameplayNotificationId shieldEventId(GetEntityId(), "shield");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(shieldEventId);

		AZ::GameplayNotificationId aimEventId(GetEntityId(), "aim");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(aimEventId);

		AZ::GameplayNotificationId mainFireId(GetEntityId(), "main_fire");
		AZ::GameplayNotificationBus<float>::MultiHandler::BusConnect(mainFireId);

		Spawn();
	}

	void DragonCrashControllerComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		AZ::GameplayNotificationBus<float>::MultiHandler::BusDisconnect();
	}

	void DragonCrashControllerComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
	{
		if (!m_isDead && m_healthCurrent <= 0.0f)
		{
			Kill();
		}

		if (!m_isDead)
		{
			TickMovement(deltaTime);
			TickDragonCrash(deltaTime);
			TickShield(deltaTime);
			TickAttack(deltaTime);
		}
		else
		{
			TickDead(deltaTime);
		}
	}

	void DragonCrashControllerComponent::OnCollision(const Collision& collision)
	{
		LmbrCentral::Tags collidedTags;
		EBUS_EVENT_ID_RESULT(collidedTags, GetEntityId(), LmbrCentral::TagComponentRequestBus, GetTags);

		if (collidedTags.find(AZ_CRC("dragon")) != collidedTags.end())
			CollisionDragon(collision);
		if (collidedTags.find(AZ_CRC("attack")) != collidedTags.end())
			CollisionAttack(collision);
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
		else if (actionBusId.m_actionNameCrc == AZ_CRC("boost"))
			m_inputBoost = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("shield"))
			m_inputShield = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("aim"))
			m_inputAim = value > 0;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_fire"))
			m_inputMainFire = value > 0;
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
		else if (actionBusId.m_actionNameCrc == AZ_CRC("boost"))
			m_inputBoost = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("shield"))
			m_inputShield = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("aim"))
			m_inputAim = false;
		else if (actionBusId.m_actionNameCrc == AZ_CRC("main_fire"))
			m_inputMainFire = false;
	}
}