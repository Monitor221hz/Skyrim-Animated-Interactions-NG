#include "hook.h"
#include "settings.cpp"
#include "util.h"
namespace AnimatedInteractions
{

    void InputHook::HandlePickUp(RE::TESObjectREFR *ref)
    {
        bool idlePlaying = false;
        auto *plyr = RE::PlayerCharacter::GetSingleton();
        auto *settings = Settings::GetSingleton();
        plyr->GetGraphVariableBool("bForceIdleStop", idlePlaying);

        if (idlePlaying)
            return;

        auto dif = ref->GetPositionZ() - plyr->GetPositionZ() - 64.0;
        auto* anim_plyr = AnimPlayer::GetSingleton();

        SKSE::log::info("Player Object Diff: {}", dif);
        if (dif > settings->GetHighTakeBound())
        {
            // PlayerUpdateHook::QueueAnimationInterpEnd("TakeHigh");
            anim_plyr->PlayAnimation("TakeHigh");
        }
        else if (dif < settings->GetLowTakeBound())
        {
            // PlayerUpdateHook::QueueAnimationInterpEnd("TakeLow");
            anim_plyr->PlayAnimation("TakeLow");
        }
        else
        {
            // PlayerUpdateHook::QueueAnimationInterpEnd("Take");
            anim_plyr->PlayAnimation("Take");
        }
        
    }



    void InputHook::ProcessButton(RE::ActivateHandler *a_this, ButtonEvent *a_event, PlayerControlsData *a_data)
    {
        uint32_t keyMask = a_event->idCode;
        uint32_t keyCode;
        auto device = a_event->device.get();

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
        auto control = static_cast<std::string>(a_event->QUserEvent());

        if (control == "Activate")
        {
            if (a_event->IsUp())
            {
                can_listen.store(true);
            }
            else if (can_listen)
            {
                can_listen.store(false);
                HandleActivatePress();
            }
        }
        _ProcessButton(a_this, a_event, a_data);
    }

