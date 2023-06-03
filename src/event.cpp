#include "event.h"

#include "anim.h"
namespace AnimatedInteractions
{
    RE::BSEventNotifyControl EventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* eventSource)
    {

        AnimPlayer* animplyr = AnimPlayer::GetSingleton();
        auto* plyr = RE::PlayerCharacter::GetSingleton();
        bool idlePlaying;
        
        plyr->GetGraphVariableBool("bForceIdleStop", idlePlaying);
        
        
        if (event->opening && !idlePlaying) animplyr->SetState(false);
        if (!event->opening) animplyr->SetState(true);

        if (!event->opening && idlePlaying && (event->menuName == RE::ContainerMenu::MENU_NAME || event->menuName == RE::LockpickingMenu::MENU_NAME)) AnimPlayer::GetSingleton()->ExitAnimation(plyr);
        // if (event->menuName == RE::ContainerMenu::MENU_NAME)
        // {
            
        //     auto* menu = RE::UI::GetSingleton()->GetMenu<RE::ContainerMenu>(event->menuName).get();
        //     if (menu->GetContainerMode()==RE::ContainerMenu::ContainerMode::kLoot || menu->GetContainerMode()==RE::ContainerMenu::ContainerMode::kSteal)
        //     {
        //         RE::TESObjectREFRPtr refr;
        //         RE::LookupReferenceByHandle(menu->GetTargetRefHandle(), refr);
                
        //         if (!event->opening && idlePlaying) AnimPlayer::GetSingleton()->ExitAnimation(plyr);
        //     }
            
        // }
        // else if(event->menuName == RE::LockpickingMenu::MENU_NAME)
        // {   
        //     if (!event->opening && idlePlaying) AnimPlayer::GetSingleton()->ExitAnimation(plyr);
        // }
        return RE::BSEventNotifyControl::kContinue;
    }
    RE::BSEventNotifyControl EventHandler::ProcessEvent(const RE::TESOpenCloseEvent* a_event, RE::BSTEventSource<RE::TESOpenCloseEvent>* a_eventSource)
    {
        // if (a_event->activeRef)SKSE::log::info("activeRef {}", a_event->activeRef.get()->GetBaseObject()->GetFormType());
        // if (a_event->ref)SKSE::log::info("Ref {}", a_event->ref.get()->GetBaseObject()->GetFormType());

        // switch(a_event->ref.get()->GetBaseObject()->GetFormType())
        // {
        //     case RE::FormType::Door:
        //         AnimPlayer::GetSingleton()->PlayAnimation("UseDoor");
        //         break;
        // }
        
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl EventHandler::ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*)
    {
        auto cont = RE::BSEventNotifyControl::kContinue;
        if (!*a_evn) return cont;

       

        for (InputEvent* e = *a_evn; e; e = e->next) {

            if (!(e->eventType.get() == INPUT_EVENT_TYPE::kButton)) return cont;
            
            
            ButtonEvent* a_event = e->AsButtonEvent();
            uint32_t keyMask = a_event->idCode;
		    uint32_t keyCode;
            auto device = a_event->device.get();

            if (!a_event->IsUp()) continue;

            if (device == INPUT_DEVICE::kMouse)
            {
                keyCode = static_cast<int>(KeyUtil::KBM_OFFSETS::kMacro_NumKeyboardKeys) + keyMask;
            }
            else if (device == INPUT_DEVICE::kGamepad)
            {
                keyCode = KeyUtil::Interpreter::GamepadMaskToKeycode(keyMask);
            }
            else 
            {
                keyCode = keyMask;
            }
            auto control = std::string(a_event->QUserEvent().c_str());


            SKSE::log::info("User Event: {} :", control );

            if (control == "Activate") HandleActivatePress();
            
            
        }



        return cont;
    }
    void EventHandler::HandleActorPress(const Actor* target)
    {
        auto actorBase = target ? target->GetActorBase() : nullptr;
        if (!actorBase) return;
        SKSE::log::info("Looting Actor: {} :", actorBase->GetName());
        auto* animplyr = AnimPlayer::GetSingleton();

        if (target->IsDead())
        {
            animplyr->PlayAnimation("SearchKneel");
        }
        
    }

    void EventHandler::HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr)
    {
        auto base = refr ? refr->GetBaseObject() : nullptr;
        if (!base) return;
        SKSE::log::info("Looting Object: {} :", base->GetName());
        auto* animplyr = AnimPlayer::GetSingleton();

        switch(base->GetFormType())
        {
        case FormType::Door:
            animplyr->PlayAnimation("UseDoor");

            break;
        case FormType::Container:
            animplyr->PlayAnimation("SearchChest");
            break;
        }
    }
    void EventHandler::HandleActivatePress()
    {
        const auto crossHairPickData = RE::CrosshairPickData::GetSingleton();

        if (!crossHairPickData) return;
        const auto target = crossHairPickData->target.get();
        const auto* actorTarget = crossHairPickData->targetActor.get().get();

        
        if (target) HandleObjectPress(target);
        if (actorTarget) HandleActorPress(actorTarget->As<RE::Actor>());
    }
}
