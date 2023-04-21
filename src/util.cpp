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
namespace NifUtil
{
    struct Node
		{
			static RE::NiAVObject* GetNiObject(
				RE::NiNode*              a_root,
				const RE::BSFixedString& a_name)
			{
				return a_root->GetObjectByName(a_name);
			}

			static void AttachToNode(
				RE::NiAVObject* a_object,
				RE::NiNode*     a_node)
			{
				if (a_object->parent != a_node)
				{
					a_node->AttachChild(a_object, true);
				}
			}

		};
    struct Armature
    {
        static RE::NiNode* GetActorNode(RE::Actor* actor, std::string nodeName)
        {
                auto root = actor->Get3D();
                if (!root) return nullptr;

                auto bone = root->GetObjectByName(nodeName);
                if (!bone) return nullptr;

                auto node = bone->AsNode();
                if (!node) return nullptr;

                return node;
        }

        static void AttachToNode(RE::NiAVObject* obj, RE::Actor* actor, std::string nodeName)
        {
            auto* node = GetActorNode(actor, nodeName);
            if (node)
            {
                node->AttachChild(obj, true);
                SKSE::log::info("Object Attached");
            }
        }
    };
    struct Collision
    {
        static bool ToggleMeshCollision(RE::NiAVObject* root,RE::bhkWorld* world, bool collisionState)
        {
            constexpr auto no_collision_flag = static_cast<std::uint32_t>(RE::CFilter::Flag::kNoCollision);
					if (root && world) {
						
							RE::BSWriteLockGuard locker(world->worldLock);

							RE::BSVisit::TraverseScenegraphCollision(root, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
								if (auto hkpBody = a_col->body ? static_cast<RE::hkpWorldObject*>(a_col->body->referencedObject.get()) : nullptr; hkpBody) {
									auto& filter = hkpBody->collidable.broadPhaseHandle.collisionFilterInfo;
									if (!collisionState) {
										filter |= no_collision_flag;
									} else {
										filter &= ~no_collision_flag;
									}
								}
								return RE::BSVisit::BSVisitControl::kContinue;
							});
					}
                    else 
                    {
                        return false;
                    }
            return true;
        }
    };

   
}

