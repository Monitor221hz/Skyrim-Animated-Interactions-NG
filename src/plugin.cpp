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
	    EventHandler::Install();
		Settings::GetSingleton()->LoadSettings();
		ConfigManager::LoadAllConfigs();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		TakeHandler::Load(); 
		AnimPlayer::GetSingleton()->GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
		ConfigManager::LoadAllConfigs();
        break;
	case SKSE::MessagingInterface::kNewGame:
		TakeHandler::Load(); 
		AnimPlayer::GetSingleton()->GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
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
	AnimatedInteractions::Hook::InstallHooks();
	
	
    
    return true;
}