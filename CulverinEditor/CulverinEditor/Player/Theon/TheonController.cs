﻿using CulverinEditor;
using CulverinEditor.Debug;

public class TheonController : CharacterController
{
    //MESH ELEMENTS
    public GameObject theon_obj; 
    public GameObject L_Arm_Theon;
    public GameObject R_Arm_Theon;
    public GameObject L_SubArm_Theon;
    public GameObject R_SubArm_Theon;

    //UI ELEMENTS
    public GameObject theon_icon_obj;
    public GameObject theon_icon_obj_hp;
    public GameObject theon_icon_obj_stamina;
    public GameObject theon_button_left;
    public GameObject theon_button_right;
    public GameObject theon_left_flag;
    public GameObject theon_right_flag;


    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_stamina = 100.0f;
    public float curr_stamina = 100.0f;
    public float sec_ability_cd = 10.0f;
    private float sec_ability_current_cd = 10.0f;

    //LEFT ABILITY STATS-------------------
    public float left_ability_dmg = 10;
    public float left_ability_cost = 10.0f;
    private TheonCD_Left cd_left;
    //----------------------------------------
    //RIGHT ABILITY STATS-------------------
    public float right_ability_dmg = 10;
    public float right_ability_cost = 10.0f;
    private TheonCD_Right cd_right;
    //----------------------------------------

