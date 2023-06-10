#include "takehandler.h"

namespace AnimatedInteractions
{
    void TakeHandler::StoreReferenceMesh(TESObjectREFR *refr)
        {
            WriteLocker locker(itemLock);
            TakenObjectType = FormTypeToString(refr->GetBaseObject()->GetFormType());
            
            refr->Load3D(false);

            
            ReferenceMesh = refr->Get3D2();//to do: fix missing mesh after getting interrupted by menu open
            // refr->Load3D(true);
        }

        NiNode* TakeHandler::GetAttachNode(NiAVObject* animObjectMesh)
        {
            auto* root = animObjectMesh->AsFadeNode(); 
            auto* userRef = root->GetUserData();            
            NiNode* defaultAttachNode = root->GetObjectByName("Default")->AsNode(); 
           
            if (userRef == nullptr) return defaultAttachNode; 

            SKSE::log::info("attempting to get {} attach node", TakenObjectType);

            auto* attachObj = root->GetObjectByName(TakenObjectType); 

            return (attachObj != nullptr) ? attachObj->AsNode() : defaultAttachNode; 
        }
        

        NiAVObject* TakeHandler::GetMeshForTakenObject(NiAVObject* original)
        {
            WriteLocker locker(itemLock);

            if (ReferenceMesh == nullptr) SKSE::log::info("Null Extract"); 
            
             auto* node = GetAttachNode(original);  
             std::vector<BSGeometry*> geometries = NifUtil::Node::GetAllGeometries(ReferenceMesh); 
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