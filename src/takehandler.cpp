#include "takehandler.h"

namespace AnimatedInteractions
{
    void TakeHandler::StoreReferenceMesh(TESObjectREFR *refr)
        {
            WriteLocker locker(itemLock);
            TakenObjectType = FormTypeToString(refr->GetBaseObject()->GetFormType());
            
            refr->Load3D(false);

            auto* ref3D = refr->Get3D2();
            if (ref3D == ReferenceMesh) return;
            ReferenceMesh = refr->Get3D2();//to do: fix missing mesh after getting interrupted by menu open
            
            RE::TESModel* model = ReferenceMesh->GetUserData()->GetObjectReference()->As<RE::TESModel>();
             
            SelectedConfig = ConfigManager::GetMeshConfigPtr(model->GetModel());
            if (SelectedConfig == nullptr) 
            {
                SKSE::log::info("Config for {} not found, using default alignment", model->GetModel());
                SelectedConfig = ConfigManager::GetFormConfigPtr(refr->GetBaseObject()->GetFormType());
                if (SelectedConfig == nullptr) SKSE::log::warn("Default alignment for {} not found", refr->GetBaseObject()->GetFormType());
            }
            // refr->Load3D(true);
        }

        NiNode* TakeHandler::GetAttachNode(NiAVObject* animObjectMesh)
        {
            auto* root = animObjectMesh->AsFadeNode(); 
            auto* userRef = root->GetUserData();            
            NiNode* defaultAttachNode = root->GetObjectByName("Attach")->AsNode(); 
            return defaultAttachNode;
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

            return original->AsFadeNode();
        }

        bool TakeHandler::IsReplaceable(TESObjectANIO *animObject)
        {
            ReadLocker locker(itemLock); 
            return (usedAnimObjects.count(animObject) > 0);
        }
}