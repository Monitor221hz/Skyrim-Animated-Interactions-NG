#include "hook.h"
#include "settings.cpp"
#include "util.h"
namespace AnimatedInteractions
{


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


        // if (target->IsDead())animplyr->PlayAnimation("SearchKneel");
    }
    void PlayerUpdateHook::FaceObject(TESObjectREFR* refr)
    {
        auto* plyr = PlayerCharacter::GetSingleton(); 
        if (!plyr) { return; }

        float headingAngle = MathUtil::Angle::DegreeToRadian(plyr->GetHeadingAngle(refr->GetPosition(), false));
        float plyr_angle = plyr->GetAngleZ();

        float new_angle = plyr_angle + headingAngle;

        new_angle = MathUtil::Angle::NormalAbsoluteAngle(new_angle);

        SKSE::log::info("Player | Original Angle: {} | Heading Angle: {} | New Angle: {}", MathUtil::Angle::RadianToDegree(plyr_angle), MathUtil::Angle::RadianToDegree(headingAngle), MathUtil::Angle::RadianToDegree(new_angle));
       
        QueueRotationZ(new_angle);
    }
    void InputHook::HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr)
    {
        auto base = refr ? refr->GetBaseObject() : nullptr;
        auto* ref = refr.get();
        if (!base)
            return;
        


        switch (base->GetFormType())
        {
        case FormType::Door:
            break;
        case FormType::Container:
            break;
        case FormType::Activator:
            break;
        case FormType::TalkingActivator:
            break;
        case FormType::NPC:
            break;
        case FormType::Static:
            break;
        case FormType::MovableStatic:
            break;
        case FormType::Furniture:
            break; 
        case FormType::PlacedHazard:
            break;
        default:
            // auto* ref = refr.get();
            // TakeHandler::StoreReferenceMesh(ref);
            // HandlePickUp(ref);
            // PlayerUpdateHook::FaceObject(ref);
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

    RE::NiAVObject *AnimObjectHook::LoadAnimObject(RE::TESModel *a_model, RE::BIPED_OBJECT a_bipedObj, RE::TESObjectREFR *a_actor, RE::BSTSmartPointer<RE::BipedAnim> &a_biped, RE::NiAVObject *a_root)
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
        // if (a_actor && a_ref)
        // {
        //     TakeHandler::StoreReferenceMesh(a_ref);
        //     TakeHandler::HandlePickUpPlayer(a_ref);
        // }
        _PickUpItem(a_actor, a_ref, a_count, a_arg3, a_playSound);
    }
    void PlayerUpdateHook::UpdatePlayer(RE::Actor *a_player, float a_delta)
    {
        _UpdatePlayer(a_player, a_delta);
        if (!should_interp)
        {
            return;
        }
        float player_angle = a_player->data.angle.z;
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
        float angle_delta = MathUtil::Angle::NormalRelativeAngle(desired_angle_z - a_player->data.angle.z);
        float max_angle_delta = rotate_z_speed_mult * *delta_time_ptr;

        angle_delta = MathUtil::Angle::ClipAngle(angle_delta, -max_angle_delta, max_angle_delta);
        float camera_angle = third_person_state->freeRotation.x;
        
        player_angle = player_angle + angle_delta;

        a_player->SetRotationZ(player_angle);
        third_person_state->freeRotation.x = MathUtil::Angle::NormalRelativeAngle(camera_angle - angle_delta);
        

        if (round(player_angle*10.0f)/10.0f == round(desired_angle_z*10.0f)/10.0f || last_angle_z == player_angle)
        {
            SetTurnState(a_player, 1);
            PlayQueuedAnimationTask();
            should_interp.store(false);
            // SKSE::log::info("last_angle {} | player_angle {} | o_delta {}", MathUtil::Angle::RadianToDegree(last_angle_z), MathUtil::Angle::RadianToDegree(player_angle), o_delta);
            return; 
        }



        last_angle_z = player_angle;

        if (angle_delta * angle_delta < FLT_EPSILON)
        {
            desired_angle_z = -1.f;
        }
        SetTurnState(a_player, angle_delta  ? 0 : 2);

    }

    void PlayerUpdateHook::SetTurnState(Actor* a_actor, int turn_state)
    {
        // auto* fixed_strings = FixedStrings::GetSingleton();
        // if (!fixed_strings) { return; }

        // a_actor->SetGraphVariableInt(fixed_strings->iSyncTurnState, turn_state);
        SKSE::GetTaskInterface()->AddTask([a_actor, turn_state]()
        {
            a_actor->SetGraphVariableInt("iSyncTurnState",turn_state);
        });
    }
    void PlayerUpdateHook::PlayQueuedAnimationTask()
    {
        SKSE::GetTaskInterface()->AddTask([]() { PlayerUpdateHook::PlayQueuedAnimation(); }); 
    }
    void PlayerUpdateHook::PlayQueuedAnimation()
    {
        if (animation_queue.empty()) { return; }
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        PlayerActivateHook::SetActivationState(AnimPlayer::PlayAnimation(animation_queue.front()));
        animation_queue.pop();
    }
    float PlayerUpdateHook::InterpAngleZ(float a_current, float a_interpSpeed, float &o_delta)
    {
        o_delta = 0.f;
        float target = desired_angle_z;
        float delta_time = *delta_time_ptr;
        if (a_interpSpeed <= 0.f)
        {
            return desired_angle_z;
        }
        
        const float clockwise_distance = target - a_current;
        const float counterclockwise_distance = target + a_current;

        float final_distance= 0.f;

        if (MathUtil::Angle::NormalAbsoluteAngle(counterclockwise_distance) < MathUtil::Angle::NormalAbsoluteAngle(clockwise_distance))
        {
            // final_distance = -counterclockwise_distance;
            float end_angle = a_current > PI ? 2 * PI + 0.1f : 0.1f;
            final_distance = end_angle - a_current;
        }
        else 
        {
            final_distance = clockwise_distance;
        }

        if (final_distance * final_distance < FLT_EPSILON)
        {
            return desired_angle_z;
        }

        o_delta = final_distance * MathUtil::Clamp(delta_time * a_interpSpeed, 0.f, 1.f);

        return MathUtil::Angle::NormalAbsoluteAngle(a_current + o_delta);
    }
    void PlayerUpdateHook::QueueRotationZ(float end_angle_z)
    {
        if (should_interp)
        {
            return;
        }
        should_interp.store(true);
        rotate_z_speed_mult = static_cast<float>(Settings::GetSingleton()->GetRotationSpeed());
        desired_angle_z = end_angle_z;
    }
    void PlayerUpdateHook::QueueAnimationPostRotate(std::string a_name)
    {
        animation_queue.emplace(a_name);
    }
    bool PlayerActivateHook::ActivateRef(TESObjectREFR* a_ref, TESObjectREFR *a_activate_trigger, uint8_t a_arg2, TESBoundObject *a_object, int32_t a_count, bool a_defaultProcessingOnly)
    {
        // if (a_ref)
        // {
        //     SKSE::log::info("Ref {}", a_ref->GetName());
        // }
        // if (a_activate_trigger)
        // {
        //     SKSE::log::info("Trigger {}", a_activate_trigger->GetName());
        // }
        if (is_activating)
        {
            return Settings::GetSingleton()->GetAnimationBlockActivation() ? false : _ActivateRef(a_ref, a_activate_trigger, a_arg2, a_object, a_count, a_defaultProcessingOnly);
        }
        if (!a_ref || !a_activate_trigger) 
        {
            return _ActivateRef(a_ref, a_activate_trigger, a_arg2, a_object, a_count, a_defaultProcessingOnly);
        }
        current_activation = Activation(a_ref, a_activate_trigger, a_arg2, a_object, a_count, a_defaultProcessingOnly);
        auto* base = a_ref->GetBaseObject();
        switch(base->GetFormType())
        {
            case FormType::Door:
                PlayerUpdateHook::QueueAnimationPostRotate("UseDoor");
                break;
            case FormType::Container:
                PlayerUpdateHook::QueueAnimationPostRotate("SearchChest");
                break;
            case FormType::ActorCharacter:
                PlayerUpdateHook::QueueAnimationPostRotate("SearchKneel");
                break;
            case FormType::Activator:
                break;
            case FormType::TalkingActivator:
                break;
            case FormType::NPC:
            {
                auto* actor_ref = a_ref->As<RE::Actor>();
                if (actor_ref->IsDead(true))
                {
                    PlayerUpdateHook::QueueAnimationPostRotate("UseKneel");
                }
                break;
            }
            case FormType::Static:
                break;
            case FormType::MovableStatic:
                break;
            case FormType::Furniture:
                break;
            case FormType::PlacedHazard:
                break;
            default:
                TakeHandler::StoreReferenceMesh(a_ref);
                TakeHandler::HandlePickUp(a_ref);
                // is_activating.store(true);
                // current_activation = Activation();
                // return _ActivateRef(a_ref, a_activate_trigger, a_arg2, a_object, a_count, a_defaultProcessingOnly);
        }
        PlayerGraphEventHook::ActivateCallback();
        PlayerUpdateHook::FaceObject(a_ref);
        // AnimPlayer::GetSingleton()->PlayAnimation("")
        return true;
    }
    void PlayerActivateHook::TriggerStored()
    {
        if (current_activation.ref == nullptr) { return; }
        current_activation.ref->ActivateRef(current_activation.activate_trigger, current_activation.arg2, current_activation.bound_object, current_activation.count, current_activation.default_processing_only);
        current_activation = Activation();
        is_activating.store(false);
    }
    void PlayerGraphEventHook::ActivateCallback()
    {
        is_active.store(true);
    }
    EventResult PlayerGraphEventHook::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_sink, RE::BSAnimationGraphEvent *a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource)
    {
        if (!is_active) 
        { 
            // SKSE::log::info("Event {}", a_event->tag);
            return _ProcessEvent(a_sink, a_event, a_eventSource);
        }
        if (a_event->tag == "TriggerActivate")
        {
            PlayerActivateHook::TriggerStored();
        }
        else if (a_event->tag == "IdleStop")
        {
            PlayerActivateHook::Reset();
        }
        else 
        {
            return _ProcessEvent(a_sink, a_event, a_eventSource);
        }
        // SKSE::log::info("Event {}", a_event->tag);
        is_active.store(false);
        return _ProcessEvent(a_sink, a_event, a_eventSource);
    }
}
