#include "CompRectTransform.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "ModuleInput.h"
#include "WindowSceneWorld.h"
#include "ModuleCamera3D.h"
#include "ModuleFS.h"

#include "ImGui/ImGuizmo.h"

#include <gl/GL.h>
#include <gl/GLU.h>


CompRectTransform::CompRectTransform(Comp_Type t, GameObject * parent) :CompTransform(t, parent)
{
	uid = App->random->Int();
	name_component = "Rect Transform";
	max_anchor = float2(0.5f, 0.5f);
	min_anchor = float2(0.5f, 0.5f);
	pivot = float2(0.5f, 0.5f);
	SetPivot(pivot);
	width = 50;
	height = 50;
}

CompRectTransform::CompRectTransform(const CompRectTransform & copy, GameObject * parent) : CompTransform(Comp_Type::C_RECT_TRANSFORM, parent)
{
}

CompRectTransform::~CompRectTransform()
{
}
void CompRectTransform::Update(float dt)
{
	// This code is made by disabling gizmos when editing transforms with editor
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		editing_transform = false;
	}
	if (App->mode_game == false)
	{
		// Show gizmo when object selected
		if (((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected() == parent && App->engine_state == EngineState::STOP)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::Enable(true);

			static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
			screen = ((SceneWorld*)App->gui->win_manager[SCENEWORLD])->GetWindowParams();
			ImGuizmo::SetRect(screen.x, screen.y, screen.z, screen.w);

			// Get global transform of the object and transpose it to edit with Guizmo
			global_transposed = global_transform.Transposed();

			if (io.WantTextInput == false)
			{
				// SET GUIZMO OPERATION ----------------------------------
				if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				}
				else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::ROTATE;
				}
				else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::SCALE;
				}
			}

			// EDIT TRANSFORM QITH GUIZMO
			ImGuizmo::Manipulate(App->camera->GetViewMatrix(), App->camera->GetProjMatrix(), mCurrentGizmoOperation, transform_mode, global_transposed.ptr());

			// Only edit transforms with guizmo if it's selected first
			if (ImGuizmo::IsUsing() && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !editing_transform && !freeze)
			{
				global_transposed.Transpose();

				//If it's a root node, global and local transforms are the same
				if (parent->GetParent() == nullptr)
				{
					local_transform = global_transposed;
				}
				else
				{
					//Otherwise, set local matrix from parent global matrix (inverted)
					const CompTransform* transform = parent->GetParent()->GetComponentTransform();
					local_transform = transform->GetGlobalTransform().Inverted() * global_transposed;
				}

				local_transform.Decompose(position, rotation, scale);
				rotation_euler = rotation.ToEulerXYZ() * RADTODEG;
				toUpdate = true;
			}
		}
	}
	if (toUpdate)
	{
		UpdateMatrix(transform_mode);
		toUpdate = false;
		updated = true;
	}
	else
	{
		updated = false;
	}
}

void CompRectTransform::ShowOptions()
{
	if (ImGui::MenuItem("Reset"))
	{
		ResetMatrix();
		ImGui::CloseCurrentPopup();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component", NULL, false))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompRectTransform*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Position"))
	{
		SetPos(float3::zero);
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::MenuItem("Reset Rotation"))
	{
		SetRot(math::Quat::identity);
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::MenuItem("Reset Scale"))
	{
		SetScale(math::float3(1.0f, 1.0f, 1.0f));
		ImGui::CloseCurrentPopup();
	}
}

