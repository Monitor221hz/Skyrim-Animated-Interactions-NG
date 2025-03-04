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
            usedAnimObjects.emplace(FormUtil::Parse::GetFormFromMod(0x808,"Animated Interactions.esp")->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Parse::GetFormFromMod(0x809,"Animated Interactions.esp")->As<TESObjectANIO>());
            usedAnimObjects.emplace(FormUtil::Parse::GetFormFromMod(0x80A,"Animated Interactions.esp")->As<TESObjectANIO>());
        }

        static void HandlePickUp(RE::TESObjectREFR *ref);
    private:


        static NiNode* GetAttachNode(NiAVObject* animObjectMesh);

        

        static int32_t ConstructNiStream(char* address, void* memory, void* num); 
        static void DestructNiStream(NiStream* niStream);
        using Lock = std::shared_mutex;
        using ReadLocker = std::shared_lock<Lock>;
        using WriteLocker = std::unique_lock<Lock>;

        static inline Lock itemLock;

        static inline NiAVObject* ReferenceMesh; 
        static inline TESObjectREFR* Reference; 
        static inline Config* SelectedConfig; 

        static inline std::string_view TakenObjectType; 

        static inline std::unordered_set<TESObjectANIO *> usedAnimObjects;


    };
}