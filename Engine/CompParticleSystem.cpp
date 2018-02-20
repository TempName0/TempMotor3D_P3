#include "CompParticleSystem.h"
#include "Application.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "ResourceMaterial.h"
#include "parson.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "GameObject.h"

CompParticleSystem::CompParticleSystem(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	name_component = "Particle_System";
}

CompParticleSystem::CompParticleSystem(const CompParticleSystem& copy, GameObject* parent) : Component(copy.GetType(), parent)
{

}

CompParticleSystem::~CompParticleSystem()
{

}

void CompParticleSystem::Init()
{

}

void CompParticleSystem::PreUpdate(float dt)
{
	const CompCamera* camera = App->renderer3D->GetActiveCamera();
	if (camera != nullptr)
		part_system->SetCameraPosToFollow(camera->GetGameObjectPos());

	part_system->PreUpdate(dt);
}

void CompParticleSystem::Update(float dt)
{
	if (pop_up_load_open) ImGuiLoadPopUp();
	if (pop_up_save_open) ImGuiSavePopUp();
}

void CompParticleSystem::Draw()
{

}

void CompParticleSystem::Clear()
{

}

// LOAD / SAVE ---------------------------------------------------------

void CompParticleSystem::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{

}

void CompParticleSystem::Load(const JSON_Object* object, std::string name)
{

}

