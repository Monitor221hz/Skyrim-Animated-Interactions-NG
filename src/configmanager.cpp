#include "configmanager.h"
namespace AnimatedInteractions
{
    Config* ConfigManager::GetMeshConfigPtr(std::string meshPath)
    {
        meshPath = Util::String::ToLower(meshPath);
        return meshConfigs.count(meshPath) > 0 ? &meshConfigs[meshPath] : nullptr;
    }

    Config* ConfigManager::GetFormConfigPtr(FormType formType)
    {
        std::string formTypeName = std::string(FormTypeToString(formType));
        return formConfigs.count(formTypeName) > 0 ? &formConfigs[formTypeName] : nullptr;
    }

    void ConfigManager::LoadMasterConfig()
    {
        formConfigs.clear();
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
                        formConfigs.emplace(Util::String::ToUpper(key.pItem),config);
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
        meshConfigs.clear();
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
                        meshConfigs.emplace(Util::String::ToLower(key.pItem),config);
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