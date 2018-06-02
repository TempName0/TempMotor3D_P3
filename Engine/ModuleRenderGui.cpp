#include "Application.h"
#include "ModuleRenderGui.h"
#include "EventDef.h"
#include "CompCanvas.h"
#include "ModuleEventSystemV2.h"
#include "ModuleWindow.h"
#include "SDL/include/SDL_opengl.h"
#include "GL3W/include/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ModuleShaders.h"
#include "Scene.h"
#include "ModuleFramebuffers.h"
#include "ShadersLib.h"
#include "CompInteractive.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "DefaultShaders.h"
#include "GameObject.h"
ModuleRenderGui::ModuleRenderGui(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	Update_enabled = true;
	postUpdate_enabled = true;
	have_config = true;
	name = "Render Gui";
	selected = nullptr;

}

ModuleRenderGui::~ModuleRenderGui()
{
}

bool ModuleRenderGui::Init(JSON_Object * node)
{
	window_width=App->window->GetWidth();
	window_height=App->window->GetHeight();
	return true;
}

bool ModuleRenderGui::Start()
{


	return true;
}

update_status ModuleRenderGui::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate: ModuleRenderGui", Profiler::Color::Blue);
	perf_timer.Start();

	screen_space_canvas.clear();


	if (last_size_dock.x != GetSizeDock("Scene").x || last_size_dock.y != GetSizeDock("Scene").y)
	{
		App->scene->scene_buff->WantRefreshRatio();
	}

	last_size_dock = GetSizeDock("Scene");
	iteractive_vector.clear();
	
	
	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderGui::Update(float dt)
{
	perf_timer.Start();


	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderGui::PostUpdate(float dt)
{
	perf_timer.Start();
	
	//if (!world_space_canvas.empty())
	//	WorldSpaceDraw();
	//if (!screen_space_canvas.empty())
	//	ScreenSpaceDraw();
	
	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleRenderGui::UpdateConfig(float dt)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
	ImGui::Checkbox("Debug Draw", &debug_draw);
	
	ImGui::PopStyleVar();
	return UPDATE_CONTINUE;
}

bool ModuleRenderGui::SetEventListenrs()
{
	AddListener(EventType::EVENT_DRAW, this);
	AddListener(EventType::EVENT_BUTTON_DOWN, this);
	AddListener(EventType::EVENT_MOUSE_MOTION, this);
	AddListener(EventType::EVENT_BUTTON_UP, this);
	AddListener(EventType::EVENT_PASS_COMPONENT, this);
	AddListener(EventType::EVENT_PASS_SELECTED, this);
	AddListener(EventType::EVENT_AXIS, this);
	AddListener(EventType::EVENT_SUBMIT, this);
	AddListener(EventType::EVENT_CANCEL, this);

	return true;
}

void ModuleRenderGui::PassSelected(CompInteractive * to_pass)
{
	Event pass_selected;
	pass_selected.Set_event_data(EventType::EVENT_PASS_SELECTED);
	pass_selected.pass_selected.component = to_pass;
	ChangeSelected(pass_selected);
}

void ModuleRenderGui::ChangeSelected(Event & this_event)
{
	if (selected != nullptr)
	{
		selected->ForceClear();
	}
	selected = this_event.pass_selected.component;
	if (selected != nullptr)
	{
		if (selected->OnList())
		{
			selected->OnInteractiveSelected(this_event);

		}
		else
		{
			selected = nullptr;
		}
	}
}


void ModuleRenderGui::OnEvent(Event & this_event)
{
	BROFILER_CATEGORY("OnEvent: ModuleRenderGui", Profiler::Color::Blue);


	switch (this_event.Get_event_data_type())
	{
	case EventType::EVENT_PASS_SELECTED:
		ChangeSelected(this_event);
		break;
	case EventType::EVENT_PASS_COMPONENT:
		iteractive_vector.push_back((CompInteractive*)this_event.pass_component.component);
		break;
	case EventType::EVENT_AXIS:
		
		if (selected != nullptr)
		{
			if(selected->GetParent()->IsActive())
				selected->OnMove(this_event);
		}
	
		break;
	case EventType::EVENT_SUBMIT:
		if (selected != nullptr)
		{
			if (selected->GetParent()->IsActive())
				selected->OnSubmit(this_event);
		}
		break;
	case EventType::EVENT_CANCEL:
		if (selected != nullptr)
		{
			if (selected->GetParent()->IsActive())
				selected->OnCancel(this_event);
		}
		break;
	case EventType::EVENT_BUTTON_DOWN:
	case EventType::EVENT_BUTTON_UP:
	case EventType::EVENT_MOUSE_MOTION:
		if (focus != nullptr)
			this_event.pointer.focus = focus->GetParent();
		if (App->engine_state != EngineState::STOP)
		{
			
				bool positive_colision = false;
				std::vector<CompInteractive*>::reverse_iterator it = iteractive_vector.rbegin();
				for (; it != iteractive_vector.rend(); ++it)
				{
					if (!(*it)->IsInteractiveEnabled())
					{
						if (!(*it)->IsActivate()) //if it's active 
						{
							(*it)->ForceClear();
						}
						continue;
					}
					
					if ((*it)->PointerInside(this_event.pointer.position))
					{
						
						if (positive_colision)
						{
							(*it)->ForceClear();
							continue;
						}
						switch (this_event.Get_event_data_type())
						{
						case EventType::EVENT_BUTTON_DOWN:
							if ((*it)->GetNavigationMode() != Navigation::NavigationMode::NAVIGATION_NONE)
							{
								PassSelected((*it));
								(*it)->OnInteractiveSelected(this_event);
							}
							(*it)->OnPointDown(this_event);
						
							focus = (*it);
							mouse_down = true;
							break;
						case EventType::EVENT_BUTTON_UP:
							(*it)->OnPointUP(this_event);
							focus = nullptr;
							mouse_down = false;
							break;
						case EventType::EVENT_MOUSE_MOTION:
							
							if ((*it)->IsDragrable())
							{
								(*it)->OnDrag(this_event);
							}
							else
							{
								if (selected != nullptr)
								{
									selected->ForceClear();
									selected = nullptr;
								}
							}
							(*it)->OnPointEnter(this_event);
							break;
						}
						positive_colision = true;
					}
					else
					{
						switch (this_event.Get_event_data_type())
						{
						case EventType::EVENT_BUTTON_UP:
							(*it)->OnPointUP(this_event);
							focus = nullptr;
							mouse_down = false;
							break;
						case EventType::EVENT_MOUSE_MOTION:
							if ((*it)->IsDragrable())
							{
								(*it)->OnDrag(this_event);
							}
							(*it)->OnPointExit(this_event);
							break;
						}
					}
				}
				if (!positive_colision&&!mouse_down && selected != nullptr)
				{
					focus = nullptr;
					if (this_event.Get_event_data_type() == EventType::EVENT_BUTTON_DOWN)
					{
						selected->ForceClear();
						selected = nullptr;
					}
				}
						
		}
		break;
	default:
		break;
	}
	//this_event.draw.ToDraw->Draw();
}

void ModuleRenderGui::OnResize(float width, float height)
{
	for (int i = 0; i < screen_space_canvas.size(); i++)
	{
		screen_space_canvas[i]->Resize(width,height);
	}
}

void ModuleRenderGui::WorldSpaceDraw()
{
}

void ModuleRenderGui::ScreenSpaceDraw(bool debug)
{
	
	//Draw
	for (int i = 0; i < screen_space_canvas.size(); i++)
	{
		screen_space_canvas[i]->DrawDebugRectTransform();
	}	
	for (int i = 0; i < screen_space_canvas.size(); i++)
	{
		screen_space_canvas[i]->DrawGraphic(debug_draw);
	}	
}
void ModuleRenderGui::ClearInteractive()
{
	if (focus != nullptr)
	{
		focus->ForceClear();
		focus = nullptr;
	}
	if (selected != nullptr)
	{
		selected->ForceClear();
		selected = nullptr;
	}
	ClearInteractiveVector();
}
void ModuleRenderGui::ClearInteractiveVector()
{
	iteractive_vector.clear();
}


bool ModuleRenderGui::CleanUp()
{

	return true;
}

