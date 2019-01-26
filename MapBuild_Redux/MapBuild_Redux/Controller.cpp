#include "Controller.h"

Level* Controller::CurrentLevel = NULL;
bool Controller::bLoading = true;
Keyboard Controller::m_Keyboard = {};

void Controller::SwitchLevel(Level* const level)
{
	bLoading = true;
	if (CurrentLevel)
	{
		CurrentLevel->Unload();
		delete CurrentLevel;
	}
	CurrentLevel = level;
	CurrentLevel->Load(&m_Keyboard);
	bLoading = false;
}

void Controller::Render()
{
	if (bLoading) return;
	if (CurrentLevel) CurrentLevel->Render();
}

void Controller::Update(double dDelta)
{
	if (!CurrentLevel) return;
	CurrentLevel->ProcessEvents(dDelta);
	CurrentLevel->Update(dDelta);
}

void Controller::Cleanup()
{
	if (CurrentLevel) CurrentLevel->Unload();
	delete CurrentLevel;
	CurrentLevel = nullptr;
}