#pragma once
#include <unordered_set>
#include <shared_mutex>
#include "util.cpp"

using namespace RE;
namespace AnimatedInteractions
{

    struct TakeHandler
    {

    public:
        static inline TESForm *GetLastTakenItem()
        {
            ReadLocker locker(itemLock); 
            return LastTakenItem;
        }

        static void SetLastTakenItem(TESForm *form)
        {
            WriteLocker locker(itemLock); 
            LastTakenItem = form;


        }

        static void BuildAnimObjectPath(TESObjectREFR* refr)
        {
            WriteLocker locker(itemLock);
            RE::TESBoundObject *boundObj = refr->GetObjectReference();
            if (!boundObj) return;
            RE::TESModel *model = boundObj->As<RE::TESModel>();
            if (!model) return;
            std::string modelName = model->GetModel();
            
            NewAnimObject = refr->Get3D2(); 
            

            
            
             
            currentAnimObjectPath = animObjectFolder + modelName; 
            

            
        }

        static NiAVObject* GetMeshForAnimObject(NiAVObject* original)
        {
            WriteLocker locker(itemLock);
            auto* root = original->AsFadeNode(); 

            // root->InsertChildAt(1, node->AsTriShape()) ; s
            // SKSE::log::info("geom name {}",node->GetFirstGeometryOfShaderType(BSShaderMaterial::Feature::kDefault)->name.c_str()); 
            if (NewAnimObject == nullptr) SKSE::log::info("Null Extract"); 
            
            // auto* geom = NewAnimObject->GetFirstGeometryOfShaderType(BSShaderMaterial::Feature::kDefault); 
            // if (geom == nullptr) geom = NewAnimObject->GetFirstGeometryOfShaderType(BSShaderMaterial::Feature::kEnvironmentMap); 
            
            
            // if (geom == nullptr) SKSE::log::info("Null Geom");
            // root->DetachChildAt(0); 
            // root->AttachChild(geom, true);  
             auto* node = root->GetObjectByName("Attach")->AsNode(); 
             std::vector<BSGeometry*> geometries = NifUtil::Node::GetAllGeometries(NewAnimObject); 
             for (auto* geom : geometries)
             {
                node->AttachChild(geom, true); 
             }
            SKSE::log::info("Num children {}", root->GetChildren().size()); 
            // node->AddExtraData(original->GetExtraData(FixedStrings::GetSingleton()->prn)); 
            // SKSE::log::info("node name{}", node->AsTriShape()->name.c_str()); 
            
            return root;
        }

        static bool IsReplaceable(TESObjectANIO *animObject)
        {
            ReadLocker locker(itemLock); 
            return (usedAnimObjects.count(animObject) > 0);
        }

        static std::string GetAnimObjectPath()
        {
            ReadLocker locker(itemLock); 
            return currentAnimObjectPath;  
        }

        static TESObjectANIO *GetLinkedAnimObject(TESForm *form)
        {
            ReadLocker locker(itemLock); 
            if (!form) return nullptr; 
            return animObjMap.contains(form) ? animObjMap[form] : nullptr;
        }
        static TESObjectANIO *GetLinkedAnimObject() { return GetLinkedAnimObject(LastTakenItem); }

        static void Load()
        {
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10F)->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10E)->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10D)->As<TESObjectANIO>());
        }

        static void AddFormMapping(TESForm *form, TESObjectANIO *animObj)
        {
            WriteLocker locker(itemLock); 
            animObjMap.emplace(form, animObj);
        }

    private:
        using Lock = std::shared_mutex;
        using ReadLocker = std::shared_lock<Lock>;
        using WriteLocker = std::unique_lock<Lock>;

        static inline Lock itemLock;

        static inline TESForm *LastTakenItem;
        static inline NiAVObject* NewAnimObject; 

        static inline std::unordered_set<TESObjectANIO *> usedAnimObjects;

        static inline std::unordered_map<RE::TESForm *, TESObjectANIO *> animObjMap;

        static inline std::string animObjectFolder = "animobjects\\Animated_Interactions\\" ; 

        static inline std::string currentAnimObjectPath; 
    };
}