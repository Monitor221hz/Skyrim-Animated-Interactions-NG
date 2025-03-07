#include "log.h"
#include "hook.h"
#include "anim.h"
#include "event.h"
#include "takehandler.h"
#include "configmanager.h"
void OnDataLoaded()
{
   
}
using namespace AnimatedInteractions; 

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		AnimatedInteractions::MenuOpenCloseEventHandler::Register(); 
		AnimatedInteractions::PlayerUpdateHook::Load();
		AnimPlayer::GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
		ConfigManager::LoadAllConfigs();
		TakeHandler::Load(); 
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		TakeHandler::Load(); 
		// AnimPlayer::GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
		ConfigManager::LoadAllConfigs();
		AnimatedInteractions::PlayerActivateHook::Reset(); 
        break;
	case SKSE::MessagingInterface::kNewGame:
		TakeHandler::Load(); 
		// AnimPlayer::GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
		AnimatedInteractions::PlayerActivateHook::Reset(); 
		break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	AnimatedInteractions::AnimObjectHook::Install();
	AnimatedInteractions::InputHook::Install();
	AnimatedInteractions::PlayerActivateHook::Install();
	AnimatedInteractions::PlayerGraphEventHook::Install();
	
	// AnimatedInteractions::PlayerPickUpHook::Install();
	AnimatedInteractions::PlayerUpdateHook::Install();
    
    return true;
}