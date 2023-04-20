

namespace InteractiveIdles
{
    class Hook
    {
        public:
        static void InstallStealHook()
        {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(16);
            REL::Relocation<std::uintptr_t> target{REL::RelocationID(39456, 40533), REL::Relocate(0x2C6, 0x2C8)};
            _PlayStealEvent = trampoline.write_call<5>(target.address(), PlayStealEvent);

            SKSE::log::info("Hook: Steal initialized");
        }
        static void InstallLootHook()
        {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(16);
            REL::Relocation<std::uintptr_t> target{REL::RelocationID(50007,50951), REL::Relocate(0x13F, 0x13F)};
            _PlayLootEvent = trampoline.write_call<5>(target.address(), PlayLootEvent);

            SKSE::log::info("Hook: Loot initialized");
        }
        static void InstallHooks()
		{
            InstallStealHook();
            InstallLootHook();
			// REL::Relocation<std::uintptr_t> CharacterVtbl{ VTABLE[0] };
			// _Hook_Update = CharacterVtbl.write_vfunc(0x0AD, &Hook_Update);
			// SKSE::log::info("Hook Actor Update!");
            // REL::Relocation<std::uintptr_t> DoorVtbl{ RE::VTABLE_TESObjectDOOR[0]};//0x01
            // REL::Relocation<std::uintptr_t> DoorOVtbl{ RE::VTABLE_TESObjectDOOR[4]}; //0x02
            
            // _HandleClose = DoorOVtbl.write_vfunc(0x02, HandleClose);
            // _HandleOpen = DoorVtbl.write_vfunc(0x01, HandleOpen);

            // SKSE::log::info("Doors hooked");
            SKSE::log::info("Pickup events hooked");

//steal
//REL (39456,40533)
//SE PlayerCharacter__PickUpObject_1406A5300+2C6	call    TESObjectREFR__sub_14069FE60
//AE sub_1406CD770+2C8	call    sub_1406C8580


//pickup
//REL (50007,50951)
//SE sub_1408528D0+13F	call    TESObjectREFR__sub_14069FE60
//AE sub_14087F4A0+13F	call    sub_1406C8580
		}
        private:
            static void HandleClose(RE::TESObjectREFR* a_target, RE::TESObjectREFR* a_activator);
            static void HandleOpen(RE::TESObjectREFR* a_target, RE::TESObjectREFR* a_activator);
            static void PickupAnimationTrigger(RE::TESForm* a_item, RE::TESForm* a_containerOwner, RE::TESObjectREFR* a_containerRef, RE::PlayerCharacter::EventType eventType);

            static void PlayStealEvent(RE::TESForm* a_item, RE::TESForm* a_containerOwner, RE::TESObjectREFR* a_containerRef, RE::PlayerCharacter::EventType eventType);
            static void PlayLootEvent(RE::TESForm* a_item, RE::TESForm* a_containerOwner, RE::TESObjectREFR* a_containerRef, RE::PlayerCharacter::EventType eventType);

            static inline REL::Relocation<decltype(PlayStealEvent)> _PlayStealEvent;
            static inline REL::Relocation<decltype(PlayStealEvent)> _PlayLootEvent;
            static inline REL::Relocation<decltype(HandleOpen)> _HandleOpen;
            static inline REL::Relocation<decltype(HandleClose)> _HandleClose;

            
    };
}