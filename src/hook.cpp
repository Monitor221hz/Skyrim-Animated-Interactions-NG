#include "hook.h"
#include "settings.cpp"
#include "util.h"
namespace AnimatedInteractions
{

    void Hook::HandlePickUp(RE::TESObjectREFR *ref)
    {
        bool idlePlaying = false;
        auto *plyr = RE::PlayerCharacter::GetSingleton();
        auto *settings = Settings::GetSingleton();
        plyr->GetGraphVariableBool("bForceIdleStop", idlePlaying);

        if (idlePlaying)
            return;

        auto dif = ref->GetPositionZ() - plyr->GetPositionZ() - 64.0;

        SKSE::log::info("Player Object Diff: {}", dif);
        if (dif > settings->GetHighTakeBound())
        {
            AnimPlayer::GetSingleton()->PlayAnimation(plyr, "TakeHigh"); // high
        }
        else if (dif < settings->GetLowTakeBound())
        {
            AnimPlayer::GetSingleton()->PlayAnimation(plyr, "TakeLow"); // low
        }
        else
        {
            AnimPlayer::GetSingleton()->PlayAnimation(plyr, "Take"); // mid
        }
    }

    int32_t Hook::PickUp(RE::TESObjectREFR *ref, const char *unk)
    {
        bool idlePlaying = false;
        auto *plyr = RE::PlayerCharacter::GetSingleton();
        plyr->GetGraphVariableBool("bForceIdleStop", idlePlaying);

        if (idlePlaying)
            return _PickUp(ref, unk);

        AnimatedInteractions::AnimPlayer::GetSingleton()->ApplyAnimationSpeed(RE::PlayerCharacter::GetSingleton());
        auto plyrPos = plyr->GetPositionZ() + 40.0;
        SKSE::log::info("Player Z: {} Object Z: {}", plyrPos, ref->GetPositionZ());

        if (plyrPos <= ref->GetPositionZ())
        {
            AnimPlayer::GetSingleton()->PlayAnimation(plyr, "PickUp");
        }
        else
        {
            AnimPlayer::GetSingleton()->PlayAnimation(plyr, "PickUpLow");
        }

        return _PickUp(ref, unk);
    }

    void Hook::ProcessButton(RE::ActivateHandler *a_this, ButtonEvent *a_event, PlayerControlsData *a_data)
    {
        uint32_t keyMask = a_event->idCode;
        uint32_t keyCode;
        auto device = a_event->device.get();

        if (a_event->IsUp() || PlayerCharacter::GetSingleton()->IsGrabbing() || a_event->HeldDuration() > 0.2)
        {
            _ProcessButton(a_this, a_event, a_data);
            return;
        }

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
            HandleActivatePress();

        _ProcessButton(a_this, a_event, a_data);
    }

    void Hook::HandleActorPress(const Actor *target)
    {
        auto actorBase = target ? target->GetActorBase() : nullptr;
        if (!actorBase)
            return;
        SKSE::log::info("Looting Actor: {} :", actorBase->GetName());
        auto *animplyr = AnimPlayer::GetSingleton();

        // if (target->IsDead())animplyr->PlayAnimation("SearchKneel");
    }
    void Hook::FaceObject(TESObjectREFR* refr)
    {
        auto* plyr = PlayerCharacter::GetSingleton(); 
        auto* playerCamera = RE::PlayerCamera::GetSingleton();
        auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
        float headingAngle =MathUtil::Angle::RadianToDegree(plyr->GetHeadingAngle(refr->GetPosition(), false)); 
        float newAngle = plyr->GetAngleZ() + headingAngle ; //radians
        
        // ObjectUtil::Transform::TranslateTo(nullptr, 0, plyr, plyr->GetPositionX(), plyr->GetPositionY(), plyr->GetPositionZ(), 
        // plyr->GetAngleX(), plyr->GetAngleY(), newAngle, 1000.0, 50.0); 
        plyr->SetRotationZ(newAngle);
        thirdPersonState->freeRotation.x -= headingAngle; 
    }
    void Hook::HandleObjectPress(RE::NiPointer<RE::TESObjectREFR> refr)
    {
        auto base = refr ? refr->GetBaseObject() : nullptr;
        auto* ref = refr.get();
        if (!base)
            return;
        
        auto *animplyr = AnimPlayer::GetSingleton();

        switch (base->GetFormType())
        {
        case FormType::Door:
            // FaceObject(ref); 
            animplyr->PlayAnimation("UseDoor");
            
            break;
        case FormType::Container:
            // FaceObject(ref); 
            animplyr->PlayAnimation("SearchChest");
            break;
        case FormType::Activator:
            // FaceObject(ref); 
            break;
        case FormType::TalkingActivator:
            // FaceObject(ref); 
            break;
        case FormType::NPC:
            break;
        case FormType::Static:
            break;
        case FormType::MovableStatic:
            break;
        case FormType::Furniture:
            // FaceObject(ref); 
            break; 
        case FormType::PlacedHazard:
            break;
        default:
            // TakeHandler::SetLastTakenItem(base); 
            TakeHandler::StoreReferenceMesh(refr.get());
            HandlePickUp(refr.get());
            break;
        }
    }
    void Hook::HandleActivatePress()
    {

        const auto crossHairPickData = RE::CrosshairPickData::GetSingleton();

        if (!crossHairPickData)
            return;
        const auto target = crossHairPickData->target.get();
        const auto *actorTarget = crossHairPickData->targetActor.get().get();

        if (target)
            HandleObjectPress(target);
        if (actorTarget)
            HandleActorPress(actorTarget->As<RE::Actor>());
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
        _PickUpItem(a_actor, a_ref, a_count, a_arg3, a_playSound);
    }
}
