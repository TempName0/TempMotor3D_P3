#ifndef COMPONENT_FINITE_STATE_MACHINE_H
#define COMPONENT_FINITE_STATE_MACHINE_H

#include "Component.h"
#include "CompScript.h"

class FSM_State;
class FSM_Transition;
class FSM_Condition;
enum FSM_CONDITION_TYPE;

class CompFiniteStateMachine : public Component
{
public:
	CompFiniteStateMachine(Comp_Type c_type, GameObject* parent);
	CompFiniteStateMachine(const CompFiniteStateMachine& copy,GameObject* parent);
	~CompFiniteStateMachine();

	// --------- Game loop ---------
		// Basically for calling the Scripts loop
	void Init();
	void PreUpdate(float dt); 

	void Start();
	void Update(float dt);
	void Clear();
	// ------------------------

	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	// ------------------------

	void CopyValues(const CompFiniteStateMachine * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	FSM_State* CreateState();
	bool DeleteState(FSM_State* state_to_delete);

	// SETTERS ----------------
	bool SetCurrentState(FSM_State* new_current_state);
	bool SetInitialState(FSM_State* new_initial_state);
	// ------------------------

	// GETTERS ----------------
	uint GetNumStates()const;
	FSM_State* GetCurrentState()const;
	FSM_State* GetInitialState()const;
	// ------------------------

private:
	void CheckOpenStateOptions(FSM_State* state);

	// ----- Needed Updating States -----//
	std::vector<FSM_State*> states;
	FSM_State* initial_state;
	FSM_State* current_state;
	// ----- Needed Updating States -----//

	// ----- Needed Creating States ----- //
	uint states_id;
	FSM_State* selected_state;
	FSM_Transition* selected_transition;
	FSM_Condition* selected_condition;
	FSM_State* target_state;		//for creating transitions
	FSM_Transition* new_transition;	//When creating a new transition, in order to fill all the conditions
	// ----- Needed Creating states ----- //

	// ----- Visual Scripting ----- //
	bool show_fsm;
	bool show_create_transition_window;
	std::vector<int> new_conditions;	// Saves the type and number of the new condition that are being created
	std::vector<int>::iterator condition_to_erase;
	bool show_create_conditions_window;
	bool show_selecting_script_window;
	// ----- Visual Scripting ----- //
};

// --------------------------------  STATES  -------------------------------- //

class FSM_State
{
public:
	FSM_State(int state_id_);
	FSM_State(const FSM_State& copy, int state_id_);	//doesn't copy the transitions, makes no sense
	~FSM_State();

	bool Init();
	bool PreUpdate(float dt);
	bool StartScripts();
	//TODO
	void DoEntryAction();
	void DoAction(float dt);
	void DoExitAction();

	// SAVE - LOAD METHODS ----------------
	void SaveState(JSON_Object * object, std::string name, bool saveScene, uint & countResources, int state_num);
	void LoadState(const JSON_Object * object, std::string name, int num_state, GameObject* parent);
	void LoadTransitionsTargetStates(const std::vector<FSM_State*>& states);
	// -------------------------------------

	bool CheckTriggeredTransition(FSM_Transition** transition)const;	
	
	bool AddScript(CompScript* script_to_add);	// Only support 1 script per node yet
	bool SelectScript(bool& selecting);
	bool RemoveScript();	// Also deletes all the transitions(as their conditions deppend on the script variables)
	FSM_Transition* AddTransition(FSM_State* target_state);
	void DeleteTransitionsWithTargetState(FSM_State* target_state);
	bool DeleteAllTransitions();
	bool DeleteTransition(FSM_Transition* transition_to_delete);

	// ----- Visual Scripting ----- //
	bool DisplayTransitionsInfo(FSM_Transition** selected_transition, FSM_Condition** selected_condition); // Returns true if any condition was selected
	bool ShowScriptInfo();
	// ----- Visual Scripting ----- //

	// SETTERS ----------------
	void SetStateName(const char* new_name);
	// ------------------------

	// GETTERS ----------------
	int GetStateID()const;
	uint GetNumTransitions()const;
	const char* GetStateName()const;
	const char* GetScriptName()const;
	CompScript* GetScript()const;
	// ------------------------

private:	
	uint state_id;
	std::string state_name;
	CompScript* script;
	std::vector<FSM_Transition*> transitions;
};

// --------------------------------  STATES  -------------------------------- //

// --------------------------------  TRANSITIONS  -------------------------------- //

class FSM_Transition
{
public:
	FSM_Transition();
	FSM_Transition(FSM_State* target_state_);
	~FSM_Transition();

