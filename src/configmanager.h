#pragma once 
#include "util.h"
#include "SimpleIni.h"



namespace AnimatedInteractions
{

    struct Config
    {

        

        public:  
        NiTransform transform;

        Config(std::string raw)
        {
            std::vector<std::string> AlignParts = Util::String::Split(raw, "|"sv);
            transform = NiTransform(); 

            std::vector<float> rotateValues = Util::String::ToFloatVector(Util::String::Split(AlignParts[1], ","sv) );
            std::vector<float> transformValues = Util::String::ToFloatVector(Util::String::Split(AlignParts[0], ","sv) );
            
            RE::NiPoint3 rotationVector= MathUtil::Angle::ToRadianVector(rotateValues[0], rotateValues[1], rotateValues[2]);

            transform.rotate.SetEulerAnglesXYZ(rotationVector); 

            transform.translate.x = transformValues[0]; 
            transform.translate.y = transformValues[1]; 
            transform.translate.z = transformValues[2];

        }
        private: 
        
    };
    class ConfigManager
    {
        public: 
        static bool GetConfig(std::string meshPath, Config &outConfig) 
        {
            if (pathConfigs.count(meshPath) == 0) return false; 

            outConfig = pathConfigs[meshPath]; 
            return true; 
        }

        static void LoadConfigs()
        {
            std::vector<std::string> files = SystemUtil::File::GetConfigs(R"(Data\)", "_AINTR"sv);
            for(auto path : files)
            {
                CSimpleIniA ini;
                ini.SetUnicode();
			    ini.SetMultiKey();
                if (const auto rc = ini.LoadFile(path.c_str()); rc < 0) {
				SKSE::log::error("\t\tcouldn't read INI");
				continue;
                }

                if (auto values = ini.GetSection(""); values && !values->empty()) {
                    for(auto& [key, entry] : *values)
                    {
                        try 
                        {

                        }
                        catch (...)
                        {
                            SKSE::log::warn("Failed to parse entry [{} = {}]", key.pItem, entry);
                        }
                    }
                }
			}
            
            
        }


        private: 

        static inline std::unordered_map<std::string, Config> pathConfigs; 


        
    };
}