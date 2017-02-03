#pragma once

#include <AzCore\Component\Component.h>
#include <AzCore\Component\TickBus.h>
#include <GameplayEventBus.h>
#include <LmbrCentral/Physics/PhysicsComponentBus.h>

namespace DragonCrashController
{
	class DragonCrashControllerComponent
		: public AZ::Component
		, public AZ::TickBus::Handler
		, public AZ::GameplayNotificationBus<float>::MultiHandler
	{
	public:
		AZ_COMPONENT(DragonCrashControllerComponent, "{13263517-6125-4A1E-B316-6CF810D340CE}");
		static void Reflect(AZ::ReflectContext* context);
		
		DragonCrashControllerComponent();
		~DragonCrashControllerComponent();

		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// TickBus
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		// GameplayNotificationBus
		void OnGameplayEventAction(const float& value) override;
		void OnGameplayEventFailed() override;

	private:
		void OnSpawn();
		void OnDeath();

		void HandleMovementTick(float deltaTime);
		void HandleDragonCrashTick(float deltaTime);
		void HandleShieldingTick(float deltaTime);
		void HandleFiringTick(float deltaTime);
		void HandleDeadTick(float deltaTime);

		void HandleDragonCollision(LmbrCentral::PhysicsComponentNotifications::Collision collision);
		void HandleAttackCollision(LmbrCentral::PhysicsComponentNotifications::Collision collision);
		void HandleOtherCollision(LmbrCentral::PhysicsComponentNotifications::Collision collision);

		// Editor-exposed settings
		float m_flightSpeed;
		float m_yawTurnSpeed;
		float m_pitchTurnSpeed;
		float m_pitchTurnLimit;
		float m_boostAcceleration;
		float m_boostMaxSpeed;
		float m_energyMax;
		float m_energyRechargeTime;
		float m_energyRechargeDelay;
		float m_healthMax;
		float m_respawnTime;

		// Inputs
		float m_inputMainYaw;
		float m_inputMainPitch;
		float m_inputSecondaryYaw;
		float m_inputSecondaryPitch;
		float m_inputAscend;
		bool m_inputBoost;
		bool m_inputShield;
		bool m_inputAim;
		bool m_inputMainFire;

		// State values
		float m_yaw;
		float m_pitch;
		bool m_isBoosting;
		bool m_isEnergyExhausted;
		float m_energyRemaining;
		float m_energyRechargeTimer;
		bool m_isDead;
		float m_healthCurrent;
		float m_respawnTimer;
	};
}