
#pragma once
#include "anim.h"
#include "takehandler.h"
using namespace RE;
namespace AnimatedInteractions
{
    class PlayerPickUpHook
    {
        public:
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

    class PlayerUpdateHook
    {
        private:
        static void UpdatePlayer(RE::Actor *a_this, float a_delta);
        static inline REL::Relocation<decltype(UpdatePlayer)> _UpdatePlayer;

        static inline float* delta_time = nullptr;

        static inline float desired_angle_z = 0.0f; 
        static inline float cam_desired_angle_z = 0.0f; 

        static inline float last_angle = 0.0f; 


        static inline std::atomic should_interp = false;

        static inline std::queue<std::string> animation_queue; 

        static void SetTurnState(Actor* a_actor, int turn_state);
        static void PlayQueuedAnimation();
        
        public:
        static void Install()
        {
            SKSE::log::info("Installing Hook: Player Update"); 

            REL::Relocation<std::uintptr_t> playerVtbl{VTABLE_PlayerCharacter[0]};

            _UpdatePlayer = playerVtbl.write_vfunc(0xAD, UpdatePlayer);

            SKSE::log::info("Installed Hook: Player Update"); 
        }
        static void Load()
        {
            delta_time = (float*)RELOCATION_ID(523660, 410199).address();
        }
        static void QueueRotationZ(float angle_z, float cam_angle_z);
        static void QueueAnimationInterpEnd(std::string a_name);

        
        



    };
    class InputHook
    {
        private:
            static void HandleActivatePress();
            static void HandleActorPress(const Actor *target);
            static void HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr);
            static void HandlePickUp(RE::TESObjectREFR *ref);
            static void ProcessButton(RE::ActivateHandler *a_this, ButtonEvent *a_event, PlayerControlsData *a_data);
            static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
            static void FaceObject(TESObjectREFR* refr);
            static inline std::atomic can_listen = true;
        public:
        static void Install()
        {
            REL::Relocation<std::uintptr_t> inputVtbl{VTABLE_ActivateHandler[0]};
            _ProcessButton = inputVtbl.write_vfunc(0x4, ProcessButton);
            SKSE::log::info("Hook: Button Input installed");
        }
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
        

        static RE::NiAVObject* LoadAnimObject(RE::TESModel* a_model, RE::BIPED_OBJECT a_bipedObj, RE::TESObjectREFR* a_actor, RE::BSTSmartPointer<RE::BipedAnim>& a_biped, RE::NiAVObject* a_root);
        
        static inline REL::Relocation<decltype(LoadAnimObject)> _LoadAnimObject; 



    };
}