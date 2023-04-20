#include "util.cpp"

namespace InteractiveIdles
{
    class AnimPlayer
    {
        public:

            static AnimPlayer *GetSingleton()
            {
                static AnimPlayer singleton;
                return &singleton;
            } 
            void GetIdleRecords()
            {
                std::string pluginName = "Interactive Idles.esp";

                // IdleHandsBehindBack =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5902);
                // IdleSearchChest =  RE::TESForm::LookupByID<RE::TESIdleForm>(0x5900);
                // IdleSearchKneel = RE::TESForm::LookupByID<RE::TESIdleForm>(0x5903);

                IdleStop = FormUtil::Form::GetFormFromMod(pluginName, 0x5901)->As<RE::TESIdleForm>();
                IdleHandsBehindBack= FormUtil::Form::GetFormFromMod(pluginName, 0x5902)->As<RE::TESIdleForm>();
                IdleSearchKneel = FormUtil::Form::GetFormFromMod(pluginName, 0x5903)->As<RE::TESIdleForm>();
                IdleSearchChest = FormUtil::Form::GetFormFromMod(pluginName, 0x5900)->As<RE::TESIdleForm>();


                IdleUseDoor = FormUtil::Form::GetFormFromMod(pluginName, 0xFB04)->As<RE::TESIdleForm>();
                IdlePickUp = FormUtil::Form::GetFormFromMod(pluginName, 0xFB05)->As<RE::TESIdleForm>();
                IdlePickUpLow = FormUtil::Form::GetFormFromMod(pluginName, 0xFB06)->As<RE::TESIdleForm>();
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
        


        void ExitAnimation(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleStop, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlaySearchChest(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleSearchChest, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlaySearchKneel(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleSearchKneel, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlayHandsBehindBack(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleHandsBehindBack, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlayUseDoor(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleUseDoor, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlayPickUp(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdlePickUp, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlayPickUpLow(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdlePickUpLow, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }
        void PlayLockPick(RE::Actor* actor)
        {
            AnimUtil::Idle::Play(IdleLockPick, actor, RE::DEFAULT_OBJECT::kActionIdle, nullptr);
        }


        

        private:



            RE::TESIdleForm *IdleLockPick;
            RE::TESIdleForm *IdlePickUp;
            RE::TESIdleForm *IdlePickUpLow;
            RE::TESIdleForm *IdleUseDoor;

            RE::TESIdleForm *IdleSearchChest;
            RE::TESIdleForm *IdleSearchKneel;
            RE::TESIdleForm *IdleHandsBehindBack;
            RE::TESIdleForm *IdleStop;
            
    };
}