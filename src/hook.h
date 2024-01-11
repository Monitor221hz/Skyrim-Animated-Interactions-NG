
#pragma once
#include "anim.h"
#include "takehandler.h"
using namespace RE;
namespace AnimatedInteractions
{
    class PlayerPickUpHook
    {
        static void Install()
        {
            SKSE::log::info("Installing Hook: Player Pick Up");

            REL::Relocation<std::uintptr_t> playerVtbl{VTABLE_PlayerCharacter[0]}; 

            _PickUpItem = playerVtbl.write_vfunc(0xCC, PickUpItem);

            SKSE::log::info("Installed Hook: Player Pick Up");
        }

        private:

        static void PickUpItem(Actor* a_actor, TESObjectREFR* a_ref, std::int32_t a_count, bool a_arg3, bool a_playSound);

        static inline REL::Relocation<decltype(PickUpItem)> _PickUpItem;

    };
    class Hook
    {
    public:
        static void InstallPickUpHook()
        {
            SKSE::log::info("Installing Hook: Pick Up");

            REL::Relocation<std::uintptr_t> characterVtbl{VTABLE_Character[0]}; 

            // auto &trampoline = SKSE::GetTrampoline();
            // SKSE::AllocTrampoline(16);
            // REL::Relocation<std::uintptr_t> target{REL::RelocationID(39456, 0), REL::Relocate(0x5FA, 0)};
            // _PickUp = trampoline.write_call<5>(target.address(), PickUp);

            SKSE::log::info("Hook: Pick up installed");
        }
        // Down	p	PlayerCharacter__sub_1405E3700+98	call    TESObjectREFR__Activate_140296C00
        static void InstallInputHook()
        {
            // SKSE::log::info("Installing Hook: Activate");
            // REL::Relocation<std::uintptr_t> doorVtbl{ RE::VTABLE_TESObjectCONT[0] };

            // _UseDoor = doorVtbl.write_vfunc(0x37, &UseDoor);
            // SKSE::log::info("Hook: Activate installed");

            REL::Relocation<std::uintptr_t> inputVtbl{VTABLE_ActivateHandler[0]};
            _ProcessButton = inputVtbl.write_vfunc(0x4, ProcessButton);
            SKSE::log::info("Hook: Button Input installed");

            // REL::Relocation<std::uintptr_t> activateVtbl{ RE::VTABLE_ActivateHandler[0]};

            // _ProcessActivate = activateVtbl.write_vfunc(0x01, ProcessActivate);

            // SKSE::log::info("Hook: Activate Handler installed");
        }

        static void InstallAnimObjectHook()
        {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(14);
            REL::Relocation<std::uintptr_t> target{ REL::RelocationID(42420, 43576), REL::Relocate(0x22A, 0x21F) };  //AnimationObjects::Load
		    _LoadAnimObject = trampoline.write_call<5>(target.address(), LoadAnimObject); 

            SKSE::log::info("Hook AnimObject installed"); 
        }

        static void InstallHooks()
        {
            // InstallStealHook();
            // InstallLootHook();
            InstallAnimObjectHook(); 
            InstallInputHook();
            // InstallPickUpHook();
            // PlayerCharacter__PickUpObject_1406A5300+4B7	call    TESObjectREFR__sub_14028D880

            // SKSE::log::info("Doors hooked");
            SKSE::log::info("Hooks installed");

            // steal
            // REL (39456,40533)
            // SE PlayerCharacter__PickUpObject_1406A5300+2C6	call    TESObjectREFR__sub_14069FE60
            // AE sub_1406CD770+2C8	call    sub_1406C8580

            // pickup
            // REL (50007,50951)
            // SE sub_1408528D0+13F	call    TESObjectREFR__sub_14069FE60
            // AE sub_14087F4A0+13F	call    sub_1406C8580

            //	p	PlayerCharacter__PickUpObject_1406A5300+5FA	call    TESObjectREFR__RemoveReferenceHavokData
        }

    private:
        static int32_t PickUp(RE::TESObjectREFR *ref, const char *unk);

        static void ProcessButton(RE::ActivateHandler* a_this,ButtonEvent *a_event, PlayerControlsData *a_data);
        static void FaceObject(TESObjectREFR* refr);
        static void HandleActivatePress();
        static void HandleActorPress(const Actor *target);
        static void HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr);
        static void HandlePickUp(RE::TESObjectREFR *ref);
        static RE::NiAVObject* LoadAnimObject(RE::TESModel* a_model, RE::BIPED_OBJECT a_bipedObj, RE::TESObjectREFR* a_actor, RE::BSTSmartPointer<RE::BipedAnim>& a_biped, RE::NiAVObject* a_root);
        
        static inline REL::Relocation<decltype(LoadAnimObject)> _LoadAnimObject; 
        
        static inline REL::Relocation<decltype(PickUp)> _PickUp;

        static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
    };
}