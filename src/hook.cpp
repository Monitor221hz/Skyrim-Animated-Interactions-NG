#include "hook.h"
#include "anim.h"
namespace InteractiveIdles
{
    

     void Hook::HandleClose(RE::TESObjectREFR* a_target, RE::TESObjectREFR* a_activator)
     {
        // if (a_activator)
        // {
        //     SKSE::log::info("Door Activator Type: {}", a_activator->GetBaseObject()->GetFormType());
        // }
        // SKSE::log::info("Door Target Type: {}", a_target->GetBaseObject()->GetFormType());
        
        if (a_target->IsPlayerRef())
        {
            SKSE::log::info("Player Closed Door");
            AnimPlayer::GetSingleton()->PlayUseDoor(RE::PlayerCharacter::GetSingleton());
            
        }
        _HandleClose(a_target, a_activator);
     }

     void Hook::HandleOpen(RE::TESObjectREFR* a_target, RE::TESObjectREFR* a_activator)
     {
        // if (a_activator)
        // {
        //     SKSE::log::info("Door Activator Type: {}", a_activator->GetBaseObject()->GetFormType());
        // }
        // SKSE::log::info("Door Target Type: {}", a_target->GetBaseObject()->GetFormType());

        if (a_target->IsPlayerRef())
        {
            SKSE::log::info("Player Opened Door");
            AnimPlayer::GetSingleton()->PlayUseDoor(RE::PlayerCharacter::GetSingleton());
        }
        _HandleOpen(a_target, a_activator);

     }

     void Hook::PickupAnimationTrigger(RE::TESForm* item, RE::TESForm* containerOwner, RE::TESObjectREFR* containerRef, RE::PlayerCharacter::EventType eventType)
     {
        SKSE::log::info("Item picked up!");
        bool equipOk = false;
        auto* plyr = RE::PlayerCharacter::GetSingleton();
        plyr->GetGraphVariableBool("bEquipOK", equipOk);
        if (equipOk)
        {
            
            if (plyr->GetPositionZ() >= containerRef->GetPositionZ())
            {

                AnimPlayer::GetSingleton()->PlayPickUp(plyr);
            }
            else 
            {
                AnimPlayer::GetSingleton()->PlayPickUpLow(plyr);
            }
        }
        _PlayStealEvent(item, containerOwner, containerRef, eventType);
     }

     void Hook::PlayLootEvent(RE::TESForm* a_item, RE::TESForm* a_containerOwner, RE::TESObjectREFR* a_containerRef, RE::PlayerCharacter::EventType eventType)
     {
        PickupAnimationTrigger(a_item, a_containerOwner, a_containerRef, eventType);
        _PlayLootEvent(a_item, a_containerOwner, a_containerRef, eventType);
     }

     void Hook::PlayStealEvent(RE::TESForm* a_item, RE::TESForm* a_containerOwner, RE::TESObjectREFR* a_containerRef, RE::PlayerCharacter::EventType eventType)
     {
         PickupAnimationTrigger(a_item, a_containerOwner, a_containerRef, eventType);
        _PlayStealEvent(a_item, a_containerOwner, a_containerRef, eventType);
     }
}
