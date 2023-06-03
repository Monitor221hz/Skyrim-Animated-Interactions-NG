#pragma once

using namespace RE;
namespace AnimatedInteractions
{
    //if ((menu->menuFlags & static_cast<Flag>(MenuFlagEx::kUnpaused)) != Flag::kNone)
    class EventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>, public RE::BSTEventSink<RE::TESOpenCloseEvent>, public RE::BSTEventSink<RE::InputEvent*>
    {
        public:
            static void Install()
            {
                auto* self = GetSingleton();
                RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(self);
                // RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESOpenCloseEvent>(self);
                // BSInputDeviceManager::GetSingleton()->AddEventSink(self);

                SKSE::log::info("UI Sink added");
            }
            static EventHandler *GetSingleton()
            {
                static EventHandler singleton;
                return &singleton;
            }

            void HandleActivatePress();
            void HandleActorPress(const Actor* target);
            void HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr);

            RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_evn, RE::BSTEventSource<RE::InputEvent*>*) override;
            RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
            RE::BSEventNotifyControl ProcessEvent(const RE::TESOpenCloseEvent* a_event, RE::BSTEventSource<RE::TESOpenCloseEvent>* a_eventSource) override;

    };
}