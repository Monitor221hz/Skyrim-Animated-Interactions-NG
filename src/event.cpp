#include "event.h"
#include "hook.h"
namespace AnimatedInteractions
{
    MenuOpenCloseEventHandler *MenuOpenCloseEventHandler::GetSingleton()
    {
        static MenuOpenCloseEventHandler singleton;
        return std::addressof(singleton);
    }

    void MenuOpenCloseEventHandler::Register()
    {
        auto ui = RE::UI::GetSingleton();
        ui->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
        SKSE::log::info("Registered {}", typeid(RE::MenuOpenCloseEvent).name());
    }

    RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent *a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent> *)
    {

        if (a_event)
            SKSE::log::info("Received RE::MenuOpenCloseEvent for {} with opening {}", a_event->menuName.c_str(), a_event->opening);

        // On HUD menu open/close - open/close the plugin's HUD menu
        if (a_event && !a_event->opening && a_event->menuName == RE::ContainerMenu::MENU_NAME)
        {
            if (auto *player = PlayerCharacter::GetSingleton())
            {
                player->NotifyAnimationGraph("InteractTransitionEnd");
                player->NotifyAnimationGraph("InteractLoopEnd");
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
}
