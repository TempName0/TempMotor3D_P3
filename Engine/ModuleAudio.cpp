#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "wwished.h"
#include "ModuleFS.h"

#include <locale>
#include <codecvt>

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	have_config = true;
	volume = 10;
	name = "Audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(JSON_Object* node)
{
	perf_timer.Start();
	bool ret = true;
	
	LOG("Loading Wwished library");

	//Convert the char path to wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	banks_full_path = App->fs->GetFullPath("AudioBanks");
	std::wstring base_path = converter.from_bytes(banks_full_path.c_str());
	init_bank_not_loaded = !Wwished::InitWwished(base_path.c_str(), "English(US)");

	if (init_bank_not_loaded)
		LOG("CAUTION: Init sound bank not loaded");

	volume = json_object_get_number(node, "Volume");
	mute = json_object_get_boolean(node, "Mute");

	Awake_t = perf_timer.ReadMs();
	return ret;
}

//bool ModuleWindow::Start()
//{
//	perf_timer.Start();
//
//	Start_t = perf_timer.ReadMs();
//	return true;
//}
//
//update_status ModuleWindow::PreUpdate(float dt)
//{
//	perf_timer.Start();
//
//	preUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::Update(float dt)
//{
//	perf_timer.Start();
//
//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::postUpdate(float dt)
//{
//	perf_timer.Start();
//
//	postUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status ModuleAudio::PostUpdate(float dt)
{
	Wwished::ProcessAudio();

	return UPDATE_CONTINUE;
}

bool ModuleAudio::SaveConfig(JSON_Object * node)
{
	//Save audio config info --------------------------------
	json_object_set_number(node, "Volume", volume);
	json_object_set_boolean(node, "Mute", mute);
	// ------------------------------------------------------
	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	UnloadAllBanks();
	LOG("Unloading Wwished library");
	return Wwished::CloseWwished();
}

void ModuleAudio::DrawOnEditor()
{
	if (init_bank_not_loaded)
	{
		ImGui::TextColored(ImVec4(255, 0, 0, 255), "Init audiobank is not loaded!");
		return;
	}

	if (loaded_banks.empty())
	{
		ImGui::Text("No audiobanks loaded");
	}
	else
	{
		int index = 0;
		for (std::vector<std::string>::iterator it = loaded_banks.begin(); it != loaded_banks.end();)
		{

			ImGui::Text((*it).c_str());
			ImGui::SameLine();
			ImGui::PushID(index);
			
			if (ImGui::Button("Unload"))
			{
				Wwished::Utility::UnLoadBank((*it).c_str());
				it = loaded_banks.erase(it);
			}
			else 
			{
				it++;
				index++;
			}
			ImGui::PopID();
		}


	}
	ImGui::Separator();
	if (ImGui::Button("Load Audiobank"))
	{
		load_bank_window_opened = true;
	}

	if (load_bank_window_opened)
	{
		ImGui::OpenPopup("Load Audio Bank");
		ImGui::SetNextWindowSize(ImVec2(200, 500));
		ImGui::SetNextWindowPosCenter();
	
		if (ImGui::BeginPopupModal("Load Audio Bank", nullptr))
		{
			ImGui::BeginChildFrame(0, ImVec2(150, 420));
			
			for (std::experimental::filesystem::recursive_directory_iterator::value_type it : std::experimental::filesystem::recursive_directory_iterator(banks_full_path.c_str()))
			{
				std::string extension_file = std::experimental::filesystem::path(it.path().string().c_str()).extension().string().c_str();

				if (extension_file.compare(".bnk") != 0)
					continue;

				std::string bank_name = std::experimental::filesystem::path(it.path().string().c_str()).stem().string().c_str();
				if (bank_name.compare("Init") == 0)
					continue;
				
				if (ImGui::Selectable(bank_name.c_str()))
				{
					LoadBank(bank_name.c_str());		
					load_bank_window_opened = false;
					break;
				}
			}

			ImGui::EndChildFrame();

			if (ImGui::Button("Cancel"))
			{
				load_bank_window_opened = false;
			}
		}
		ImGui::EndPopup();		
	}
}

void ModuleAudio::UnloadAllBanks()
{
	LOG("Unloading all audio banks");
	for (std::vector<std::string>::iterator it = loaded_banks.begin(); it != loaded_banks.end(); it++)
	{
		Wwished::Utility::UnLoadBank((*it).c_str());
	}
	loaded_banks.clear();
}



update_status ModuleAudio::UpdateConfig(float dt)
{
	if (ImGui::SliderInt("Volume", &volume, 0, 100))
	{
		if (mute == false)
		{
			//change volume
		}
	}

	if (ImGui::Checkbox("Mute", &mute))
	{
		//mute
	}
	return UPDATE_CONTINUE;
}

void ModuleAudio::SaveAudioBanks(JSON_Object * config_node)
{

	json_object_dotset_number_with_std(config_node, "Scene.Info.Number of Audio Banks", loaded_banks.size());

	std::string audio_banks = "AudioBanks.";

	int i = 0;
	for (std::vector<std::string>::iterator it = loaded_banks.begin(); it != loaded_banks.end(); it++, i++)
	{		
		json_object_dotset_string_with_std(config_node, audio_banks + "Bank " + std::to_string(i).c_str(), (*it).c_str());
	}
}

void ModuleAudio::LoadAudioBanksFromScene(int number_of_banks, JSON_Object * config_node)
{
	UnloadAllBanks();
	for (int i = 0; i < number_of_banks; i++)
	{
		std::string bank_number = "AudioBanks.Bank ";
		bank_number += std::to_string(i).c_str();
		std::string bank_name = json_object_dotget_string_with_std(config_node, bank_number.c_str());
		LoadBank(bank_name.c_str());		
	}
}

int ModuleAudio::LoadBank(const char * bank_name)
{
	int result = Wwished::Utility::LoadBank(bank_name);
	if (result == 1)
	{
		loaded_banks.push_back(bank_name);
		LOG("AudioBank %s Loaded", bank_name)
	}
	else if (result == 0)
	{
		LOG("WARNING: AudioBank %s not found", bank_name)
	}
	else if (result == 2)
	{
		LOG("WARNING: AudioBank %s already loaded", bank_name)
	}


	return result;
}

