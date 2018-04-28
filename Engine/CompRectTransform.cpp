#include "CompRectTransform.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "Scene.h"
#include "ModuleInput.h"
#include "WindowSceneWorld.h"
#include "ModuleCamera3D.h"
#include "ModuleFS.h"

#include "ImGui/ImGuizmo.h"

#include <gl/GL.h>
#include <gl/GLU.h>


CompRectTransform::CompRectTransform(Comp_Type t, GameObject * parent) :CompTransform(t, parent), AnimableComponent()
{
	uid = App->random->Int();
	name_component = "CompRectTransform";
	ui_position = float2::zero;
	resize_factor = float2::one;
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
		Resize(resize_factor);
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
		if (ImGui::DragFloat2("##pos", &ui_position[0], drag_speed))
		{
			Resize(resize_factor);
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
		if (open_anchor)
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
		ImGui::Text("Uniform Scale"); ImGui::SameLine(op + 30);

		if (ImGui::Checkbox("##uniform_scale", &unitar_resize))
		{
			Resize(resize_factor);
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

void CompRectTransform::SyncComponent(GameObject * sync_parent)
{
	CompTransform::SyncComponent(sync_parent);

	GameObject* parent_go = parent->GetParent();
	if (parent_go != nullptr)
	{
		CompRectTransform* rect_transf = (CompRectTransform*)parent_go->FindComponentByType(C_RECT_TRANSFORM);
		if (rect_transf != nullptr)
		{
			Resize(rect_transf->GetResizeFactor());
		}
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
	
	float3 save_pos = GetPos();
	save_pos.x = ui_position.x;
	save_pos.y = ui_position.y;
	save_pos.z = 0;

	App->fs->json_array_dotset_float3(object, name + "Position", save_pos);
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
	max_anchor = App->fs->json_array_dotget_float2_string(object, name + "Max Anchor");

	min_anchor = App->fs->json_array_dotget_float2_string(object, name + "Min Anchor");

	right_pivot = App->fs->json_array_dotget_float2_string(object, name + "RPivot");

	left_pivot = App->fs->json_array_dotget_float2_string(object, name + "LPivot");

	pivot = App->fs->json_array_dotget_float2_string(object, name + "Pivot");


	width = json_object_dotget_number_with_std(object, name + "Width");

	height = json_object_dotget_number_with_std(object, name + "Height");

	ignore_z = json_object_dotget_boolean_with_std(object, name + "Ignore Z");

	Init(position, rotation, scale);

	ui_position = float2(position.x, position.y);

	Enable();
}

void CompRectTransform::DrawRectTransform()
{
	float3 south_west = GetSouthWestPosition();
	float3 south_east = GetSouthEastPosition();
	float3 north_west = GetNorthWestPosition();
	float3 north_east = GetNorthEastPosition();

	glBegin(GL_LINES);
	glLineWidth(20.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(position.x + south_west.x, position.y + south_west.y, south_west.z);
	glVertex3f(position.x + south_east.x, position.y + south_east.y, south_east.z);

	glVertex3f(position.x + south_east.x, position.y + south_east.y, south_east.z);
	glVertex3f(position.x + north_east.x, position.y + north_east.y, north_east.z);

	glVertex3f(position.x + north_east.x, position.y + north_east.y, north_east.z);
	glVertex3f(position.x + north_west.x, position.y + north_west.y, north_west.z);

	glVertex3f(position.x + north_west.x, position.y + north_west.y, north_west.z);
	glVertex3f(position.x + south_west.x, position.y + south_west.y, south_west.z);

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void CompRectTransform::Resize(float2 res_factor, bool is_canvas)
{
	float2 pos;

	GameObject* parent_obj = parent->GetParent();
	if (parent_obj != nullptr)
	{
		CompRectTransform* parent_transform = (CompRectTransform*)parent_obj->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
		if (parent_transform != nullptr)
		{
			resize_factor = res_factor;
			if (unitar_resize)
			{
				curr_resize.x = Max(resize_factor.x, resize_factor.y);
				curr_resize.y = curr_resize.x;
			}
			else
			{
				curr_resize = resize_factor;
			}
			int p_width = parent_transform->GetWidth();
			int p_height = parent_transform->GetHeight();
			pos.x = (p_width*(max_anchor.x-pivot.x) + ui_position.x)*res_factor.x;
			pos.y = (p_height*(max_anchor.y-pivot.y) + ui_position.y)*res_factor.y;
			float3 loc_pos = GetPos();
			loc_pos.x = pos.x;
			loc_pos.y = pos.y;
			SetPos(loc_pos);
		}
	}
	else if (is_canvas)
	{
		pos.x = width * 0.5* res_factor.x;
		pos.y = height * 0.5* res_factor.y;
		float3 loc_pos = GetPos();
		loc_pos.x = pos.x;
		loc_pos.y = pos.y;
		SetPos(loc_pos);
	}
}

void CompRectTransform::ResizeRecursive(float2 resize_factor, bool is_canvas)
{
	Resize(resize_factor, is_canvas);

	std::vector<GameObject*>* childs_vec = parent->GetChildsPtr();
	for (uint i = 0; i < childs_vec->size(); i++)
	{
		CompRectTransform* child_rect = (CompRectTransform*)(*childs_vec)[i]->FindComponentByType(C_RECT_TRANSFORM);
		if (child_rect != nullptr)
		{
			child_rect->ResizeRecursive(resize_factor);
		}
	}
}

void CompRectTransform::SetNewAnimationValue(const AnimationData & value)
{
	switch (value.type)
	{
	case ParameterValue::RECT_TRANSFORM_POSITION:
		LOG("%f,%f,%f", value.value.f3_value.x, value.value.f3_value.y, value.value.f3_value.z)
			SetPos(value.value.f3_value);
		break;
	case ParameterValue::RECT_TRANSFORM_ROTATION:
		SetRot(value.value.f3_value);
		break;
	case ParameterValue::RECT_TRANSFORM_SCALE:
		SetScale(value.value.f3_value);
		break;
	case ParameterValue::RECT_TRANSFORM_WIDTH:
		SetWidth(value.value.f_value);
		break;
	case ParameterValue::RECT_TRANSFORM_HEIGHT:
		SetHeight(value.value.f_value);
		break;
	default:
		break;
	}
}

AnimationData CompRectTransform::ShowParameters()
{
	ImGui::OpenPopup("OptionsCollider");
	AnimationData ret;
	ret.type = ParameterValue::PARAMETER_NONE;
	SetNextWindowSize(ImVec2(200, 200));
	if (ImGui::BeginPopup("OptionsCollider"))
	{
		ImGui::Columns(2, "parameters");
		ImGui::Text("Parameter type");
		ImGui::NextColumn();
		ImGui::Text("Select");
		ImGui::NextColumn();
		ImGui::Separator();

		ImGui::Text("Position");
		ImGui::NextColumn();
		if (ImGui::Button("Select##pos_button"))
		{
			ret.type = ParameterValue::RECT_TRANSFORM_POSITION;
			ret.value.f3_value = GetPos();
		}
		ImGui::NextColumn();

		ImGui::Text("Rotation");
		ImGui::NextColumn();
		if (ImGui::Button("Select##rot_button"))
		{
			ret.type = ParameterValue::RECT_TRANSFORM_ROTATION;
			ret.value.f3_value = GetRotEuler();
		}
		ImGui::NextColumn();

		ImGui::Text("Scale");
		ImGui::NextColumn();
		if (ImGui::Button("Select##sca_button"))
		{
			ret.type = ParameterValue::RECT_TRANSFORM_SCALE;
			ret.value.f3_value = GetScale();
		}
		ImGui::NextColumn();

		ImGui::Text("Width");
		ImGui::NextColumn();
		if (ImGui::Button("Select##width_button"))
		{
			ret.type = ParameterValue::RECT_TRANSFORM_WIDTH;
			ret.value.f_value = GetWidth();
		}
		ImGui::NextColumn();

		ImGui::Text("Height");
		ImGui::NextColumn();
		if (ImGui::Button("Select##height_button"))
		{
			ret.type = ParameterValue::RECT_TRANSFORM_HEIGHT;
			ret.value.f_value = GetHeight();
		}
		ImGui::NextColumn();
		ImGui::Columns(1);

		ImGui::EndPopup();
	}
	return ret;
}

const char * CompRectTransform::ReturnParameterName(ParameterValue parameter)
{
	switch (parameter)
	{
	case ParameterValue::RECT_TRANSFORM_POSITION:
		return "Position";
		break;
	case ParameterValue::RECT_TRANSFORM_ROTATION:
		return "Rotation";
		break;
	case ParameterValue::RECT_TRANSFORM_SCALE:
		return "Scale";
		break;
	case ParameterValue::RECT_TRANSFORM_WIDTH:
		return "Width";
		break;
	case ParameterValue::RECT_TRANSFORM_HEIGHT:
		return "Height";
		break;
	default:
		break;
	}
	return nullptr;
}

void CompRectTransform::SetWidth(int set_width)
{
	update_rect = true;
	width = set_width;
	Resize(resize_factor);
}

void CompRectTransform::SetHeight(int set_height)
{
	update_rect = true;
	height = set_height;
	Resize(resize_factor);
}

void CompRectTransform::SetUIPos(float3 new_pos)
{
	if (new_pos.IsFinite())
	{
		ui_position.x = new_pos.x;
		ui_position.y = new_pos.y;
		Resize(resize_factor);
	}
}

void CompRectTransform::SetMaxAnchor(float2 set_max_anchor)
{
	max_anchor = set_max_anchor;
	Resize(resize_factor);
}

void CompRectTransform::SetMinAnchor(float2 set_min_anchor)
{
	min_anchor = set_min_anchor;
	Resize(resize_factor);
}

void CompRectTransform::SetPivot(float2 set_pivot)
{
	left_pivot = pivot;
	right_pivot = float2(1.0f - pivot.x, 1.0f - pivot.y);
	pivot = set_pivot;
	Resize(resize_factor);
}



void CompRectTransform::SetUpdateRect(bool set_update_rect)
{
	update_rect = set_update_rect;
}

float2 CompRectTransform::GenerateResizeFactor(int width, int height)
{

	resize_factor.x = (float)width / (float)this->width;
	resize_factor.y = (float)height / (float)this->height;

	if (unitar_resize)
	{
		curr_resize.x = Max(resize_factor.x, resize_factor.y);
		curr_resize.y = curr_resize.x;
	}
	else
	{
		curr_resize = resize_factor;
	}
	return resize_factor;

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

AnimationValue CompRectTransform::GetParameter(ParameterValue parameter)
{
	AnimationValue ret;
	switch (parameter)
	{
	case PARAMETER_NONE:
		break;
	case RECT_TRANSFORM_POSITION:
		ret.f3_value = GetPos();
		break;
	case RECT_TRANSFORM_ROTATION:
		ret.f3_value = GetRotEuler();
		break;
	case RECT_TRANSFORM_SCALE:
		ret.f3_value = GetScale();
		break;
	case RECT_TRANSFORM_WIDTH:
		ret.f_value = GetWidth();
		break;
	case RECT_TRANSFORM_HEIGHT:
		ret.f_value = GetHeight();
		break;
	default:
		break;
	}
	return ret;
}

int CompRectTransform::GetWidth()const
{
	return width;
}

int CompRectTransform::GetHeight()const
{
	return height;
}

float2 CompRectTransform::GetResizeFactor() const
{
	return resize_factor;
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

float2 CompRectTransform::GetUIPosition() const
{
	return ui_position;
}

bool CompRectTransform::GetUpdateRect() const
{
	return update_rect;
}

float4 CompRectTransform::GetRect()const
{
	return float4(-width * left_pivot.x, -height * left_pivot.y, width*right_pivot.x, height*right_pivot.y);
}
float4 CompRectTransform::GetGlobalRect()const
{
	return float4(position_global.x - (abs(width*resize_factor.x)*left_pivot.x), position_global.y + (abs(height*resize_factor.y)*left_pivot.y), (float)abs(width*resize_factor.x), (float)abs(height*resize_factor.y));
}
float3 CompRectTransform::GetGlobalPosition()const
{
	return float3(position_global.x - (abs(width)*left_pivot.x), position_global.y + (abs(height)*left_pivot.y), position_global.z);
}
float3 CompRectTransform::GetNorthEastPosition()const
{
	return float3(width*right_pivot.x, height * left_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetNorthWestPosition()const
{
	return float3(-width * left_pivot.x, height * left_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetSouthEastPosition()const
{
	return float3(width*right_pivot.x, -height * right_pivot.y, GetPos().z);
}

float3 CompRectTransform::GetSouthWestPosition()const
{
	return float3(-width * left_pivot.x, -height * right_pivot.y, GetPos().z);
}