bool CompParticleSystem::SaveParticleStates(const ResourceMaterial* TextureResource, const ParticleTextureData* TexData, const ParticleState* stateI, const ParticleState* stateF) const
{
	/*JSON_Object* file_conf = nullptr;

	//If this entry does not exist, create it
	if (json_object_get_object(root_object, "particlestate") == NULL)
		json_object_set_value(root_object, "particlestate", json_value_init_object());
	//Save
	file_conf = json_object_get_object(root_object, "particlestate");

	const ParticleState* State = nullptr;
	JSON_Object* conf = nullptr;

	if (TextureResource != nullptr)
	{
		json_object_set_value(file_conf, "texture", json_value_init_object());
		conf = json_object_get_object(file_conf, "texture");
		SetString(conf, "texture_path", TextureResource->GetOriginalFile().c_str());
		SetInt(conf, "columns", TexData->columns);
		SetInt(conf, "rows", TexData->rows);
		SetInt(conf, "numberOfFrames", TexData->numberOfFrames);
		SetUInt(conf, "AnimationOrder", TexData->AnimationOrder);
	}

	for (uint i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			State = stateI;
			json_object_set_value(file_conf, "initial_state", json_value_init_object());
			conf = json_object_get_object(file_conf, "initial_state");
		}
		else
		{
			State = stateF;
			json_object_set_value(file_conf, "final_state", json_value_init_object());
			conf = json_object_get_object(file_conf, "final_state");
		}
		SetFloat3(conf, "force", State->force);
		SetFloat3(conf, "forceVariation", State->forceVariation);
		SetFloat(conf, "Size", State->Size);
		SetFloat(conf, "SizeVariation", State->SizeVariation);
		SetFloat4(conf, "RGBATint", State->RGBATint);
		SetFloat4(conf, "RGBATintVariation", State->RGBATintVariation);
		SetBool(conf, "alpha_preview", State->alpha_preview);
		SetBool(conf, "alpha_half_preview", State->alpha_half_preview);
		SetBool(conf, "options_menu", State->options_menu);
		SetBool(conf, "alpha", State->alpha);
		SetBool(conf, "alpha_bar", State->alpha_bar);
		SetBool(conf, "side_preview", State->side_preview);
		SetUInt(conf, "inputs_mode", State->inputs_mode);
		SetUInt(conf, "picker_mode", State->picker_mode);
	}

	json_serialize_to_file(root_value, file_name.c_str());*/
	return true;
}
/*
bool CompParticleSystem::LoadParticleStates(CompParticleSystem* system, ParticleState& stateI, ParticleState& stateF) const
{
	JSON_Object* file_conf = nullptr;
	file_conf = json_object_get_object(root_object, "particlestate");

	ParticleState* State = nullptr;
	JSON_Object* conf = nullptr;

	conf = json_object_get_object(file_conf, "texture");
	if (conf != nullptr)
	{
		const char* Path = App->parsonjson->GetString(conf, "texture_path");
		int columns = App->parsonjson->GetInt(conf, "columns");
		int rows = App->parsonjson->GetInt(conf, "rows");
		int numberOfFrames = App->parsonjson->GetInt(conf, "numberOfFrames");
		uint AnimationOrder = App->parsonjson->GetUInt(conf, "AnimationOrder");
		system->SetTextureResource(Path, columns, rows, numberOfFrames, AnimationOrder);
	}

	for (uint i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			State = &stateI;
			conf = json_object_get_object(file_conf, "initial_state");
		}
		else
		{
			State = &stateF;
			conf = json_object_get_object(file_conf, "final_state");
		}

		State->force = GetFloat3(conf, "force");
		State->forceVariation = GetFloat3(conf, "forceVariation");
		State->Size = GetFloat(conf, "Size");
		State->SizeVariation = GetFloat(conf, "SizeVariation");
		State->RGBATint = GetFloat4(conf, "RGBATint");
		State->RGBATintVariation = GetFloat4(conf, "RGBATintVariation");
		State->alpha_preview = GetBool(conf, "alpha_preview");
		State->alpha_half_preview = GetBool(conf, "alpha_half_preview");
		State->options_menu = GetBool(conf, "options_menu");
		State->alpha = GetBool(conf, "alpha");
		State->alpha_bar = GetBool(conf, "alpha_bar");
		State->side_preview = GetBool(conf, "side_preview");
		State->inputs_mode = GetUInt(conf, "inputs_mode");
		State->picker_mode = GetUInt(conf, "picker_mode");
	}

	return true;
}

bool CompParticleSystem::SaveParticleEmitter(CompParticleSystem* system, const ParticleEmitter* emitter) const
{
	JSON_Object* conf = nullptr;

	//If this entry does not exist, create it
	if (json_object_get_object(root_object, "emitter") == NULL)
		json_object_set_value(root_object, "emitter", json_value_init_object());
	//Save
	conf = json_object_get_object(root_object, "emitter");

	bool ShowEmitterBoundBox = false;
	bool ShowEmitter = false;
	system->GetDebugOptions(ShowEmitterBoundBox, ShowEmitter);
	SetBool(conf, "ShowEmitterBoundBox", ShowEmitterBoundBox);
	SetBool(conf, "ShowEmitter", ShowEmitter);

	if (system->GetChildParticle() != nullptr)
		SetString(conf, "ChildParticle", system->GetChildParticle()->c_str());
	if (system->GetChildEmitter() != nullptr)
		SetString(conf, "ChildEmitter", system->GetChildEmitter()->c_str());

	SetFloat(conf, "EmitterLifeMax", emitter->EmitterLifeMax);
	SetFloat4x4(conf, "Transform", emitter->Transform);
	SetUInt(conf, "SpawnRate", emitter->SpawnRate);
	SetFloat(conf, "Lifetime", emitter->Lifetime);
	SetFloat(conf, "LifetimeVariation", emitter->LifetimeVariation);
	SetFloat(conf, "EmissionDuration", emitter->EmissionDuration);
	SetBool(conf, "Loop", emitter->Loop);
	SetFloat(conf, "Speed", emitter->Speed);
	SetFloat(conf, "SpeedVariation", emitter->SpeedVariation);
	SetFloat3(conf, "BoundingBox_min", emitter->BoundingBox.minPoint);
	SetFloat3(conf, "BoundingBox_max", emitter->BoundingBox.maxPoint);
	//SetUInt(conf, "EmissionType", emitter->EmissionType);
	SetUInt(conf, "Type", emitter->Type);
	SetUInt(conf, "ParticleFacingOptions", emitter->ParticleFacingOptions);

	switch (emitter->Type)
	{
	case 0: //EmitterType_Sphere
	case 1: //EmitterType_SemiSphere
		SetFloat(conf, "Radius", emitter->EmitterShape.Sphere_Shape.r);
		break;
	case 2: //EmitterType_Cone
		SetFloat(conf, "URadius", emitter->EmitterShape.ConeTrunk_Shape.Upper_Circle.r);
		SetFloat(conf, "BRadius", emitter->EmitterShape.ConeTrunk_Shape.Bottom_Circle.r);
		SetFloat(conf, "heigth", emitter->EmitterShape.ConeTrunk_Shape.heigth);
		break;
	case 3: //EmitterType_Box
		SetFloat3(conf, "EmitterAABB_min", emitter->EmitterShape.Box_Shape.minPoint);
		SetFloat3(conf, "EmitterAABB_max", emitter->EmitterShape.Box_Shape.maxPoint);
		break;
	case 4: //EmitterType_Circle
		SetFloat(conf, "Radius", emitter->EmitterShape.Circle_Shape.r);
		break;
	}

	json_serialize_to_file(root_value, file_name.c_str());
	return true;
}

bool CompParticleSystem::LoadParticleEmitter(CompParticleSystem* system, ParticleEmitter& emitter) const
{
	JSON_Object* conf = nullptr;
	conf = json_object_get_object(root_object, "emitter");

	bool ShowEmitterBoundBox = GetBool(conf, "ShowEmitterBoundBox");
	bool ShowEmitter = GetBool(conf, "ShowEmitter");
	system->SetDebugOptions(ShowEmitterBoundBox, ShowEmitter);

	const char* ChildParticle = GetString(conf, "ChildParticle");
	const char* ChildEmitter = GetString(conf, "ChildEmitter");
	if ((ChildParticle != nullptr) && (ChildEmitter != nullptr))
		system->SetChild(ChildParticle, ChildEmitter);

	emitter.EmitterLifeMax = GetFloat(conf, "EmitterLifeMax");
	emitter.Transform = GetFloat4x4(conf, "Transform");
	emitter.SpawnRate = GetUInt(conf, "SpawnRate");
	emitter.Lifetime = GetFloat(conf, "Lifetime");
	emitter.LifetimeVariation = GetFloat(conf, "LifetimeVariation");
	emitter.EmissionDuration = GetFloat(conf, "EmissionDuration");
	emitter.Loop = GetBool(conf, "Loop");
	emitter.Speed = GetFloat(conf, "Speed");
	emitter.SpeedVariation = GetFloat(conf, "SpeedVariation");
	emitter.BoundingBox.minPoint = GetFloat3(conf, "BoundingBox_min");
	emitter.BoundingBox.maxPoint = GetFloat3(conf, "BoundingBox_max");
	//emitter.EmissionType = (ParticleEmitter::TypeEmission)GetUInt(conf, "EmissionType");
	emitter.Type = (ParticleEmitter::TypeEmitter)GetUInt(conf, "Type");
	emitter.ParticleFacingOptions = (ParticleEmitter::TypeBillboard)GetUInt(conf, "ParticleFacingOptions");

	switch (emitter.Type)
	{
	case 0: //EmitterType_Sphere
	case 1: //EmitterType_SemiSphere
		emitter.EmitterShape.Sphere_Shape.r = GetFloat(conf, "Radius");
		break;
	case 2: //EmitterType_Cone
		emitter.EmitterShape.ConeTrunk_Shape.Upper_Circle.r = GetFloat(conf, "URadius");
		emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r = GetFloat(conf, "BRadius");
		emitter.EmitterShape.ConeTrunk_Shape.heigth = GetFloat(conf, "heigth");
		break;
	case 3: //EmitterType_Box
		emitter.EmitterShape.Box_Shape.minPoint = GetFloat3(conf, "EmitterAABB_min");
		emitter.EmitterShape.Box_Shape.maxPoint = GetFloat3(conf, "EmitterAABB_max");
		break;
	case 4: //EmitterType_Circle
		emitter.EmitterShape.Circle_Shape.r = GetFloat(conf, "Radius");
		break;
	}

	return true;
}*/