    void InputHook::HandleActorPress(const Actor *target)
    {
        auto actorBase = target ? target->GetActorBase() : nullptr;
        if (!actorBase)
            return;
        SKSE::log::info("Looting Actor: {} :", actorBase->GetName());
        auto *animplyr = AnimPlayer::GetSingleton();

        // if (target->IsDead())animplyr->PlayAnimation("SearchKneel");
    }
    void InputHook::FaceObject(TESObjectREFR* refr)
    {
        auto* plyr = PlayerCharacter::GetSingleton(); 
        if (!plyr) { return; }
        auto* playerCamera = RE::PlayerCamera::GetSingleton();
        if (!playerCamera) { return; }
        auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
        if (!thirdPersonState) { return; }

        float headingAngle =MathUtil::Angle::DegreeToRadian(plyr->GetHeadingAngle(refr->GetPosition(), true));
        float plyr_angle = plyr->GetAngleZ();
        // float newAngle = plyr->GetAngleZ() + headingAngle ; //radians
        float new_angle = MathUtil::Angle::NormalAbsoluteAngle(plyr_angle + headingAngle);
        float new_camera_angle = MathUtil::Angle::NormalAbsoluteAngle(thirdPersonState->freeRotation.x - headingAngle);
        SKSE::log::info("Player | Original Angle: {} | Heading Angle: {} | New Angle: {}", MathUtil::Angle::RadianToDegree(plyr_angle), MathUtil::Angle::RadianToDegree(headingAngle), MathUtil::Angle::RadianToDegree(new_angle));
        
        PlayerUpdateHook::QueueRotationZ(new_angle, new_camera_angle);
    }
    void InputHook::HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr)
    {
        auto base = refr ? refr->GetBaseObject() : nullptr;
        auto* ref = refr.get();
        if (!base)
            return;
        
        auto *animplyr = AnimPlayer::GetSingleton();

        switch (base->GetFormType())
        {
        case FormType::Door:
            animplyr->PlayAnimation("UseDoor");
            FaceObject(ref); 

            break;
        case FormType::Container:
            animplyr->PlayAnimation("SearchChest");
            FaceObject(ref); 
            break;
        case FormType::Activator:
            FaceObject(ref); 
            break;
        case FormType::TalkingActivator:
            FaceObject(ref); 
            break;
        case FormType::NPC:
            break;
        case FormType::Static:
            break;
        case FormType::MovableStatic:
            break;
        case FormType::Furniture:
            FaceObject(ref); 
            break; 
        case FormType::PlacedHazard:
            break;
        default:
            TakeHandler::StoreReferenceMesh(refr.get());
            HandlePickUp(refr.get());
            FaceObject(ref);
            break;
        }
    }
    void InputHook::HandleActivatePress()
    {

        const auto crossHairPickData = RE::CrosshairPickData::GetSingleton();

        if (!crossHairPickData)
            return;
        const auto target = crossHairPickData->target.get();
        const auto *actorTarget = crossHairPickData->targetActor.get().get();

        if (target)
        {
            HandleObjectPress(target);
            return;
        }
            
        if (actorTarget)
        {
            HandleActorPress(actorTarget->As<RE::Actor>());
            return;
        }
            
    }

    RE::NiAVObject *Hook::LoadAnimObject(RE::TESModel *a_model, RE::BIPED_OBJECT a_bipedObj, RE::TESObjectREFR *a_actor, RE::BSTSmartPointer<RE::BipedAnim> &a_biped, RE::NiAVObject *a_root)
    {
        RE::TESModel *model = a_model;
        NiAVObject* output = _LoadAnimObject(model, a_bipedObj, a_actor, a_biped, a_root); 
        if (const auto animObject = PointerUtil::adjust_pointer<RE::TESObjectANIO>(a_model->GetAsModelTextureSwap(), -0x20); animObject)
        {
            if (TakeHandler::IsReplaceable(animObject))
            {

                
                output = TakeHandler::GetMeshForTakenObject(output); 
                 
                // model->SetModel(TakeHandler::GetAnimObjectPath().c_str()); 
                // SKSE::log::info("Animobject Path {}", TakeHandler::GetAnimObjectPath());
                //  if (const auto swappedAnimObject = TakeHandler::GetLinkedAnimObject()) { model = swappedAnimObject; } 
                 
            }
            
        }

         return output;
    }
    void PlayerPickUpHook::PickUpItem(Actor *a_actor, TESObjectREFR *a_ref, std::int32_t a_count, bool a_arg3, bool a_playSound)
    {
        if (a_actor && a_ref)
        {
            TakeHandler::StoreReferenceMesh(a_ref);
            TakeHandler::HandlePickUpPlayer(a_ref);
        }
        _PickUpItem(a_actor, a_ref, a_count, a_arg3, a_playSound);
    }
    void PlayerUpdateHook::UpdatePlayer(RE::Actor *a_player, float a_delta)
    {
        _UpdatePlayer(a_player, a_delta);
        if (!should_interp)
        {
            return;
        }
        float player_angle = a_player->GetAngleZ();
        auto *player_camera = RE::PlayerCamera::GetSingleton();
        if (!player_camera)
        {
            return;
        }

        auto third_person_state = static_cast<RE::ThirdPersonState *>(player_camera->currentState.get());
        if (!third_person_state)
        {
            return;
        }

        float camera_angle = third_person_state->freeRotation.x;


        float player_delta_time = *delta_time;
        player_angle = MathUtil::Interp::InterpTo(player_angle, desired_angle_z, player_delta_time, 10.0f);
        camera_angle = MathUtil::Interp::InterpTo(camera_angle, cam_desired_angle_z, player_delta_time, 10.0f);
        
        third_person_state->freeRotation.x = camera_angle;
        a_player->SetRotationZ(player_angle);


        if (round(player_angle*10.0f)/10.0f == round(desired_angle_z*10.0f)/10.0f || last_angle == player_angle)
        {
            SetTurnState(a_player, 1);
            PlayQueuedAnimation();
            should_interp.store(false);
            return; 
        }



        last_angle = a_player->GetAngleZ();

        SetTurnState(a_player, player_angle > last_angle ? 0 : 2);

    }

    void PlayerUpdateHook::SetTurnState(Actor* a_actor, int turn_state)
    {
        auto* fixed_strings = FixedStrings::GetSingleton();
        if (!fixed_strings) { return; }

        
        a_actor->SetGraphVariableInt(fixed_strings->iSyncTurnState, turn_state);
    }
    void PlayerUpdateHook::PlayQueuedAnimation()
    {
        if (animation_queue.empty()) { return; }
        auto* anim_player = AnimPlayer::GetSingleton();
        anim_player->PlayAnimation(animation_queue.front());
        // animation_queue.pop();
    }
    void PlayerUpdateHook::QueueRotationZ(float angle_z, float cam_angle_z)
    {
        if (should_interp)
        {
            return;
        }
        should_interp.store(true);
        desired_angle_z = angle_z;
        cam_desired_angle_z = cam_angle_z;
    }
    void PlayerUpdateHook::QueueAnimationInterpEnd(std::string a_name)
    {
        animation_queue.emplace(a_name);
    }
}
