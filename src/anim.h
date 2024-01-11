#pragma once
#include "util.h"
#include "settings.cpp"
#include <shared_mutex>

namespace AnimatedInteractions
{
    class AnimPlayer
    {
        public:
            
            AnimPlayer()
            {
                State = true;
            }
            static AnimPlayer *GetSingleton()
            {
                static AnimPlayer singleton;
                return &singleton;
            }

            static void TryForceThirdPerson()
            {
                if (!Settings::GetSingleton()->GetForceThirdPerson()) return; 
                auto playerControls = PlayerControls::GetSingleton(); 
                playerControls->data.povScriptMode = true; 
                using func_t = decltype(&TryForceThirdPerson);
                REL::Relocation<func_t> func{REL::RelocationID(54948, 55565)};
                func();

                playerControls->data.povScriptMode = false; 
            }
            bool BehaviorPatchInstalled()
            {
                bool bDummy;
                RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		        RE::PlayerCharacter::GetSingleton()->GetAnimationGraphManager(animationGraphManagerPtr);
                
                RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr ? animationGraphManagerPtr->graphs[0].get() : nullptr;

                if (!animationGraph) return false;

                return animationGraph->GetGraphVariableBool("II_PatchInstalled", bDummy);
            }
            void GetIdleRecords()
            {
                
                std::string pluginName = "Animated Interactions.esp";

                // IdleHandsBehindBack =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5902);
                // IdleSearchChest =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5900);
                // IdleSearchKneel = RE::TESForm::LookupByID<RE::TESIdleForm>(0x5903);
                WriteLocker locker(animLock);
                
                idleMap.emplace("Exit", GetIdleForm(0x5901) );
                idleMap.emplace("SearchKneel",GetIdleForm(0x5903));
                idleMap.emplace("SearchChest",GetIdleForm(0x5900) );
                // idleMap.emplace("PickUp", GetIdleForm(0xFB05));
                // idleMap.emplace("PickUpLow", GetIdleForm(0xFB06));

                idleMap["UseDoor"] = GetIdleForm(0x1EE0C);
                idleMap["Take"] = GetIdleForm(0x1EE09);

                idleMap["TakeHigh"] = GetIdleForm(0x1EE0B);
                idleMap["TakeLow"] = GetIdleForm(0x1EE0A);

                if (BehaviorPatchInstalled())
                {
                    SKSE::log::info("Behavior Patch Detected.");
                }

            }
        

        void PlayAnimation(RE::Actor* actor, std::string idleName)
        {   
            ReadLocker locker(animLock);
            if (!State) return;
            // RE::PlayerCamera::GetSingleton()->ForceThirdPerson();
            TryForceThirdPerson();
            actor->SetGraphVariableFloat("II_AnimationSpeed", static_cast<float>(Settings::GetSingleton()->GetAnimationSpeed()));
            if (!AnimUtil::Idle::Play(idleMap[idleName], actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr))
            {
                SKSE::log::warn("PlayAnimation failed at: {} for actor: {}", idleName, actor->GetActorBase()->GetName());
            }
        }


        void PlayAnimation(std::string idleName)
        {
            PlayAnimation(PlayerCharacter::GetSingleton(), idleName);
        }

        void PlayAnimationAndWait(RE::Actor* actor, std::string idleName)
        {
            PlayAnimation(idleName);
            WriteLocker locker(animLock);
            cv.wait_for(locker, std::chrono::milliseconds(250));
            
        }

        void PlayAnimationAndWait(std::string idleName)
        {
            PlayAnimationAndWait(PlayerCharacter::GetSingleton(), idleName);
        }

        
        void ExitAnimation(RE::Actor* actor)
        {
            PlayAnimation(actor, "Exit");
        }
        

        void ApplyAnimationSpeed(RE::Actor *actor)
        {
            actor->SetGraphVariableFloat("II_AnimationSpeed", Settings::GetSingleton()->GetAnimationSpeed());
        }

        void SetState(bool newState)
        {
            WriteLocker locker(animLock);
            State = newState;
        }

        bool GetState()
        {
            ReadLocker locker(animLock);
            return State;
        }

        private:

            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;
            std::condition_variable_any cv;

            static inline bool State;
            

            static inline Lock animLock;
            static inline std::unordered_map<std::string, RE::TESIdleForm*> idleMap;

            
            
            static RE::TESIdleForm* GetIdleForm(std::uint32_t formid)
            {
                return FormUtil::Form::GetFormFromMod("Animated Interactions.esp", formid)->As<RE::TESIdleForm>();
            }
            
    };
}