void CompRectTransform::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsRectTransform");
	}
	ImGui::PopStyleVar();

	// Options Button --------------------------------------------------
	if (ImGui::BeginPopup("OptionsRectTransform"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	// SET TRANSFORM MODE -------------------------------------------------------
	if (ImGui::RadioButton("Local", transform_mode == ImGuizmo::LOCAL))
	{
		transform_mode = ImGuizmo::LOCAL;
	}
	//ImGui::SameLine();
	//if (ImGui::RadioButton("World", transform_mode == ImGuizmo::WORLD))
	//{
	//	transform_mode = ImGuizmo::WORLD;
	//}

	// SHOW OUTPUT (depending on mode and if transform is freezed) ---------------
	if (App->engine_state == EngineState::STOP)
	{
		ShowTransform(0.5f); // 0.5f = drag speed of editor variables of the transform
	}
	else
	{
		// In game mode, you can't edit transforms of static objects
		if (freeze)
		{
			ShowTransform(0.0f);  // 0.0f = no editable
		}
		else
		{
			ShowTransform(0.5f);
		}
	}
	// ------------------------------------------------------------------

	ImGui::TreePop();
}


void CompRectTransform::ShowTransform(float drag_speed)
{
	int op = ImGui::GetWindowWidth() / 4;

	// Set color of the disabled text when transform is freezed
	if (drag_speed == 0)
	{
		PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
	}

	// Depending transform mode, edit local/world matrix (LOCAL for now)
	switch (transform_mode)
	{
	case (ImGuizmo::MODE::LOCAL):
	{
		ImGui::Text("Position"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##pos", &position[0], drag_speed))
		{
			SetPos(position);
			editing_transform = true;
		}
		ImGui::Text("Rotation"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##rot", &rotation_euler[0], drag_speed))
		{
			SetRot(rotation_euler);
			editing_transform = true;
		}
		ImGui::Text("Scale"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##scale", &scale[0], drag_speed))
		{
			SetScale(scale);
			editing_transform = true;
		}

		ImGui::Text("Width"); ImGui::SameLine(op + 30);
		if (ImGui::DragInt("##width", &width, drag_speed))
		{
			SetWidth(width);
		}
		ImGui::Text("Height"); ImGui::SameLine(op + 30);
		if (ImGui::DragInt("##height", &height, drag_speed))
		{
			SetHeight(height);
		}

		bool open_anchor = ImGui::TreeNodeEx("Anchor", ImGuiTreeNodeFlags_DefaultOpen);
		if(open_anchor)
		{
			ImGui::Text("Max X,Y"); ImGui::SameLine(op + 30);
			if (ImGui::DragFloat2("##max_anchor", &max_anchor[0], 0.1f, 0.0f, 1.0f))
			{
				SetMaxAnchor(max_anchor);
			}

			ImGui::Text("Min X,Y"); ImGui::SameLine(op + 30);
			if (ImGui::DragFloat2("##min_anchor", &min_anchor[0], 0.1f, 0.0f, 1.0f))
			{
				SetMinAnchor(min_anchor);
			}
			ImGui::TreePop();
		}

		ImGui::Text("Pivot X,Y"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat2("##min_pivot", &pivot[0], 0.1f, 0.0f, 1.0f))
		{
			SetPivot(pivot);
		}
		break;
	}
	default:
		break;
	}

	if (drag_speed == 0)
	{
		ImGui::PopStyleColor();
	}
}

void CompRectTransform::CopyValues(const CompRectTransform* component)
{
	//more...
}

void CompRectTransform::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	// TRANSFORM -----------

	App->fs->json_array_dotset_float3(object, name + "Position", GetPos());
	// Rotation
	App->fs->json_array_dotset_float4(object, name + "Rotation", float4(GetRot().x, GetRot().y, GetRot().z, GetRot().w));
	// Scale
	App->fs->json_array_dotset_float3(object, name + "Scale", GetScale());

	//...
	App->fs->json_array_dotset_float2(object, name + "Max Anchor", max_anchor);

	App->fs->json_array_dotset_float2(object, name + "Min Anchor", min_anchor);

	App->fs->json_array_dotset_float2(object, name + "RPivot", right_pivot);

	App->fs->json_array_dotset_float2(object, name + "LPivot", left_pivot);

	App->fs->json_array_dotset_float2(object, name + "Pivot", pivot);


	json_object_dotset_number_with_std(object, name + "Width", width);

	json_object_dotset_number_with_std(object, name + "Height", height);

	json_object_dotset_boolean_with_std(object, name + "Ignore Z", ignore_z);

}

void CompRectTransform::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	float3 position = App->fs->json_array_dotget_float3_string(object, name + "Position");
	float4 rotation = App->fs->json_array_dotget_float4_string(object, name + "Rotation");
	float3 scale = App->fs->json_array_dotget_float3_string(object, name + "Scale");
	max_anchor=App->fs->json_array_dotget_float2_string(object, name + "Max Anchor" );

	min_anchor=App->fs->json_array_dotget_float2_string(object, name + "Min Anchor" );

	right_pivot=App->fs->json_array_dotget_float2_string(object, name + "RPivot" );

	left_pivot=App->fs->json_array_dotget_float2_string(object, name + "LPivot" );

	pivot=App->fs->json_array_dotget_float2_string(object, name + "Pivot" );


	width=json_object_dotget_number_with_std(object, name + "Width" );

	height=json_object_dotget_number_with_std(object, name + "Height" );

	ignore_z=json_object_dotget_boolean_with_std(object, name + "Ignore Z" );

	Init(position, rotation, scale);

	Enable();
}

void CompRectTransform::SetWidth(int set_width)
{
	update_rect = true;
	width = set_width;
}

void CompRectTransform::SetHeight(int set_height)
{
	update_rect = true;
	height = set_height;
}

void CompRectTransform::SetMaxAnchor(float2 set_max_anchor)
{
	max_anchor = set_max_anchor;
}

void CompRectTransform::SetMinAnchor(float2 set_min_anchor)
{
	min_anchor = set_min_anchor;
}

void CompRectTransform::SetPivot(float2 set_pivot)
{
	left_pivot = pivot;
	right_pivot = float2(1.0f - pivot.x, 1.0f - pivot.y);
	pivot = set_pivot;
}



void CompRectTransform::SetUpdateRect(bool set_update_rect)
{
	update_rect = set_update_rect;
}

std::vector<float3> CompRectTransform::GenerateQuadVertices()
{
	std::vector<float3> ret;
	ret.push_back(GetSouthWestPosition());
	ret.push_back(GetSouthEastPosition());
	ret.push_back(GetNorthEastPosition());
	ret.push_back(GetNorthWestPosition());
	return ret;
}

int CompRectTransform::GetWidth()const
{
	return height;
}

int CompRectTransform::GetHeight()const
{
	return height;
}

float2 CompRectTransform::GetMaxAnchor()const
{
	return max_anchor;
}

float2 CompRectTransform::GetMinAnchor()const
{
	return min_anchor;
}

float2 CompRectTransform::GetPivot()const
{
	return pivot;
}

bool CompRectTransform::GetUpdateRect() const
{
	return update_rect;
}

float4 CompRectTransform::GetRect()const
{
	return float4(-width*left_pivot.x, -height*left_pivot.y,width*right_pivot.x, height*right_pivot.y);
}
float4 CompRectTransform::GetGlobalRect()const
{
	return float4(global_transform.TranslatePart().x-( width*left_pivot.x), global_transform.TranslatePart().y + (height*left_pivot.y), (float)width, (float)height);
}
float3 CompRectTransform::GetNorthEastPosition()const
{
	return float3(width*right_pivot.x, height * left_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetNorthWestPosition()const
{
	return float3(- width*left_pivot.x, height * left_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetSouthEastPosition()const
{
	return float3(width*right_pivot.x, - height * right_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetSouthWestPosition()const
{
	return float3(- width*left_pivot.x,- height * right_pivot.y, GetPos().z);
}
