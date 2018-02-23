#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib.h"
#include <string>
#include <vector>

#define WIDTH_ADDCOMPONENT 204

class CompTransform;
class CompMesh;
class CompMaterial;
class CompScript;
class CompAnimation;
class CompRectTransform;
class CompImage;
class CompText;
class CompEditText;
class CompButton;
class CompCheckBox;
class CompLight;
class CompBone;

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	GameObject(char* nameGameObject);
	GameObject(char* nameGameObject, uint uuid);
	GameObject(const GameObject& copy, bool haveparent = false, GameObject* parent = nullptr);
	~GameObject();

	// Play Engine
	// Return if all scripts in scene are compilated correctly.
	bool CheckScripts(int & numfails);
	void StartScripts();
	void ClearAllVariablesScript();
	GameObject* GetGameObjectbyuid(uint uid);
	GameObject* GetGameObjectfromScene(int);
	void FindChildsWithTag(const char* tag, std::vector<GameObject*>* vec);
	GameObject * FindGameObjectWithTag(const char * tag);

	void Init();
	void PreUpdate(float dt);
	void Update(float dt);
	void postUpdate();
	bool CleanUp();
	bool Enable();
	bool Disable();
	void Draw();

	void SetActive(bool active);
	void SetVisible(bool visible);
	void SetStatic(bool set_static);
	void FixedDelete(bool check_delete);
	void SetName(char* name);
	void SetTag(char* tag);

	bool IsActive() const;
	bool IsVisible() const;
	bool IsStatic() const;
	bool IsDeleteFixed() const;
	const char* GetName() const;
	const char* GetTag()const;
	bool CompareTag(const char* str)const;

	// EDITOR METHODS ------------------
	void ShowHierarchy();
	void ShowGameObjectOptions();
	void ShowInspectorInfo();
	void FreezeTransforms(bool freeze, bool change_childs);
	void ShowFreezeChildsWindow(bool freeze, bool& active);
	// ----------------------------------

	// Name -------------
	void NameNotRepeat(std::string& name, bool haveParent, GameObject* parent);

	// Components -----------------------------

	Component* AddComponent(Comp_Type type, bool isFromLoader = false);
	void AddComponentCopy(const Component& copy);
	int GetNumComponents() const;
	Component* FindComponentByType(Comp_Type type) const;
	Component* FindParentComponentByType(Comp_Type type)const;
	Component* FindComponentByUUID(uint uid)const;

	void GetComponentsByType(Comp_Type type, std::vector<Component*>* fill_comp, bool iterate_hierarchy = false) const;
	Component* GetComponentByName(const char* name_component) const;

	CompTransform* GetComponentTransform() const;
	CompRectTransform * GetComponentRectTransform() const;
	CompMesh* GetComponentMesh() const;
	CompMaterial* GetComponentMaterial() const;
	CompScript * GetComponentScript() const;
	CompAnimation* GetComponentAnimation() const;
	CompLight* GetComponentLight() const;
	CompBone* GetComponentBone() const;
	CompImage* GetComponentImage() const;

	Component* GetComponentbyIndex(uint i) const;
	void DeleteAllComponents();
	void DeleteComponent(Component* component);
	void SaveComponents(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void LoadComponents(const JSON_Object * object, std::string name, uint numComponents);

	void SyncComponents();

	// Childs ---------------------------------
	uint GetNumChilds() const;
	GameObject* GetChildbyIndex(uint pos_inVec) const;
	GameObject* GetChildbyName(const char* name) const;
	GameObject* GetChildByRealName(std::string name) const;
	GameObject* GetChildDeepSearch(const char* name) const;
	uint GetIndexChildbyName(const char* name) const;
	void RemoveChildbyIndex(uint index);
	std::vector<GameObject*> GetChildsVec() const;
	std::vector<GameObject*>* GetChildsPtr();
	void AddChildGameObject(GameObject* child);
	void AddChildGameObject_Copy(const GameObject* child);

	// Transform Modifications -----------------
	void UpdateChildsMatrices();

	// Parent ----------------
	GameObject* GetParent() const; //Not const pointer to enable parent variables modification
	bool HaveParent() const;
	void SetParent(GameObject* new_parent);

	// Bounding Box -----------------------
	void AddBoundingBox(const ResourceMesh* mesh);
	void DrawBoundingBox();
	void SetAABBActive(bool active);
	bool IsAABBActive() const;
	uint GetUUID() const;
	void SetUUID(uint uuid);
	void SetUUIDRandom();

	// Delete -----------------
	bool WanttoDelete() const;
	void SettoDelete();
	void RemoveScriptReference(GameObject* go);

public:
	AABB * bounding_box = nullptr;
	AABB  box_fixed;
	bool beingUsedByScript = false;

private:
	uint uid = 0;
	char* name = "CHANGE THIS";
	char* tag = "undefined";
	bool active = false;
	bool visible = false;
	bool static_obj = false;
	bool to_delete = false; 
	bool fixed_delete = false;
	bool bb_active = false;

	GameObject* parent = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
};

#endif