	// SAVE - LOAD METHODS ----------------
	void SaveTransition(JSON_Object * object, std::string name, bool saveScene, uint & countResources, int state_num, int transition_num);
	void LoadTransition(const JSON_Object * object, std::string name, int num_state, int num_transition, FSM_State* state_loading);
	// -------------------------------------

	// TODO: Should Ints/Floats transform in each other in case of using wrong function to avoid errors ???
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, bool condition_b);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, int condition_a, int condition_b);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, float condition_a, float condition_b);

	bool IsTriggered(const FSM_State* current_state)const;

	// --- Visual Scripting --- //
	void CreateConditionsModifyingOptions(FSM_State* selected_state);
	bool DisplayConditionsInfo(FSM_Condition** selected_condition); // Returns true if any condition was selected
	// --- Visual Scripting --- //

	bool DeleteAllConditions();
	bool DeleteCondition(FSM_Condition* condition_to_delete);

	// SETTERS ----------------
	void SetTargetState(FSM_State* new_target_state);
	// ------------------------
	
	// GETTERS ----------------
	int GetTargetStateID();
	FSM_State* GetTargetState();
	uint GetNumConditions()const;
	// ------------------------

private:
	int target_state_id;
	FSM_State* target_state;
	std::vector<FSM_Condition*> conditions;
	bool add_second_variable_from_script = false;
};

// --------------------------------  TRANSITIONS  -------------------------------- //

// --------------------------------  CONDITIONS  -------------------------------- //

enum FSM_CONDITION_TYPE
{
	FSM_COND_NONE = -1,
	FSM_COND_BOOL = 0,
	FSM_COND_EQUAL_INT,
	FSM_COND_GREATER_THAN_INT,
	FSM_COND_GREATER_EQUAL_INT,
	FSM_COND_LOWER_THAN_INT,
	FSM_COND_LOWER_EQUAL_INT,
	FSM_COND_EQUAL_FLOAT,
	FSM_COND_GREATER_THAN_FLOAT,
	FSM_COND_GREATER_EQUAL_FLOAT,
	FSM_COND_LOWER_THAN_FLOAT,
	FSM_COND_LOWER_EQUAL_FLOAT,
	FSM_COND_MAX
};

class FSM_Condition
{
public:
	FSM_Condition();
	FSM_Condition(FSM_CONDITION_TYPE condition_type_);
	~FSM_Condition();

	// SAVE - LOAD METHODS ----------------
	virtual void SaveCondition(JSON_Object * object, std::string name, bool saveScene, uint & countResources) {}
	virtual void LoadCondition(const JSON_Object * object, std::string name, FSM_State* loading_state) {}
	// -------------------------------------

	virtual bool Test(const FSM_State* current_state)	{ return false; }

	// SETTERS ----------------
	virtual void SetConditionB(bool condition_b_) { }
	virtual bool SetCondition(int condition_a_, int condition_b_)	{ return false; }
	virtual bool SetCondition(float condition_a_, float condition_b_)	{ return false; }
	void SetConditionType(FSM_CONDITION_TYPE condition_type_);
	// ------------------------
	
	// GETTERS ----------------
	FSM_CONDITION_TYPE GetConditionType()const;
	const char* GetConditionTypeStr()const;
	// ------------------------

	const char* variable_a_name = NULL;

private:
	void DeleteCandidateIfRightClicked();

	FSM_CONDITION_TYPE condition_type;
};

	// BOOL ----------------

struct FSM_ConditionBool : public FSM_Condition
{
public:
	FSM_ConditionBool(bool condition_b_);
	~FSM_ConditionBool();

	// SAVE - LOAD METHODS ----------------
	void SaveCondition(JSON_Object * object, std::string name, bool saveScene, uint & countResources);
	void LoadCondition(const JSON_Object * object, std::string name, FSM_State* loading_state);
	// -------------------------------------
	
	bool Test(const FSM_State* current_state);

	// SETTERS ----------------
	bool SetScriptVariable(ScriptVariable** script_variable);
	void SetConditionB(bool condition_b_);
	// ------------------------

