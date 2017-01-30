
#pragma once

#include <IGameRulesSystem.h>

namespace LYGame
{
    class DragonCrashGameRules
        : public CGameObjectExtensionHelper < DragonCrashGameRules, IGameRules >
    {
    public:
        DragonCrashGameRules() {}
        virtual ~DragonCrashGameRules();

        //////////////////////////////////////////////////////////////////////////
        //! IGameObjectExtension
        bool Init(IGameObject* pGameObject) override;
        void PostInit(IGameObject* pGameObject) override;
        void ProcessEvent(SEntityEvent& event) override { }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // IGameRules
        bool OnClientConnect(ChannelId channelId, bool isReset) override;
        //////////////////////////////////////////////////////////////////////////
    };
} // namespace LYGame