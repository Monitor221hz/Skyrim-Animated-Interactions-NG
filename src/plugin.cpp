#include "log.h"
#include "hook.h"
#include "anim.h"
#include "event.h"
#include "takedata.h"
void OnDataLoaded()
{
   
}
using namespace AnimatedInteractions; 

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
	    AnimatedInteractions::EventHandler::Install();
		Settings::GetSingleton()->LoadSettings();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		TakeHandler::Load(); 
		AnimatedInteractions::AnimPlayer::GetSingleton()->GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
        break;
	case SKSE::MessagingInterface::kNewGame:
		TakeHandler::Load(); 
		AnimatedInteractions::AnimPlayer::GetSingleton()->GetIdleRecords();
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