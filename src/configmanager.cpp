#include "configmanager.h"
namespace AnimatedInteractions
{
    Config* ConfigManager::GetMeshConfigPtr(std::string meshPath)
    {
        meshPath = Util::String::ToLower(meshPath);
        return mesh_configs.count(meshPath) > 0 ? &mesh_configs[meshPath] : nullptr;
    }

    Config* ConfigManager::GetFormConfigPtr(FormType formType)
    {
        std::string formTypeName = std::string(FormTypeToString(formType));
        return form_configs.count(formTypeName) > 0 ? &form_configs[formTypeName] : nullptr;
    }

    void ConfigManager::LoadMasterConfig()
    {
        form_configs.clear();
        constexpr auto path = L"Data/SKSE/Plugins/Animated_Interactions/Alignment.ini";

        CSimpleIniA ini; 
        ini.SetUnicode();
        if (const auto rc = ini.LoadFile(path); rc < 0)
        {
            SKSE::log::error("\t\tcouldn't read Alignment.ini");
        }
        if (auto values = ini.GetSection("DefaultTransform"); values && !values->empty())
        {
            for (auto &[key, entry] : *values)
                {
                    try
                    {
                        Config config; 
                        config.BuildFromString(entry);
                        form_configs.emplace(Util::String::ToUpper(key.pItem),config);
                        SKSE::log::info("Alignment data found for: ({})", key.pItem);
                    }
                    catch (...)
                    {
                        SKSE::log::warn("Failed to parse default entry [{} = {}]", key.pItem, entry);
                    }
                }
        }


    }
    void ConfigManager::LoadUserConfigs()
    {
        std::vector<std::string> files = SystemUtil::File::GetConfigs(R"(Data\)", "_AINTR"sv);
        mesh_configs.clear();
        for (auto path : files)
        {
            CSimpleIniA ini;
            ini.SetUnicode();

            if (const auto rc = ini.LoadFile(path.c_str()); rc < 0)
            {
                SKSE::log::error("\t\tcouldn't read INI {}", path);
                continue;
            }
            SKSE::log::info("reading config {}", path);

            if (auto values = ini.GetSection(""); values && !values->empty())
            {
                for (auto &[key, entry] : *values)
                {
                    try
                    {
                        Config config; 
                        config.BuildFromString(entry);
                        mesh_configs.emplace(Util::String::ToLower(key.pItem),config);
                    }
                    catch (...)
                    {
                        SKSE::log::warn("Failed to parse entry [{} = {}]", key.pItem, entry);
                    }
                }
            }
        }


    }
}