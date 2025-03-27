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
                // State = true;
            }

            static void TryForceThirdPerson()
            {
                if (!Settings::GetSingleton()->GetForceThirdPerson()) return; 
                auto playerControls = PlayerControls::GetSingleton(); 
                auto* camera = PlayerCamera::GetSingleton(); 
                if (!camera) 
                { 
                    return; 
                }
                playerControls->data.povScriptMode = true; 
                camera->ForceThirdPerson(); 

                playerControls->data.povScriptMode = false; 
            }
            static bool BehaviorPatchInstalled()
            {
                bool bDummy;
                RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		        RE::PlayerCharacter::GetSingleton()->GetAnimationGraphManager(animationGraphManagerPtr);
                
                RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr ? animationGraphManagerPtr->graphs[0].get() : nullptr;

                if (!animationGraph) return false;

                return animationGraph->GetGraphVariableBool("II_PatchInstalled", bDummy);
            }
            static void GetIdleRecords()
            {
                constexpr auto path = L"Data/SKSE/Plugins/Animated_Interactions/Forms.ini";

                CSimpleIniA ini;
                ini.SetUnicode();

                ini.LoadFile(path);
                auto forms_section = ini.GetSection("Idles");
                WriteLocker locker(animLock);
                for (auto &[key, entry] : *forms_section)
                {
                    AddIdleFormFromString(key.pItem, entry);
                }
                // std::string pluginName = "Animated Interactions.esp";

                // IdleHandsBehindBack =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5902);
                // IdleSearchChest =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5900);
                // IdleSearchKneel = RE::TESForm::LookupByID<RE::TESIdleForm>(0x5903);
                
                
                // idleMap.emplace("Exit", GetIdleForm(0x5901) );
                // idleMap.emplace("SearchKneel",GetIdleForm(0x5903));
                // idleMap.emplace("SearchChest",GetIdleForm(0x5900) );
                // idleMap.emplace("PickUp", GetIdleForm(0xFB05));
                // idleMap.emplace("PickUpLow", GetIdleForm(0xFB06));

                // idleMap["UseDoor"] = GetIdleForm(0x1EE0C);
                // idleMap["Take"] = GetIdleForm(0x1EE09);

                // idleMap["TakeHigh"] = GetIdleForm(0x1EE0B);
                // idleMap["TakeLow"] = GetIdleForm(0x1EE0A);

                if (BehaviorPatchInstalled())
                {
                    SKSE::log::info("Behavior Patch Detected.");
                }

            }
        

        static bool PlayAnimation(RE::Actor* actor, std::string idleName)
        {   
            ReadLocker locker(animLock);
            // RE::PlayerCamera::GetSingleton()->ForceThirdPerson();
            TryForceThirdPerson();
            actor->SetGraphVariableFloat("II_AnimationSpeed", static_cast<float>(Settings::GetSingleton()->GetAnimationSpeed()));
            bool result = AnimUtil::Idle::Play(idleMap[idleName], actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
            if (!result)
            {
                SKSE::log::warn("PlayAnimation failed at: {} for actor: {}", idleName, actor->GetActorBase()->GetName());
            }
            return result;
        }

        static bool PlayAnimation(std::string idleName)
        {
            return PlayAnimation(PlayerCharacter::GetSingleton(), idleName);
        }
        
        static void ExitAnimation(RE::Actor* actor)
        {
            PlayAnimation(actor, "Exit");
        }
        

        static void ApplyAnimationSpeed(RE::Actor *actor)
        {
            actor->SetGraphVariableFloat("II_AnimationSpeed", Settings::GetSingleton()->GetAnimationSpeed());
        }
        
        static void AddIdleFormFromString(std::string name, std::string raw)
        {
            auto *idle = FormUtil::Parse::GetFormFromConfigString(raw)->As<RE::TESIdleForm>();
            idleMap.emplace(name, idle);
        }

        private:

            using Lock = std::shared_mutex;
            using ReadLocker = std::shared_lock<Lock>;
            using WriteLocker = std::unique_lock<Lock>;
            std::condition_variable_any cv;

            

            static inline Lock animLock;
            static inline std::unordered_map<std::string, RE::TESIdleForm*> idleMap;

            
            
            static RE::TESIdleForm* GetIdleForm(std::uint32_t formid)
            {
                return FormUtil::Parse::GetFormFromMod(formid,"Animated Interactions.esp")->As<RE::TESIdleForm>();
            }
            
    };
}