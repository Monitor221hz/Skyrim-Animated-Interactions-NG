namespace InteractiveIdles
{
    //if ((menu->menuFlags & static_cast<Flag>(MenuFlagEx::kUnpaused)) != Flag::kNone)
    class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
        public:
            static void AddHook()
            {
                RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
                SKSE::log::info("UI Sink added");
            }
            static MenuHandler *GetSingleton()
            {
                static MenuHandler singleton;
                return &singleton;
            }
            RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

    };
}