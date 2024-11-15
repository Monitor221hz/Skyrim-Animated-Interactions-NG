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
        auto* ref3d = ref->Get3D(); 
        if (idlePlaying || ref3d == nullptr)
            return;
        float refZ = ref3d->world.translate.z; 
        float y_diff = (float)(refZ - plyr->GetPositionZ() - 64.0f);
        float x_diff = (float)(sqrt(pow(ref->GetPositionX() - plyr->GetPositionX(), 2) + pow(ref->GetPositionY() - plyr->GetPositionY(), 2)));
        SKSE::log::info("Player Object Diff: {}", y_diff);
        PlayerUpdateHook::QueueSpinePitch(plyr, x_diff, y_diff); 
        PlayerUpdateHook::QueueAnimation("TakeCustom"); 
        // if (y_diff > settings->GetHighTakeBound())
        // {
        //     PlayerUpdateHook::QueueAnimation("TakeHigh");
        //     PlayerUpdateHook::QueueAnimationSpinePitch(x_diff, y_diff); 
        //     // anim_plyr->PlayAnimation("TakeHigh");
        // }
        // else if (y_diff < settings->GetLowTakeBound())
        // {
        //     PlayerUpdateHook::QueueAnimation("TakeLow");
        //     // anim_plyr->PlayAnimation("TakeLow");
        // }
        // else
        // {
        //     PlayerUpdateHook::QueueAnimation("Take");
        //     // anim_plyr->PlayAnimation("Take");
        // }
        
    }
    void TakeHandler::StoreReferenceMesh(TESObjectREFR *refr)
        {
            WriteLocker locker(itemLock);
            TakenObjectType = FormTypeToString(refr->GetBaseObject()->GetFormType());
            
            refr->Load3D(false);

            auto* ref3D = refr->Get3D2();
            if (ref3D == ReferenceMesh || ref3D == nullptr) return;
            ReferenceMesh = refr->Get3D2();
            auto* ref = ReferenceMesh->GetUserData(); 
            if (ref)
            {
                auto* base = ref->GetObjectReference();
                if (base)
                {
                    auto* model = base->As<RE::TESModel>();
                    if (model != nullptr)
                    {
                        SelectedConfig = ConfigManager::GetMeshConfigPtr(model->GetModel());

                        SKSE::log::info("Reference mesh obtained");
                        if (SelectedConfig == nullptr)
                        {
                            SKSE::log::info("Config for {} not found, using default alignment", model->GetModel());
                            SelectedConfig = ConfigManager::GetFormConfigPtr(refr->GetBaseObject()->GetFormType());
                            if (SelectedConfig == nullptr)
                                SKSE::log::warn("Default alignment for {} not found", refr->GetBaseObject()->GetFormType());
                        }
                    }

                }
            }
            refr->Load3D(true);
            SKSE::log::info("Reference mesh obtained");
        }

        NiNode* TakeHandler::GetAttachNode(NiAVObject* animObjectMesh)
        {
            auto* root = animObjectMesh->AsFadeNode();         
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
             

             auto geometries = NifUtil::Node::GetAllGeometries(ReferenceMesh); 
             if (SelectedConfig != nullptr) 
             {
                node->local = SelectedConfig->transform; 
             }
             for (auto* geom : geometries)
             {
                // auto* geom = geom_ptr.get().get();
                if (!geom) { continue; }

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