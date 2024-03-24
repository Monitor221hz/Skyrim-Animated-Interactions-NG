#include "takehandler.h"
#include "anim.h"
#include "hook.h"
namespace AnimatedInteractions
{
       void TakeHandler::HandlePickUp(RE::TESObjectREFR *ref)
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
            PlayerUpdateHook::QueueAnimationPostRotate("TakeHigh");
            // anim_plyr->PlayAnimation("TakeHigh");
        }
        else if (dif < settings->GetLowTakeBound())
        {
            PlayerUpdateHook::QueueAnimationPostRotate("TakeLow");
            // anim_plyr->PlayAnimation("TakeLow");
        }
        else
        {
            PlayerUpdateHook::QueueAnimationPostRotate("Take");
            // anim_plyr->PlayAnimation("Take");
        }
        
    }
    void TakeHandler::StoreReferenceMesh(TESObjectREFR *refr)
        {
             WriteLocker locker(itemLock);
            TakenObjectType = FormTypeToString(refr->GetBaseObject()->GetFormType());
            
            refr->Load3D(false);

            auto* ref3D = refr->Get3D2();
            if (ref3D == ReferenceMesh) return;
            ReferenceMesh = refr->Get3D2();
            
            refr->Load3D(true);
            SKSE::log::info("Reference mesh obtained");
        }

        NiNode* TakeHandler::GetAttachNode(NiAVObject* animObjectMesh)
        {
            auto* root = animObjectMesh->AsFadeNode(); 
            auto* userRef = root->GetUserData();            
            NiNode* defaultAttachNode = root->GetObjectByName("Attach")->AsNode(); 
            return defaultAttachNode;
        }



        int32_t TakeHandler::ConstructNiStream(char *address, void *memory, void* num)
        {
            using func_t = decltype(ConstructNiStream);
            REL::Relocation<func_t> func{RELOCATION_ID(74039, 0)};
            return func(address, memory, num);
        }

        void TakeHandler::DestructNiStream(NiStream *niStream)
        {
            using func_t = decltype(DestructNiStream); 
            REL::Relocation<func_t> func{RELOCATION_ID(68972, 0)}; 
            func(niStream); 
        }

        NiAVObject* TakeHandler::GetMeshForTakenObject(NiAVObject* original)
        {
            WriteLocker locker(itemLock);
            
            
            if (ReferenceMesh == nullptr) SKSE::log::info("Null Extract"); 
            
             
             auto* node = GetAttachNode(original);  
             Config config; 
             

             std::vector<BSGeometry*> geometries = NifUtil::Node::GetAllGeometries(ReferenceMesh); 
             if (SelectedConfig != nullptr) node->local = SelectedConfig->transform; 
             for (auto* geom : geometries)
             {
                auto* clone = NifUtil::Node::Clone(geom);
                
                SKSE::log::info("Trishape Attached: {}", geom->name.c_str());
                node->AttachChild(clone, true); 
             }
            // SKSE::log::info("Num children {}", root->GetChildren().size());
            //  auto *user = ReferenceMesh->GetUserData();
            //  if (user)
            //     user->SetDelete(true);


             return original->AsFadeNode();
        }

        bool TakeHandler::IsReplaceable(TESObjectANIO *animObject)
        {
            ReadLocker locker(itemLock); 
            return (usedAnimObjects.count(animObject) > 0);
        }


}