//-----------------------------------------------------------
void CompParticleSystem::ShowOptions()
{

}

void CompParticleSystem::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsParticleSystem");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsParticleSystem"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}



// PopUps ---------------------------------------------------
void CompParticleSystem::ImGuiLoadPopUp()
{
	/*const char* Str = "Wrong Type";
	switch (file_type)
	{
	case Texture_Resource: Str = "Load Texture"; break;
	case Particle_Resource: Str = "Load Particle"; break;
	case Emitter_Resource: Str = "Load Emitter"; break;
	case Child_Particle_Resource: Str = "Load Child Particle"; break;
	case Child_Emitter_Resource: Str = "Load Child Emitter"; break;
	case MeshResource: Str = "Load Mesh"; break;
	}

	ImGui::OpenPopup(Str);
	if (ImGui::BeginPopupModal(Str, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (file_type == MeshResource)
		{
			ImGui::Text("You are in mesh folder of library. Here you\ncan search for files containing only one mesh.\nThis meshes are stored with the name of the\nnode containing them in the original 3d model\nfile you store in assets folder.");
			if (ImGui::Button("Load Plane", ImVec2(50, 20)))
				part_system->SetMeshResourcePlane();
		}
		else ImGui::Text("Here are only shown files that are accepted\nextention files.");

		ImGui::BeginChild("File Browser##1", ImVec2(300, 300), true);
		switch (file_type)
		{
		case Texture_Resource: DrawDirectory(App->importer->Get_Assets_path()->c_str()); break;
		case Particle_Resource: case Child_Particle_Resource: DrawDirectory(App->importer->Get_ParticleSystem_Particles_path()->c_str()); break;
		case Emitter_Resource: case Child_Emitter_Resource: DrawDirectory(App->importer->Get_ParticleSystem_Emitter_path()->c_str()); break;
		case MeshResource: DrawDirectory(App->importer->Get_Library_mesh_path()->c_str()); break;
		}
		ImGui::EndChild();
		char file_path[1000] = "";
		sprintf_s(file_path, 1000, "%s", file_to_load.c_str());
		ImGui::InputText("##input text 1", file_path, 1000, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("Ok##1", ImVec2(50, 20)))
		{
			if (!file_to_load.empty())
			{
				switch (file_type)
				{
				case Texture_Resource: ImGuiLoadTexturePopUp(); break;
				case Particle_Resource: ImGuiLoadParticlePopUp(); break;
				case Emitter_Resource: ImGuiLoadEmitterPopUp(); break;
				case Child_Particle_Resource: child_particle = file_to_load; break;
				case Child_Emitter_Resource: child_emitter = file_to_load; break;
				case MeshResource: ImGuiLoadMeshPopUp(); break;
				}
			}
			pop_up_load_open = false;
			file_to_load.clear();
			file_to_load_name.clear();
			directory_temporal_str.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##1", ImVec2(50, 20)))
		{
			pop_up_load_open = false;
			file_to_load.clear();
			file_to_load_name.clear();
			directory_temporal_str.clear();
		}
		ImGui::EndPopup();
	}*/
}

