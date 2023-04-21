
#pragma once
#include "anim.h"

namespace InteractiveIdles
{
    class Hook
    {
        public:
        static void InstallPickUpHook()
        {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(16);
            REL::Relocation<std::uintptr_t> target{REL::RelocationID(39456,0), REL::Relocate(0x5FA, 0)};
            _PickUp = trampoline.write_call<5>(target.address(), PickUp);

            SKSE::log::info("Hook: Pick up initialized");
        }
        static void InstallHooks()
		{
            // InstallStealHook();
            // InstallLootHook();
            InstallPickUpHook();
            //PlayerCharacter__PickUpObject_1406A5300+4B7	call    TESObjectREFR__sub_14028D880


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

//	p	PlayerCharacter__PickUpObject_1406A5300+5FA	call    TESObjectREFR__RemoveReferenceHavokData
		}
        private:

            static int32_t PickUp(RE::TESObjectREFR* ref, const char* unk);



            static inline REL::Relocation<decltype(PickUp)> _PickUp;

            
    };
}