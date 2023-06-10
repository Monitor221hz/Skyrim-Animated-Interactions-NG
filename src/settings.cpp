#pragma once

#include "SimpleIni.h"
#include "takehandler.h"
#include "util.h"
using namespace RE; 
namespace AnimatedInteractions
{
class Settings
{
public:

    [[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	struct Animation
    {
        double AnimationSpeed = 1.0;
		double HighThreshold = -35.0;
		double LowThreshold = -115.0;

    };

	void LoadSettings()
	{
		constexpr auto path = L"Data/SKSE/Plugins/Animated Interactions.ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		
		ini.LoadFile(path);
		animation.AnimationSpeed= ini.GetDoubleValue("Animation", "AnimationSpeedMultiplier", 1.0);
		animation.HighThreshold = ini.GetDoubleValue("Animation", "TakeHighThreshold", 50.0);
		animation.LowThreshold = ini.GetDoubleValue("Animation", "TakeLowThreshold", -50.0);

        SKSE::log::info("Settings loaded. Animation speed {} ", animation.AnimationSpeed);
		std::list<CSimpleIniA::Entry> Sections;
		ini.GetAllSections(Sections);

		

		

		
		// weather.SnowVisionAngleDegrees = ini.GetLongValue("Weather", "SnowVisionAngleDegrees");
		// weather.RainSoundLevelMultiplier = ini.GetLongValue("Weather", "RainSoundLevelMultiplier");
		
		// crosshair.mode = ini.GetLongValue("Crosshair", "Mode");
		// crosshair.min = ini.GetDoubleValue("Crosshair", "OpacityMin", 0.0);
		// crosshair.max = ini.GetDoubleValue("Crosshair", "OpacityMax", 100.0);

		// sneakMeter.mode = ini.GetLongValue("SneakMeter", "Mode");
		// sneakMeter.min = ini.GetDoubleValue("SneakMeter", "OpacityMin", 0.0);
		// sneakMeter.max = ini.GetDoubleValue("SneakMeter", "OpacityMax", 90.0);
		// sneakMeter.range = ini.GetDoubleValue("SneakMeter", "PulseRange", 0.05);
		// sneakMeter.frequency = ini.GetDoubleValue("SneakMeter", "PulseFrequency", 0.05);

		// spells.mode = ini.GetLongValue("Spells", "Mode");

		// fade.time = ini.GetDoubleValue("Fade", "Multiplier", 1.0);

		// smoothCam.mode = ini.GetLongValue("SmoothCam", "Mode");
	
	}
    [[nodiscard]] double GetAnimationSpeed() const { return animation.AnimationSpeed; }
	[[nodiscard]] double GetHighTakeBound() const { return animation.HighThreshold; }
	[[nodiscard]] double GetLowTakeBound() const { return animation.LowThreshold; }
	// [[nodiscard]] int GetCrosshairMode() const { return crosshair.mode; }



private:

    Animation animation;

	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;

	// Crosshair  crosshair;
	// SneakMeter sneakMeter;
	// Spells     spells;
	// Fade       fade;
	// SmoothCam  smoothCam;
	

};
}
