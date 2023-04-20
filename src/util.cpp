#pragma once

namespace PerkUtil {
    struct EntryVisitor : public RE::PerkEntryVisitor {
    public:
        explicit EntryVisitor(RE::Actor* a_actor) {
            actor_ = a_actor;
            result_ = 0;
        }

        ReturnType Visit(RE::BGSPerkEntry* perk_entry) override {
            const auto* entry_point = static_cast<RE::BGSEntryPointPerkEntry*>(perk_entry);
            const auto* perk = entry_point->perk;
            
            if (entry_point->functionData &&
                entry_point->entryData.function == RE::BGSEntryPointPerkEntry::EntryData::Function::kMultiplyValue) {
                const RE::BGSEntryPointFunctionDataOneValue* value =
                    static_cast<RE::BGSEntryPointFunctionDataOneValue*>(entry_point->functionData);
                result_ = value->data;
            }

            return ReturnType::kContinue;
        }

        [[nodiscard]] float get_result() const { return result_; }

    private:
        RE::Actor* actor_;
        float result_;
    };
}

namespace AnimUtil
{
    struct Idle
    {
        public:
            static bool Play(RE::TESIdleForm* idle, RE::Actor* actor, RE::DEFAULT_OBJECT action, RE::Actor* target)
            {
            if (actor && actor->GetActorRuntimeData().currentProcess)
            {
                typedef bool (*func_t)(RE::AIProcess *, RE::Actor *, RE::DEFAULT_OBJECT, RE::TESIdleForm *, bool, bool, RE::Actor *);
                REL::Relocation<func_t> func{RELOCATION_ID(38290, 39256)};
                return func(actor->GetActorRuntimeData().currentProcess, actor, action, idle, true, true, target);
            }
            return false;
            }
    };
}

namespace FormUtil
{
    struct Form
    {
        public:
            static RE::TESForm *GetFormFromMod(std::string modname, uint32_t formid)
            {
            if (!modname.length() || !formid)
                return nullptr;
            RE::TESDataHandler *dh = RE::TESDataHandler::GetSingleton();
            RE::TESFile *modFile = nullptr;
            for (auto it = dh->files.begin(); it != dh->files.end(); ++it)
            {
                RE::TESFile *f = *it;
                if (strcmp(f->fileName, modname.c_str()) == 0)
                {
                    modFile = f;
                    break;
                }
            }
            if (!modFile)
                return nullptr;
            uint8_t modIndex = modFile->compileIndex;
            uint32_t id = formid;
            if (modIndex < 0xFE)
            {
                id |= ((uint32_t)modIndex) << 24;
            }
            else
            {
                uint16_t lightModIndex = modFile->smallFileCompileIndex;
                if (lightModIndex != 0xFFFF)
                {
                    id |= 0xFE000000 | (uint32_t(lightModIndex) << 12);
                }
            }
            return RE::TESForm::LookupByID(id);
            }
    };
}

