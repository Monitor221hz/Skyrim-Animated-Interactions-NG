#include "log.h"
#include "hook.h"
#include "anim.h"
#include "event.h"
void OnDataLoaded()
{
   
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
	    
		InteractiveIdles::AnimPlayer::GetSingleton()->GetIdleRecords();
		Settings::GetSingleton()->LoadSettings();
		
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
        break;
	case SKSE::MessagingInterface::kNewGame:
		
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
	InteractiveIdles::Hook::InstallHooks();
	InteractiveIdles::MenuHandler::AddHook();
	
    
    return true;
}