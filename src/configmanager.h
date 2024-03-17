#pragma once 
#include "util.h"
#include "SimpleIni.h"



namespace AnimatedInteractions
{

    struct Config
    {
        public:  
        NiTransform transform;
        

        inline void BuildFromString(std::string raw)
        {
            std::vector<std::string> AlignParts = Util::String::Split(raw, "|"sv);
            transform = NiTransform();

            std::vector<float> transformValues = Util::String::ToFloatVector(Util::String::Split(AlignParts[0], ","sv));
            transform.translate.x = transformValues[0];
            transform.translate.y = transformValues[1];
            transform.translate.z = transformValues[2];

            if (AlignParts.size() == 1)
                return;

            std::vector<float> rotateValues = Util::String::ToFloatVector(Util::String::Split(AlignParts[1], ","sv));
            RE::NiPoint3 rotationVector = MathUtil::Angle::ToRadianVector(rotateValues[0], rotateValues[1], rotateValues[2]);
            transform.rotate.SetEulerAnglesXYZ(rotationVector);

            if (AlignParts.size() == 2)
                return;

            float scale = atof(AlignParts[2].c_str());
            transform.scale = scale;
        }

        private: 
        
    };
    class ConfigManager
    {
        public: 


        static Config* GetMeshConfigPtr(std::string mesh_path);
        static Config* GetFormConfigPtr(FormType form_type);

        

        static void LoadMasterConfig();
        static void LoadUserConfigs();

        static void LoadAllConfigs()
        {
            LoadMasterConfig(); 
            LoadUserConfigs();
        }


        private: 

        static inline std::unordered_map<std::string, Config> mesh_configs; 

        static inline std::unordered_map<std::string, Config> form_configs;
        
    };
}