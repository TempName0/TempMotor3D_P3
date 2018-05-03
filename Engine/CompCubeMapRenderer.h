#ifndef COMPONENT_CUBEMAP_RENDERER
#define COMPONENT_CUBEMAP_RENDERER

#include "Component.h"
#include "CubeMap_Texture.h"
#include "DepthCubeMap.h"
union Event; 

class CompCubeMapRenderer :	public Component
{
public:
	CompCubeMapRenderer(Comp_Type t, GameObject * parent);
	//CompCubeMapRenderer(const CompCubeMapRenderer & copy, GameObject * parent);

	~CompCubeMapRenderer();

	void PreUpdate(float dt);
	void Bake(Event& event);
	
	virtual void ShowInspectorInfo();
	// --------------------------------

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

	void GetOwnBufferSize(uint& buffer_size);
	void SaveBinary(char** cursor, int position) const;
	void LoadBinary(char** cursor);
	// -------------------------------------


public:
	CubeMap_Texture cube_map;
	bool render = false;



};


#endif