void CompParticleSystem::ImGuiLoadTexturePopUp()
{
	/*size_t bar_pos = file_to_load.rfind("\\") + 1;
	size_t dot_pos = file_to_load.rfind(".");
	file_to_load_name = file_to_load.substr(bar_pos, dot_pos - bar_pos);
	uint Texuuid = App->resources->LoadResource((*App->importer->Get_Library_material_path() + "\\" + file_to_loadName + ".dds").c_str(), file_to_load.c_str());
	SetTextureResource(Texuuid);*/
}

void CompParticleSystem::ImGuiLoadParticlePopUp()
{
	/*ParticleState InitialState;
	ParticleState FinalState;

	size_t dot_pos = file_to_load.rfind(".");
	file_to_load_name = file_to_load.substr(0, dot_pos);

	ParsonJSON* parsonjson = new ParsonJSON(file_to_loadName.c_str(), true, false, false);
	bool Loaded = parsonjson->Init();
	if (Loaded) parsonjson->LoadParticleStates(this, InitialState, FinalState);
	RELEASE(parsonjson);

	part_system->SetInitialStateResource(InitialState);
	part_system->SetFinalStateResource(FinalState);*/
}

void CompParticleSystem::ImGuiLoadEmitterPopUp()
{
	/*ParticleEmitter Emitter;

	size_t dot_pos = file_to_load.rfind(".");
	file_to_loadName = file_to_load.substr(0, dot_pos);

	ParsonJSON* parsonjson = new ParsonJSON(file_to_loadName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->LoadParticleEmitter(this, Emitter);
	RELEASE(parsonjson);

	part_system->SetEmitterResource(Emitter);*/
}

void CompParticleSystem::ImGuiLoadMeshPopUp()
{
	//uint Meshuuid = App->resources->LoadResource(file_to_load.c_str(), file_to_load.c_str());
	//SetMeshResource(Meshuuid);
}

void CompParticleSystem::ImGuiSavePopUp()
{
	/*ImGui::OpenPopup("Save File##1");
	if (ImGui::BeginPopupModal("Save File##1", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
	{
		const char* Str = "Wrong Type";
		switch (file_type)
		{
		case Particle_Resource: Str = "Save Particle"; break;
		case Emitter_Resource: Str = "Save Emitter"; break;
		}
		static char file_name[500] = "";
		ImGui::InputText(Str, file_name, 500);
		if (ImGui::Button("Ok", ImVec2(50, 20)))
		{
			if (strcmp(file_name, ""))
			{
				FileToSave = file_name;
				switch (file_type)
				{
				case Particle_Resource: ImGuiSaveParticlePopUp(); break;
				case Emitter_Resource: ImGuiSaveEmitterPopUp(); break;
				}
			}
			PopUpSaveOpen = false;
			file_to_load.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			PopUpSaveOpen = false;
		}
		ImGui::EndPopup();
	}*/
}

void CompParticleSystem::ImGuiSaveParticlePopUp()
{
	/*ParticleState InitialState;
	part_system->GetInitialState(InitialState);
	ParticleState FinalState;
	part_system->GetFinalState(FinalState);

	FileToSaveName = *App->importer->Get_ParticleSystem_Particles_path() + "\\" + FileToSave;

	ParsonJSON* parsonjson = new ParsonJSON(FileToSaveName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->SaveParticleStates(TextureResource, part_system->GetTextureResource(), &InitialState, &FinalState);

	RELEASE(parsonjson);*/
}

void CompParticleSystem::ImGuiSaveEmitterPopUp()
{
	/*ParticleEmitter Emitter;
	part_system->GetEmitter(Emitter);

	FileToSaveName = *App->importer->Get_ParticleSystem_Emitter_path() + "\\" + FileToSave;

	ParsonJSON* parsonjson = new ParsonJSON(FileToSaveName.c_str(), true, false, false);
	bool Meta = parsonjson->Init();
	if (Meta) parsonjson->SaveParticleEmitter(this, &Emitter);
	RELEASE(parsonjson);*/
}