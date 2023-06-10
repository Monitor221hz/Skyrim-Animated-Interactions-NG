#pragma once
#include <unordered_set>
#include <shared_mutex>
#include "util.h"
#include "configmanager.h"
using namespace RE;
namespace AnimatedInteractions
{

    struct TakeHandler
    {

    public:
        static void StoreReferenceMesh(TESObjectREFR *refr); 

        static NiAVObject* GetMeshForTakenObject(NiAVObject* original);

        static bool IsReplaceable(TESObjectANIO *animObject);

        static void Load()
        {
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10F)->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10E)->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Form::GetFormFromMod("Animated Interactions.esp", 0x2E10D)->As<TESObjectANIO>());
        }

        

    private:


        static NiNode* GetAttachNode(NiAVObject* animObjectMesh);
        
        using Lock = std::shared_mutex;
        using ReadLocker = std::shared_lock<Lock>;
        using WriteLocker = std::unique_lock<Lock>;

        static inline Lock itemLock;

        static inline NiAVObject* ReferenceMesh; 
        static inline Config* SelectedConfig; 

        static inline std::string_view TakenObjectType; 

        static inline std::unordered_set<TESObjectANIO *> usedAnimObjects;


    };
}