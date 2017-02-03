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
			atan2(2 * (float)(quat.GetW() * quat.GetX() + quat.GetY() * quat.GetZ()), 1 - 2 * (float)(quat.GetX() * quat.GetX() + quat.GetY() * quat.GetY())),
			asin(2 * (float)(quat.GetW() * quat.GetY() - quat.GetZ() * quat.GetX())),
			atan2(2 * (float)(quat.GetW() * quat.GetZ() + quat.GetX() * quat.GetY()), 1 - 2 * (float)(quat.GetY() * quat.GetY() + quat.GetZ() * quat.GetZ()))
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
		: m_flightSpeed(100.0)
		, m_yawTurnSpeed(180.0)
		, m_pitchTurnSpeed(180.0)
		, m_pitchTurnLimit(70.0)
		, m_boostAcceleration(50.0)
		, m_boostMaxSpeed(300.0)
		, m_energyMax(8.0)
		, m_energyRechargeTime(4.0)
		, m_energyRechargeDelay(2.0)
		, m_healthMax(100.0)
		, m_respawnTime(4.0)
		, m_inputMainYaw(0.0)
		, m_inputMainPitch(0.0)
		, m_inputSecondaryYaw(0.0)
		, m_inputSecondaryPitch(0.0)
		, m_inputAscend(0.0)
		, m_inputBoost(false)
		, m_inputShield(false)
		, m_inputAim(false)
		, m_inputMainFire(false)
		, m_yaw(0.0)
		, m_pitch(0.0)
		, m_isBoosting(false)
		, m_isEnergyExhausted(false)
		, m_energyRemaining(0.0)
		, m_energyRechargeTimer(0.0)
		, m_isDead(false)
		, m_healthCurrent(0.0)
		, m_respawnTimer(0.0)
	{
	}

	DragonCrashControllerComponent::~DragonCrashControllerComponent()
	{
	}

	void DragonCrashControllerComponent::OnSpawn()
	{
		AZ::Transform respawnTransform = AZ::Transform::CreateTranslation(AZ::Vector3(0,0,500));

		EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTM, respawnTransform);

		AZ::Quaternion rotationQuat = AZ::Quaternion::CreateFromTransform(respawnTransform);
		AZ::Vector3 angles = QuatToYPR(rotationQuat);
		m_yaw = angles.GetZ();
		m_pitch = angles.GetY();

		m_isBoosting = false;
		m_isEnergyExhausted = false;
		m_energyRemaining = m_energyMax;
		m_energyRechargeTimer = 0.0;
		m_healthCurrent = m_healthMax;

		m_isDead = false;
	}

	void DragonCrashControllerComponent::OnDeath()
	{
		m_isDead = true;
		m_respawnTimer = 0.0;
	}

	void DragonCrashControllerComponent::HandleMovementTick(float deltaTime)
	{
		// Update rotation state
		m_yaw -= m_inputMainYaw * m_yawTurnSpeed * deltaTime;
		m_pitch -= m_inputMainPitch * m_pitchTurnSpeed * deltaTime;
		float roll = 0.0;

		// Constrain pitch
		if (m_pitch > m_pitchTurnLimit)
			m_pitch = m_pitchTurnLimit;
		else if (m_pitch < -m_pitchTurnLimit)
			m_pitch = -m_pitchTurnLimit;

		// Get current transform using transform bus
		AZ::Transform transform;
		EBUS_EVENT_ID_RESULT(transform, GetEntityId(), AZ::TransformBus, GetWorldTM);

		AZ::Quaternion currentRotation = AZ::Quaternion::CreateFromTransform(transform);
		AZ::Quaternion desiredRotation =
			AZ::Quaternion::CreateRotationZ(m_yaw) *
			AZ::Quaternion::CreateRotationX(m_pitch) *
			AZ::Quaternion::CreateRotationZ(roll);
		AZ::Quaternion difference = desiredRotation * currentRotation.GetInverseFast();

		AZ::Vector3 angularVelocity = QuatToYPR(difference) * 0.8 / deltaTime;

		AZ::Vector3 forwardVelocity = transform.GetBasisY();
		forwardVelocity *= m_flightSpeed;

		pe_action_set_velocity action;
		action.v = AZVec3ToLYVec3(forwardVelocity);
		action.w = AZVec3ToLYVec3(angularVelocity);

		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, action, false);
	}

	void DragonCrashControllerComponent::HandleDragonCrashTick(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::HandleShieldTick(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::HandleAttackTick(float deltaTime)
	{

	}

	void DragonCrashControllerComponent::HandleDeadTick(float deltaTime)
	{
		pe_action_set_velocity action;
		action.v = AZVec3ToLYVec3(AZ::Vector3(0,0,0));
		action.w = AZVec3ToLYVec3(AZ::Vector3(0,0,0));
		EBUS_EVENT_ID(GetEntityId(), LmbrCentral::CryPhysicsComponentRequestBus, ApplyPhysicsAction, action, false);

		m_respawnTimer += deltaTime;
		if (m_respawnTimer >= m_respawnTime) 
			OnSpawn();
	}

	void DragonCrashControllerComponent::HandleDragonCollision(Collision collision)
	{

	}

	void DragonCrashControllerComponent::HandleAttackCollision(Collision collision)
	{

	}

	void DragonCrashControllerComponent::HandleOtherCollision(Collision collision)
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

		OnSpawn();
	}

	void DragonCrashControllerComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		AZ::GameplayNotificationBus<float>::MultiHandler::BusDisconnect();
	}

	void DragonCrashControllerComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
	{
		if (!m_isDead && m_healthCurrent <= 0.0)
			OnDeath();

		if (!m_isDead)
		{
			HandleMovementTick(deltaTime);
			HandleDragonCrashTick(deltaTime);
			HandleShieldTick(deltaTime);
			HandleAttackTick(deltaTime);
		}
		else
			HandleDeadTick(deltaTime);
	}

	void DragonCrashControllerComponent::OnCollision(const Collision& collision)
	{
		LmbrCentral::Tags collidedTags;
		EBUS_EVENT_ID_RESULT(collidedTags, GetEntityId(), LmbrCentral::TagComponentRequestBus, GetTags);

		if (collidedTags.find(AZ_CRC("dragon")) != collidedTags.end())
			HandleDragonCollision(collision);
		if (collidedTags.find(AZ_CRC("attack")) != collidedTags.end())
			HandleAttackCollision(collision);
		else
			HandleOtherCollision(collision);
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