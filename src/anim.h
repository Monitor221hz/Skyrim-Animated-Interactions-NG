#pragma once
#include "util.cpp"
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
                idleMap.emplace("UseDoor", GetIdleForm(0xFB04) );
                // idleMap.emplace("PickUp", GetIdleForm(0xFB05));
                // idleMap.emplace("PickUpLow", GetIdleForm(0xFB06));
                idleMap.emplace("LockPick", GetIdleForm(0xFB07));

                idleMap.emplace("Take", GetIdleForm(0xFB05));
                idleMap.emplace("TakeHigh", idleMap["Take"]);
                idleMap.emplace("TakeLow", GetIdleForm(0xFB06));




                if (BehaviorPatchInstalled())
                {

                    idleMap["UseDoor"] = GetIdleForm(0x1EE0C);
                    idleMap["Take"] = GetIdleForm(0x1EE09);
                    idleMap["TakeHigh"] = GetIdleForm(0x1EE0B);
                    idleMap["TakeLow"] = GetIdleForm(0x1EE0A);

                    SKSE::log::info("Behavior Patch Detected.");
                }

                IdleStop = FormUtil::Form::GetFormFromMod(pluginName, 0x5901)->As<RE::TESIdleForm>();
                IdleHandsBehindBack= FormUtil::Form::GetFormFromMod(pluginName, 0x5902)->As<RE::TESIdleForm>();
                IdleSearchKneel = FormUtil::Form::GetFormFromMod(pluginName, 0x5903)->As<RE::TESIdleForm>();
                IdleSearchChest = FormUtil::Form::GetFormFromMod(pluginName, 0x5900)->As<RE::TESIdleForm>();

                IdleUseDoor = FormUtil::Form::GetFormFromMod(pluginName, 0xFB04)->As<RE::TESIdleForm>();

                IdlePickUp = FormUtil::Form::GetFormFromMod(pluginName, 0xFB05)->As<RE::TESIdleForm>();
                IdlePickUpLow = FormUtil::Form::GetFormFromMod(pluginName, 0xFB06)->As<RE::TESIdleForm>();
                IdleTake = FormUtil::Form::GetFormFromMod(pluginName, 0x19D08)->As<RE::TESIdleForm>();
                IdleLockPick = FormUtil::Form::GetFormFromMod(pluginName, 0xFB07)->As<RE::TESIdleForm>();
                //0xb240a
                
                if (IdleStop)
                {
                    SKSE::log::info("Load succeeded");
                }
                else
                {
                    SKSE::log::warn("Load failed");
                }
            }
        

        void PlayAnimation(RE::Actor* actor, std::string idleName)
        {   
            ReadLocker locker(animLock);
            if (!State) return;
            RE::PlayerCamera::GetSingleton()->ForceThirdPerson();
            actor->SetGraphVariableFloat("II_AnimationSpeed", Settings::GetSingleton()->GetAnimationSpeed());
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


            RE::TESIdleForm *IdleLockPick;
            RE::TESIdleForm *IdleTake;
            RE::TESIdleForm *IdlePickUp;
            RE::TESIdleForm *IdlePickUpLow;
            RE::TESIdleForm *IdleUseDoor;

            RE::TESIdleForm *IdleSearchChest;
            RE::TESIdleForm *IdleSearchKneel;
            RE::TESIdleForm *IdleHandsBehindBack;
            RE::TESIdleForm *IdleStop;
            
    };
}