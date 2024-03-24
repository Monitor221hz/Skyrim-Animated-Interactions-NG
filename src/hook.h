
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

        static inline float* delta_time_ptr = nullptr;
        
        static inline float start_angle_z = 0.0f;
        static inline float desired_angle_z = 0.0f; 

        static inline float last_angle_z = 0.0f; 
        static inline float rotate_z_mult = 1.0f;

        static inline std::atomic should_interp = false;

        static inline std::queue<std::string> animation_queue; 

        static void SetTurnState(Actor* a_actor, int turn_state);

        static void PlayQueuedAnimationTask();
        static void PlayQueuedAnimation();
        static float InterpAngleZ(float a_current, float a_interpSpeed, float& o_delta);
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
            delta_time_ptr = (float*)RELOCATION_ID(523660, 410199).address();
        }
        static void FaceObject(TESObjectREFR* refr);
        static void QueueRotationZ(float strt_angle_z, float end_angle_z);
        static void QueueAnimationPostRotate(std::string a_name);

        
        



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

            static inline std::atomic can_listen = true;
        public:
        static void Install()
        {
            REL::Relocation<std::uintptr_t> inputVtbl{VTABLE_ActivateHandler[0]};
            _ProcessButton = inputVtbl.write_vfunc(0x4, ProcessButton);
            SKSE::log::info("Hook: Button Input installed");
        }
    };
    class PlayerActivateHook
    {
        private:
        static bool ActivateRef(TESObjectREFR* a_ref, TESObjectREFR* a_activate_trigger, uint8_t a_arg2, TESBoundObject* a_object, int32_t a_count, bool a_defaultProcessingOnly);
        static inline REL::Relocation<decltype(ActivateRef)> _ActivateRef; 

        struct Activation
        {
            TESObjectREFR* ref = nullptr; 
            TESObjectREFR* activate_trigger = nullptr; 
            uint8_t arg2 = 0; 
            TESBoundObject* bound_object = nullptr; 
            int32_t count = 0;
            bool default_processing_only = false;
            Activation() {}
            Activation(TESObjectREFR* a_ref, TESObjectREFR* a_activate_trigger, uint8_t a_arg2, TESBoundObject* a_object, int32_t a_count, bool a_default_processing_only) :
            ref(a_ref), activate_trigger(a_activate_trigger), arg2(a_arg2), bound_object(a_object), count(a_count), default_processing_only(a_default_processing_only) {}
        };

        static inline Activation current_activation; 
        public:
        static void TriggerStored();
        static void Install()
        {
            //	p	TESObjectREFR__Activate_140296C00+9D	call    TESObjectREFR__Activate_140296C00 X
            //	p	TESObjectREFR__sub_1402A8CC0+D0	call    TESObjectREFR__Activate_140296C00 X
            //Up	p	TESObjectCONT__Activate_14022B6D0+164	call    TESObjectREFR__Activate_140296C00 17485 X 
            //	p	TESObjectREFR__sub_1402A8E20+D3	call    TESObjectREFR__Activate_140296C00 19858 X 
            //	p	PlayerCharacter__sub_1405E3700+98	call    TESObjectREFR__Activate_140296C00 36496 X 
            //	p	Character__sub_140602410+1E6	call    TESObjectREFR__Activate_140296C00 36854 X (NPC interact) 
            // Down	p	PlayerCharacter__sub_1406A9F90+135	call    TESObjectREFR__Activate_140296C00 39471
            auto& trampoline = SKSE::GetTrampoline(); 
            SKSE::AllocTrampoline(14);
            REL::Relocation<std::uintptr_t> target{ REL::RelocationID(39471, 0x0), REL::Relocate(0x135, 0x0)}; 
            _ActivateRef = trampoline.write_call<5>(target.address(), ActivateRef);

            SKSE::log::info("Activate Hook installed");
        }
    };
    using EventResult = RE::BSEventNotifyControl;
    class PlayerGraphEventHook
    {
    public:
        static void Install()
        {
            REL::Relocation<uintptr_t> AnimEventVtbl_PC{RE::VTABLE_PlayerCharacter[2]};
            _ProcessEvent = AnimEventVtbl_PC.write_vfunc(0x1, ProcessEvent);
            SKSE::log::info("Hook Installed: Player Graph Event");
        }

        static void ActivateCallback();

    private:
        static EventResult ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_sink, RE::BSAnimationGraphEvent *a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource);
        static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;

        static inline std::atomic is_active { false }; 
    };
    
    class AnimObjectHook
    {
    public:
        static void Install()
        {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(14);
            REL::Relocation<std::uintptr_t> target{ REL::RelocationID(42420, 43576), REL::Relocate(0x22A, 0x21F) };  //AnimationObjects::Load
		    _LoadAnimObject = trampoline.write_call<5>(target.address(), LoadAnimObject); 

            SKSE::log::info("Hook AnimObject installed"); 
        }

    private:
        

        static RE::NiAVObject* LoadAnimObject(RE::TESModel* a_model, RE::BIPED_OBJECT a_bipedObj, RE::TESObjectREFR* a_actor, RE::BSTSmartPointer<RE::BipedAnim>& a_biped, RE::NiAVObject* a_root);
        
        static inline REL::Relocation<decltype(LoadAnimObject)> _LoadAnimObject; 



    };
}