	// GETTERS ----------------
	const char* GetConditionAName()const;
	ScriptVariable* GetConditionA()const;
	bool GetConditionB()const;
	// ------------------------

private:
	std::string condition_script_name;	//for Load/Save
	ScriptVariable * condition_script_a;
	bool condition_b;
};

	// ------------------------
	// INTEGERS ----------------

struct FSM_ConditionEqualInt : public FSM_Condition
{
public:
	FSM_ConditionEqualInt(int condition_a_, int condition_b_);
	~FSM_ConditionEqualInt();

	bool Test();//TODODODOOD

	// SETTERS ----------------
	bool SetCondition(int condition_a_, int condition_b_);
	// ------------------------

	// GETTERS ----------------
	int GetConditionA()const;
	// ------------------------

private:
	int condition_a;
	int condition_b;
};

struct FSM_ConditionGreaterThanInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanInt(int condition_a_, int condition_b_);
	~FSM_ConditionGreaterThanInt();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(int condition_a_, int condition_b_);
	// ------------------------

	// GETTERS ----------------
	int GetConditionA()const;
	// ------------------------

private:
	int condition_a;
	int condition_b;
};

struct FSM_ConditionGreaterEqualInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualInt(int condition_a_, int condition_b_);
	~FSM_ConditionGreaterEqualInt();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(int condition_a_, int condition_b_);
	// ------------------------

	// GETTERS ----------------
	int GetConditionA()const;
	// ------------------------

private:
	int condition_a;
	int condition_b;
};

struct FSM_ConditionLowerThanInt : public FSM_Condition
{
public:
	FSM_ConditionLowerThanInt(int condition_a_, int condition_b_);
	~FSM_ConditionLowerThanInt();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(int condition_a_, int condition_b_);
	// ------------------------

	// GETTERS ----------------
	int GetConditionA()const;
	// ------------------------

private:
	int condition_a;
	int condition_b;
};

struct FSM_ConditionLowerEqualInt : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualInt(int condition_a_, int condition_b_);
	~FSM_ConditionLowerEqualInt();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(int condition_a_, int condition_b_);
	// ------------------------

	// GETTERS ----------------
	int GetConditionA()const;
	// ------------------------

private:
	int condition_a;
	int condition_b;
};

	// ------------------------
	// FLOATS ----------------

struct FSM_ConditionEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionEqualFloat(float condition_a_, float condition_b_);
	~FSM_ConditionEqualFloat();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(float condition_a_, float condition_b_);
	// ------------------------

	// GETTERS ----------------
	float GetConditionA()const;
	// ------------------------

private:
	float condition_a;
	float condition_b;
};

struct FSM_ConditionGreaterThanFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanFloat(float condition_a_, float condition_b_);
	~FSM_ConditionGreaterThanFloat();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(float condition_a_, float condition_b_);
	// ------------------------

	// GETTERS ----------------
	float GetConditionA()const;
	// ------------------------

private:
	float condition_a;
	float condition_b;
};

struct FSM_ConditionGreaterEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualFloat(float condition_a_, float condition_b_);
	~FSM_ConditionGreaterEqualFloat();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(float condition_a_, float condition_b_);
	// ------------------------

	// GETTERS ----------------
	float GetConditionA()const;
	// ------------------------

private:
	float condition_a;
	float condition_b;
};

struct FSM_ConditionLowerThanFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerThanFloat(float condition_a_, float condition_b_);
	~FSM_ConditionLowerThanFloat();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(float condition_a_, float condition_b_);
	// ------------------------

	// GETTERS ----------------
	float GetConditionA()const;
	// ------------------------

private:
	float condition_a;
	float condition_b;
};

struct FSM_ConditionLowerEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualFloat(float condition_a_, float condition_b_);
	~FSM_ConditionLowerEqualFloat();

	bool Test();

	// SETTERS ----------------
	bool SetCondition(float condition_a_, float condition_b_);
	// ------------------------

	// GETTERS ----------------
	float GetConditionA()const;
	// ------------------------

private:
	float condition_a;
	float condition_b;
};
	// ------------------------

// --------------------------------  CONDITIONS  -------------------------------- //

#endif // !COMPONENT_FINITE_STATE_MACHINE_H