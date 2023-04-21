#include "event.h"

#include "anim.h"
namespace InteractiveIdles
{
    RE::BSEventNotifyControl MenuHandler::ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* eventSource)
    {
        auto* plyr = RE::PlayerCharacter::GetSingleton();
        if (event->menuName == RE::ContainerMenu::MENU_NAME)
        {
            
            auto* menu = RE::UI::GetSingleton()->GetMenu<RE::ContainerMenu>(event->menuName).get();
            if (menu->GetContainerMode()==RE::ContainerMenu::ContainerMode::kLoot || menu->GetContainerMode()==RE::ContainerMenu::ContainerMode::kSteal)
            {
                RE::TESObjectREFRPtr refr;
                RE::LookupReferenceByHandle(menu->GetTargetRefHandle(), refr);
                
                if (event->opening)
                {
                    if (refr)
                    {
                        RE::FormType formType = refr.get()->GetBaseObject()->GetFormType();
                        InteractiveIdles::AnimPlayer::GetSingleton()->ApplyAnimationSpeed(RE::PlayerCharacter::GetSingleton());
                        SKSE::log::info("Container Menu Target Form Type: {}", formType);
                        switch (formType)
                        {
                        case RE::FormType::NPC:
                            SKSE::log::info("Looting actor.");
                            AnimPlayer::GetSingleton()->PlaySearchKneel(plyr);
                            break;
                        case RE::FormType::Furniture:
                            SKSE::log::info("Looting furniture");
                            AnimPlayer::GetSingleton()->PlayUseDoor(plyr);
                            break;
                        case RE::FormType::Container:
                            SKSE::log::info("Looting container");
                            AnimPlayer::GetSingleton()->PlaySearchChest(plyr);
                            break;
                        default:
                            SKSE::log::info("Default loot");
                            AnimPlayer::GetSingleton()->PlayUseDoor(plyr);
                            break;
                        }
                    }
                }
                else
                {

                    AnimPlayer::GetSingleton()->ExitAnimation(plyr);
                }
            }
            
        }
        else if(event->menuName == RE::LockpickingMenu::MENU_NAME)
        {
            SKSE::log::info("LOCKPICK opened", event->menuName);
            auto* menu = RE::UI::GetSingleton()->GetMenu<RE::LockpickingMenu>(event->menuName).get();
            if (event->opening)
            {
                    AnimPlayer::GetSingleton()->PlayLockPick(plyr);
            }
            else
            {
                    AnimPlayer::GetSingleton()->ExitAnimation(plyr);
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
}
