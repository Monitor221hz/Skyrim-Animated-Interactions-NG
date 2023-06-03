#pragma once
#include <unordered_set>
#include <shared_mutex>
#include "util.cpp"

using namespace RE;
namespace AnimatedInteractions
{

    struct TakeData
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
            RE::TESBoundObject *boundObj = refr->GetObjectReference();
            if (!boundObj) return;
            RE::TESModel *model = boundObj->As<RE::TESModel>();
            if (!model) return;
            std::string modelName = model->GetModel();
            currentAnimObjectPath = animObjectFolder + modelName; 
        }

        static bool IsReplaceable(TESObjectANIO *animObject)
        {
            ReadLocker locker(itemLock); 
            return (usedAnimObjects.count(animObject) > 0);
        }

        static std::string GetAnimObjectPath()
        {
            ReadLocker(itemLock); 
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

        static inline std::unordered_set<TESObjectANIO *> usedAnimObjects;

        static inline std::unordered_map<RE::TESForm *, TESObjectANIO *> animObjMap;

        static inline std::string animObjectFolder = "animobjects\\Animated_Interactions\\" ; 

        static inline std::string currentAnimObjectPath; 
    };
}