    protected override void Start()
    {
        SetPosition(Position.BEHIND);
        sec_ability_current_cd = sec_ability_cd;

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        theon_obj = GetLinkedObject("theon_obj");
        L_Arm_Theon = GetLinkedObject("L_Arm_Theon");
        R_Arm_Theon = GetLinkedObject("R_Arm_Theon");
        L_SubArm_Theon = GetLinkedObject("L_SubArm_Theon");
        R_SubArm_Theon = GetLinkedObject("R_SubArm_Theon");

        theon_icon_obj = GetLinkedObject("theon_icon_obj");
        theon_button_right = GetLinkedObject("theon_button_right");
        theon_button_left = GetLinkedObject("theon_button_left");
        theon_left_flag = GetLinkedObject("theon_left_flag");
        theon_right_flag = GetLinkedObject("theon_right_flag");

        theon_icon_obj_hp = GetLinkedObject("theon_icon_obj_hp");
        theon_icon_obj_stamina = GetLinkedObject("theon_icon_obj_stamina");   

        //Start Idle animation
        //anim_controller = theon_obj.GetComponent<CompAnimation>();    
        //ToggleMesh(false);

        //Move icon to the left
        theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
        GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));

        //Disable Theon Abilities buttons
        EnableAbilities(false);

        Debug.Log(gameObject.GetName());
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        //// Debug method to control Hp
        //CheckHealth();
    
        //// First check if you are alive
        //health = GetLinkedObject("health_obj").GetComponent<Hp>();
        //if (health.GetCurrentHealth() > 0)
        //{
        //    // Check if player is moving to block attacks/abilities
        //    movement = GetLinkedObject("player_obj").GetComponent<MovementController>();
        //    //if (!movement.IsMoving())
        //    if(1 == 1)
        //    {
        //        /* Player is alive */
        //        switch (state)
        //        {
        //            case State.IDLE:
        //                {
        //                    //Check For Input + It has to check if he's moving to block attack (¿?)
        //                    CheckAttack();
        //                    break;
        //                }
        //            case State.ATTACKING:
        //                {
        //                    //Check for end of the Attack animation
        //                    anim_controller = theon_obj.GetComponent<CompAnimation>();
        //                    if (anim_controller.IsAnimationStopped("Attack1"))
        //                    {
        //                        state = State.IDLE;
        //                    }
        //                    else
        //                    {
        //                        // Keep playing specific attack animation  until it ends
        //                        Debug.Log("Theon Attacking");
        //                    }
        //                    break;
        //                }
        //            case State.COVER:
        //                {
        //                    //Check for end of the Attack animation
        //                    anim_controller = theon_obj.GetComponent<CompAnimation>();

        //                    if (anim_controller.IsAnimationStopped("Cover"))
        //                    {
        //                        state = State.IDLE;
        //                    }
        //                    else
        //                    {
        //                        // Keep playing specific attack animation  until it ends
        //                        Debug.Log("Jaime Covering");
        //                    }
        //                    break;
        //                }
        //            case State.BLOCKING:
        //                {
        //                    //Check for end of the Attack animation
        //                    anim_controller = theon_obj.GetComponent<CompAnimation>();
        //                    if (anim_controller.IsAnimationStopped("Block"))
        //                    {
        //                        state = State.IDLE;
        //                    }
        //                    else
        //                    {
        //                        // Keep playing specific attack animation  until it ends
        //                        Debug.Log("Theon Blocking");
        //                    }
        //                    break;
        //                }
        //            case State.HIT:
        //                {
        //                    //Check for end of the Attack animation
        //                    anim_controller = theon_obj.GetComponent<CompAnimation>();
        //                    if (anim_controller.IsAnimationStopped("Hit"))
        //                    {
        //                        state = State.IDLE;
        //                    }
        //                    else
        //                    {
        //                        // Keep playing specific attack animation  until it ends
        //                        Debug.Log("Theon Hit");
        //                    }
        //                    break;
        //                }
        //            case State.DEAD:
        //                {
        //                    Debug.Log("We are going doown");
        //                    break;
        //                }
        //            default:
        //                {
        //                    break;
        //                }
        //        }
        //    }
        //}
    }

    public override void CheckAttack()
    {
        //Left Attack
        if (Input.GetKeyDown(KeyCode.Num1))
        {
            Debug.Log("Theon Pressed 1");
            PrepareLeftAbility();
        }

        //Right Attack
        else if (Input.GetKeyDown(KeyCode.Num2))
        {
            Debug.Log("Theon Pressed 2");
            //PrepareAbility();
        }


        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {

            Debug.Log("Theon Pressed 1");
            PrepareLeftAbility();
        }

        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            Debug.Log("Theon Pressed 2");
            //PrepareAbility();
        }

    }

    public override void SecondaryAbility()
    {
        Debug.Log("Jaime Secondary Ability");
    }


    public override void GetDamage(float dmg)
    {
        if (state == State.COVER)
        {
            SetAnimationTransition("ToBlock", true);

            GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("MetalHit");

            SetState(State.BLOCKING);
        }
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.GetDamage(dmg);

            // SET HIT ANIMATION
            SetAnimationTransition("ToHit", true);

            SetState(State.HIT);
        }
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller = theon_obj.GetComponent<CompAnimation>();
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            Debug.Log("Update HP Theon");          

            //Set Icon in the center
            theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(0.0f, 365.0f, 0.0f));
            theon_icon_obj_hp.GetComponent<CompImage>().SetEnabled(false, theon_icon_obj_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(false, theon_icon_obj_stamina);
            
            //Update HP
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            health.SetHP(curr_hp, max_hp);

            Debug.Log("Update HP Theon");

            //Update Stamina
            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            stamina.SetStamina(curr_stamina, max_stamina);

            //Enable Theon Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            GetLinkedObject("theon_s_button_obj").SetActive(false);

            Debug.Log("Update Stamina Theon");
        }

        //Get values from var and store them
        else
        {
            health = GetLinkedObject("health_obj").GetComponent<Hp>();
            curr_hp = health.GetCurrentHealth();

            stamina = GetLinkedObject("stamina_obj").GetComponent<Stamina>();
            curr_stamina = stamina.GetCurrentStamina();

            //Set icon at the left
            if (left)
            {
                theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(-115.0f, 430.0f, 0.0f));
                GetLinkedObject("theon_s_button_obj").SetActive(true);
                GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(124.0f, -33.0f, 0.0f));
            }

            //Set the icon at the right
            else
            {
                theon_icon_obj.GetComponent<CompRectTransform>().SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                theon_icon_obj.GetComponent<CompRectTransform>().SetPosition(new Vector3(115.0f, 430.0f, 0.0f));
                GetLinkedObject("theon_s_button_obj").SetActive(true);
                GetLinkedObject("theon_s_button_obj").GetComponent<CompRectTransform>().SetPosition(new Vector3(-123.0f, 31.5f, 0.0f));
            }

            //Enable Secondary Bars & Update them
            theon_icon_obj_hp.GetComponent<CompImage>().FillAmount(curr_hp / max_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().FillAmount(curr_stamina / max_stamina);
            theon_icon_obj_hp.GetComponent<CompImage>().SetEnabled(true, theon_icon_obj_hp);
            theon_icon_obj_stamina.GetComponent<CompImage>().SetEnabled(true, theon_icon_obj_stamina);

            //Disable Theon Abilities buttons
            EnableAbilities(false);
        }
        Debug.Log("Update Child Theon");
    }

    public override bool IsAnimationStopped(string name)
    {
        return anim_controller.IsAnimationStopped(name);
    }

    public override void ToggleMesh(bool active)
    {
        L_Arm_Theon.GetComponent<CompMesh>().SetEnabled(active, L_Arm_Theon);
        R_Arm_Theon.GetComponent<CompMesh>().SetEnabled(active, R_Arm_Theon);
        R_SubArm_Theon.GetComponent<CompMesh>().SetEnabled(active, R_SubArm_Theon);
        L_SubArm_Theon.GetComponent<CompMesh>().SetEnabled(active, L_SubArm_Theon);
    }

    public bool IsSecondaryAbilityReady()
    {
        Debug.Log(sec_ability_current_cd);
        if (sec_ability_current_cd <= 0.0f)
            return true;
        else
            return false;
    }

    public override float GetSecondaryAbilityCoolDown()
    {
        return sec_ability_cd;
    }

    public override void ReduceSecondaryAbilityCoolDown()
    {
        sec_ability_current_cd -= Time.DeltaTime();
    }

    public override void ResetCoolDown()
    {
        sec_ability_current_cd = sec_ability_cd;
        Debug.Log("new CD");
        Debug.Log(sec_ability_current_cd);
    }

    public bool OnLeftClick()
    {
        
        // Check if player is in Idle State
        if (state == State.IDLE) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            if (GetCurrentStamina() > left_ability_cost)
            {
                cd_left = theon_button_left.GetComponent<TheonCD_Left>();
                //Check if the ability is not in cooldown
                if (!cd_left.in_cd)
                                {
                    Debug.Log("Theon LW Going to Attack");

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoLeftAbility();
                    SetState(State.ATTACKING);
                    // Set Attacking Animation
                    SetAnimationTransition("ToAttack1", true);

                    // Play the Sound FX
                    PlayFx();

                    return true;
                }
                else
                {
                    Debug.Log("Ability in CD");
                    return false;
                }
            }
            else
            {
                Debug.Log("Not Enough Stamina");
                return false;
            }
        }
        return false;
    }

    public void PrepareLeftAbility()
    {
        Debug.Log("Theon LW Prepare Ability");
        OnLeftClick();
        button = theon_button_left.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        Debug.Log("Theon LW Attack Left");

        // Decrease stamina -----------
        DecreaseStamina(left_ability_cost);

        Debug.Log("Theon LW Going to hit");

        // Attack the enemy in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.Hit(attack_dmg);
        //}
    }

    public void PlayFx()
    {
        //GetLinkedObject("player_obj").GetComponent<CompAudio>().PlayEvent("SwordSlash");
    }

    public bool OnRightClick()
    {
       
        // Check if player is in Idle State
        if (state == State.IDLE)
        {
            // Check if player has enough stamina to perform its attack
            if (GetCurrentStamina() > right_ability_cost)
            {
                cd_right = theon_button_left.GetComponent<TheonCD_Right>();
                //Check if the ability is not in cooldown
                if (!cd_right.in_cd)
                {
                    Debug.Log("Theon RW Going to Block");

                    // First, OnClick of RightWeapon, then, onClick of Cooldown
                    DoRightAbility();
                    SetState(State.ATTACKING);
                    // Set Animation
                    SetAnimationTransition("ToCover", true);

                    return true;

                }
                else
                {
                    Debug.Log("Theon RW Ability in CD");
                    return false;
                }
            }
            else
            {
                Debug.Log("Theon RW Not Enough Stamina");
                return false;
            }
        }
        return false;
    }

    public void PrepareAbility()
    {
        Debug.Log("Theon RW Prepare Block");
        button = theon_button_right.GetComponent<CompButton>();
        button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility() //Might be virtual
    {
        Debug.Log("Theon LW Attack Left");

        // Decrease stamina -----------
        DecreaseStamina(right_ability_cost);

        Debug.Log("Theon LW Going to hit");

        // Attack the enemy in front of you
        //if (GetLinkedObject("player_obj").GetComponent<MovementController>().EnemyInFront())
        //{
        //    // To change => check the specific enemy in front of you
        //    enemy = enemy_obj.GetComponent<EnemyController>();
        //    enemy.Hit(attack_dmg);
        //}
    }

    public override void EnableAbilities(bool active)
    {
        theon_button_left.SetActive(active);
        theon_button_right.SetActive(active);
        theon_left_flag.SetActive(active);
        theon_right_flag.SetActive(active);
    }
}