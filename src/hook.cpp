#include "hook.h"
namespace InteractiveIdles
{
    

     int32_t Hook::PickUp(RE::TESObjectREFR* ref, const char* unk)
     {
        bool equipOk = false;
        auto* plyr = RE::PlayerCharacter::GetSingleton();
        plyr->GetGraphVariableBool("bEquipOK", equipOk);
        if (equipOk)
        {
            InteractiveIdles::AnimPlayer::GetSingleton()->ApplyAnimationSpeed(RE::PlayerCharacter::GetSingleton());
            float plyrPos = plyr->GetPositionZ()+40.0;
            SKSE::log::info("Player Z: {} Object Z: {}", plyrPos, ref->GetPositionZ());
            
            if (plyrPos <= ref->GetPositionZ())
            {
                AnimPlayer::GetSingleton()->PlayPickUp(plyr);
            }
            else 
            {
                AnimPlayer::GetSingleton()->PlayPickUpLow(plyr);
            }
        }
        return _PickUp(ref, unk);
     }
}
