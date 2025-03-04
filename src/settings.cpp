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
		bool BlocksActivation = false;
		bool UseLegacyTake = false; 
        double AnimationSpeed = 1.0;
		double HighThreshold = -35.0;
		double LowThreshold = -115.0;

    };

	struct Rotation 
	{
		double YawSpeed = 10.0;
		double SpinePitchSpeed = 10.0; 

		double SpinePitchMinDegrees = 10.0; 
		double SpinePitchMaxDegrees = 10.0; 
	};

	struct Camera
	{
		bool ForceThirdPerson = false; 
		

	}; 

	void LoadSettings()
	{
		constexpr auto path = L"Data/SKSE/Plugins/Animated_Interactions/Settings.ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		
		ini.LoadFile(path);
		animation.UseLegacyTake = ini.GetBoolValue("Animation", "LegacyTakeAnimations", false); 
		animation.BlocksActivation = ini.GetBoolValue("Animation", "AnimationBlocksActivate", false);
		animation.AnimationSpeed= ini.GetDoubleValue("Animation", "AnimationSpeedMultiplier", 1.0);
		animation.HighThreshold = ini.GetDoubleValue("Animation", "TakeHighThreshold", 50.0);
		animation.LowThreshold = ini.GetDoubleValue("Animation", "TakeLowThreshold", -50.0);
		camera.ForceThirdPerson = ini.GetBoolValue("Camera", "ForceThirdPerson", false); 
		rotation.YawSpeed = ini.GetDoubleValue("Rotation", "YawSpeedMultiplier", 10.0f);
		rotation.SpinePitchSpeed = ini.GetDoubleValue("Rotation", "SpinePitchSpeedMultiplier", 10.0f);
		rotation.SpinePitchMinDegrees = ini.GetDoubleValue("Rotation", "SpinePitchMinDegrees", -30.0f); 
		rotation.SpinePitchMaxDegrees = ini.GetDoubleValue("Rotation", "SpinePitchMaxDegrees", 30.0f); 
	
	}
	[[nodiscard]] bool GetLegacyTakeAnimation() const { return animation.UseLegacyTake; }
	[[nodiscard]] bool GetAnimationBlockActivation() const { return animation.BlocksActivation; }	
    [[nodiscard]] double GetAnimationSpeed() const { return animation.AnimationSpeed; }
	[[nodiscard]] double GetHighTakeBound() const { return animation.HighThreshold; }
	[[nodiscard]] double GetLowTakeBound() const { return animation.LowThreshold; }
	[[nodiscard]] bool GetForceThirdPerson() const { return camera.ForceThirdPerson; }
	[[nodiscard]] double GetYawSpeed() const { return rotation.YawSpeed; }
	[[nodiscard]] double GetSpinePitchSpeed() const { return rotation.SpinePitchSpeed; }
	[[nodiscard]] double GetSpinePitchMin() const { return rotation.SpinePitchMinDegrees; }
	[[nodiscard]] double GetSpinePitchMax() const { return rotation.SpinePitchMaxDegrees; }

	// [[nodiscard]] int GetCrosshairMode() const { return crosshair.mode; }



private:

    Animation animation;
	Camera camera; 
	Rotation rotation;

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
