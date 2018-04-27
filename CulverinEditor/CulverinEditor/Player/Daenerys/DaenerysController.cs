﻿using CulverinEditor;
using CulverinEditor.Debug;

public class DaenerysController : CharacterController
{
    //MESH ELEMENTS
    public GameObject daenerys_obj;
    public GameObject rarm_daenerys_obj;
    public GameObject larm_daenerys_obj;

    //UI ELEMENTS
    public GameObject daenerys_icon_obj;
    public GameObject daenerys_icon_obj_hp;
    public GameObject daenerys_icon_obj_mana;

    public GameObject daenerys_button_left_obj;
    public GameObject daenerys_button_left_obj_idle;

    public GameObject daenerys_button_right_obj;
    public GameObject daenerys_button_right_obj_idle;

    GameObject daenerys_s_button;
    GameObject daenerys_s_button_idle;

    public GameObject daenerys_left_flag;
    public GameObject daenerys_right_flag;

    //FIREBALL PARTICLES
    public GameObject daenerys_fireball_particles;
    public GameObject daenerys_fireball_particles2;
    public GameObject daenerys_fireball_particles3;
    public GameObject daenerys_fireball_particles4;

    //CAMERA
    public GameObject Global_Camera;

    /* Stats to modify Hp/Stamina bar depending on current character */
    public float max_hp = 100.0f;
    public float curr_hp = 100.0f;
    public float max_mana = 100.0f;
    public float curr_mana = 100.0f;
    private float mana_regen = 0.0f;

    //Sec Ability Stats
    public float sec_ability_cost = 30.0f;
    private DaenerysCD_Secondary sec_ability_cd;

    //Left Ability Stats
    public float mana_cost_percentage_left = 20.0f;
    public float damage_percentage_left = 10.0f;
    public int distance_left_attack = 3;
    private DaenerysCD_Left cd_left;
    float AttackLeftTimer = 0.0f;
    
    //Right Ability Stats
    public float mana_cost_percentage_right = 20.0f;
    public float damage_percentage_right = 10.0f;
    private DaenerysCD_Right cd_right;
    private bool set_fire_wall = false;

    private bool play_audio_roar = false;

    protected override void Start()
    {
        base.Start();

        SetPosition(Position.BEHIND_RIGHT);

        // LINK VARIABLES TO GAMEOBJECTS OF THE SCENE
        daenerys_obj = GetLinkedObject("daenerys_obj");
        larm_daenerys_obj = GetLinkedObject("larm_daenerys_obj");
        rarm_daenerys_obj = GetLinkedObject("rarm_daenerys_obj");

        daenerys_icon_obj = GetLinkedObject("daenerys_icon_obj");

        daenerys_button_left_obj = GetLinkedObject("daenerys_button_left_obj");
        daenerys_button_left_obj_idle = GetLinkedObject("daenerys_button_left_obj_idle");
        cd_left = daenerys_button_left_obj.GetComponent<DaenerysCD_Left>();

        daenerys_button_right_obj = GetLinkedObject("daenerys_button_right_obj");
        daenerys_button_right_obj_idle = GetLinkedObject("daenerys_button_right_obj_idle");
        cd_right = daenerys_button_right_obj.GetComponent<DaenerysCD_Right>();

        daenerys_s_button = GetLinkedObject("daenerys_s_button_obj");
        daenerys_s_button_idle = GetLinkedObject("daenerys_s_button_obj_idle");
        sec_ability_cd = daenerys_s_button.GetComponent<DaenerysCD_Secondary>();

        daenerys_left_flag = GetLinkedObject("daenerys_left_flag");
        daenerys_right_flag = GetLinkedObject("daenerys_right_flag");

        daenerys_icon_obj_hp = GetLinkedObject("daenerys_icon_obj_hp");
        daenerys_icon_obj_mana = GetLinkedObject("daenerys_icon_obj_mana");
        mana_regen = mana.regen;

        daenerys_fireball_particles = GetLinkedObject("daenerys_fireball_particles");
        daenerys_fireball_particles2 = GetLinkedObject("daenerys_fireball_particles2");
        daenerys_fireball_particles3 = GetLinkedObject("daenerys_fireball_particles3");
        daenerys_fireball_particles4 = GetLinkedObject("daenerys_fireball_particles4");

        Global_Camera = GetLinkedObject("Global_Camera");

        //Link components with correct gameobjects
        LinkComponents(daenerys_icon_obj, daenerys_icon_obj_hp, null, daenerys_icon_obj_mana, 
                       daenerys_button_left_obj, daenerys_button_right_obj, daenerys_s_button, daenerys_s_button_idle, 
                       GetLinkedObject("daenerys_left_cd_text"), GetLinkedObject("daenerys_right_cd_text"), GetLinkedObject("daenerys_secondary_cd_text"),
                       larm_daenerys_obj, rarm_daenerys_obj, daenerys_button_left_obj_idle, daenerys_button_right_obj_idle);

        // Start Idle animation
        anim_controller.PlayAnimationNode("Out");
        ToggleMesh(false);

        //Disable Mana bar
        mana_img.SetEnabled(false);

        //Move icon to the right
        icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
        icon_trans.SetUIPosition(new Vector3(115.0f, 100.0f, 0.0f));

        icon_img.SetColor(new Vector3(1.0f, 1.0f, 1.0f), 1.0f);

        sec_button_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
        sec_button_idle_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));

        AttackLeftTimer = 0.0f;

        //Disable Daenerys Abilities buttons
        EnableAbilities(false);
    }

    public override void Update()
    {
        base.Update();
    }

    public override void ControlCharacter()
    {
        //Debug.Log(state, Department.PLAYER);
        // First check if you are alive
        if (health.GetCurrentHealth() > 0)
        {
            // Check if player is moving to block attacks/abilities
            if (!movement.IsMoving())
            {
                /* Player is alive */

                switch (state)
                {
                    case State.IDLE:
                        {
                            //Check For Input + It has to check if she's moving to block attack (¿?)
                            CheckAttack();
                            break;
                        }
                    case State.ATTACKING:
                        {
                            AttackLeftTimer += Time.deltaTime;
                            //Check for end of the Attack animation
                            if (anim_controller.IsAnimationStopped("Idle"))
                            { 
                                if (anim_controller.IsAnimOverXTime(0.6f) && play_audio_roar)
                                {
                                    GameObject flamethr = Instantiate("CorrectDaenerysFireBreath");

                                    flamethr.transform.SetPosition(GetFlameBreathPosition(curr_position));
                                    flamethr.transform.SetRotation(flamethr.transform.GetRotation());

                                    Fireball fballscript = flamethr.GetComponent<Fireball>();
                                    fballscript.vfront = curr_forward;

                                    GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
                                    if (coll_object != null)
                                    {
                                        coll_object.GetTag();
                                        if (coll_object.CompareTag("Enemy"))
                                        {
                                            fballscript.vfront = GetSecondaryForwardToEnemy(flamethr.transform.GetPosition(), coll_object.transform.GetPosition());
                                        }
                                    }
                                    PlayFx("DaenerysDragonRoar");                             
                                    play_audio_roar = false;
                                }

                                if (anim_controller.IsAnimationStopped("AttackLeft"))
                                {
                                    state = State.IDLE;
                                }
                            }
                            else if(play_audio_roar == false)
                            {
                                state = State.IDLE;
                            }
                            if(AttackLeftTimer >= 1.35f)
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.FIRE_WALL:
                        {
                            //Check for end of the Attack animation
                            if (anim_controller.IsAnimationStopped("Idle"))
                            {
                                if (set_fire_wall == false && anim_controller.IsAnimOverXTime(0.3f))
                                {
                                    int tile_x, tile_y;
                                    movement.GetPlayerPos(out tile_x, out tile_y);
                                    Vector3 player_pos = player_obj.GetComponent<Transform>().GetPosition();
                                    player_pos.y -= 9.1f;
                                    MovementController.Direction direction = movement.curr_dir;
                                    switch (direction)
                                    {
                                        case MovementController.Direction.NORTH:
                                            {
                                                tile_y -= 1;
                                                player_pos.z -= 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.SOUTH:
                                            {
                                                tile_y += 1;
                                                player_pos.z += 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.EAST:
                                            {
                                                tile_x += 1;
                                                player_pos.x += 25.4f;
                                                break;
                                            }
                                        case MovementController.Direction.WEST:
                                            {
                                                tile_x -= 1;
                                                player_pos.x -= 25.4f;
                                                break;
                                            }
                                        default:
                                            {
                                                break;
                                            }
                                    }
                                    //GET TILE POS!
                                    GameObject fire_wall = Instantiate("FireWall");
                                    fire_wall.transform.SetPosition(player_pos);
                                    fire_wall.GetComponent<FireWall>().SetTiles(tile_x, tile_y);
                                    set_fire_wall = true;
                                }

                                if (anim_controller.IsAnimOverXTime(0.8f))
                                {
                                    state = State.IDLE;
                                }
                            }
                            else if(set_fire_wall == true && IsAnimationStopped("AttackRight"))
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.HIT:
                        {
                            //Check for end of the Attack animation
                            if (anim_controller.IsAnimationStopped("Hit"))
                            {
                                state = State.IDLE;
                            }
                            break;
                        }
                    case State.DEAD:
                        {
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
    }

    public override void ManageEnergy()
    {
        if (state != State.DEAD)
        {
            if (curr_mana < max_mana)
            {
                curr_mana += mana_regen;
                if (curr_mana > max_mana)
                {
                    curr_mana = max_mana;
                }
                float calc_mana = curr_mana / max_mana;
                icon_mana_img.FillAmount(calc_mana);
            }
        }
    }

    public override void CheckAttack()
    {
        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {
            PrepareLeftAbility();
        }
        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            PrepareRightAbility();
        }
    }

    public override bool GetDamage(float dmg)
    {
        health.GetDamage(dmg);

        // SET HIT ANIMATION
        if (health.GetCurrentHealth() > 0)
        {
            if (GetState() == 0)
            {
                Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("Hit");
                SetAnimationTransition("ToHit", true);
                SetState(State.HIT);
            }

            PlayFx("DaenerysHurt");

            //Damage Feedback
            damage_feedback.SetDamage(health.GetCurrentHealth(), max_hp);
        }

        else
        {
            SetAnimationTransition("ToDeath", true);
            Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Death");
            SetState(State.DEAD);

            PlayFx("DaenerysDead");
        }

        return true;
    }

    public override void SetAnimationTransition(string name, bool value)
    {
        anim_controller.SetTransition(name, value);
    }

    public override void UpdateHUD(bool active, bool left)
    {
        //Update Hp bar
        if (active)
        {
            //Set Icon in the center
            icon_trans.SetScale(new Vector3(1.0f, 1.0f, 1.0f));
            icon_trans.SetUIPosition(new Vector3(0.0f, 22.0f, 0.0f));
            icon_hp_img.SetEnabled(false);
            icon_mana_img.SetEnabled(false);

            //Update HP
            health.SetHP(curr_hp, max_hp);

            //Disable Stamina Bar
            stamina_img.SetEnabled(false);
            leftamina_img.SetEnabled(false);

            //Enable Mana Bar
            mana_img.SetEnabled(true);
            left_mana_img.SetEnabled(true);

            //Update Mana 
            mana.SetMana(curr_mana, max_mana);

            //Enable Daenerys Abilities buttons
            EnableAbilities(true);

            //Disable Secondary button
            sec_button.SetInteractivity(false);
            sec_button_img.SetRender(false);
            sec_button_idle_img.SetRender(false);
        }

        //Get values from var and store them
        else
        {
            curr_hp = health.GetCurrentHealth();
            curr_mana = mana.GetCurrentMana();

            //Set icon at the left
            if (left)
            {
                icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                icon_trans.SetUIPosition(new Vector3(-115.0f, 100.0f, 0.0f));
                sec_button_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));
                sec_button_idle_trans.SetUIPosition(new Vector3(124.0f, -33.0f, 0.0f));

            }
            //Set the icon at the right
            else
            {
                icon_trans.SetScale(new Vector3(0.7f, 0.7f, 0.7f));
                icon_trans.SetUIPosition(new Vector3(115.0f, 100.0f, 0.0f));
                sec_button_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
                sec_button_idle_trans.SetUIPosition(new Vector3(-123.0f, -31.5f, 0.0f));
            }

            //Enable Secondary Button
            sec_button.SetInteractivity(true);
            sec_button_img.SetRender(true);
            sec_button_idle_img.SetRender(true);

            //Enable Secondary Bars & Update them
            icon_hp_img.FillAmount(curr_hp / max_hp);
            icon_mana_img.FillAmount(curr_mana / max_mana);
            icon_hp_img.SetEnabled(true);
            icon_mana_img.SetEnabled(true);

            //Disable Mana Bar
            mana_img.SetEnabled(false);
            left_mana_img.SetEnabled(false);

            //Enable Stamina Bar
            stamina_img.SetEnabled(true);
            leftamina_img.SetEnabled(true);

            //Disable Daenerys Abilities buttons
            EnableAbilities(false);
        }
    }

    public override bool IsAnimationStopped(string name)
    {
        return anim_controller.IsAnimationStopped(name);
    }

    public override bool IsAnimationRunning(string name)
    {
        return anim_controller.IsAnimationRunning(name);
    }

    public override void ToggleMesh(bool active)
    {
        left_arm.SetEnabled(active);
        right_arm.SetEnabled(active);
    }

    //LEFT ARM ------------------------------
    public bool OnLeftClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0) /*0 = IDLE*/
        {
            // Check if player has enough stamina to perform its attack
            float mana_cost = mana_cost_percentage_left * max_mana / 100.0f;
            if (CanWasteMana(mana_cost))
            {
                //Check if the ability has enough charges
                if (cd_left.GetCurrentCharges() > 0)
                {
                    //Set Attacking State
                    SetState(State.ATTACKING);

                    // First, OnClick of LeftWeapon, then, onClick of Cooldown
                    DoLeftAbility();

                    // Set Attacking Animation
                    SetAnimationTransition("ToAttackLeft", true);

                    Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Firebreath");

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (characters_manager.daenerys_tired == false)
                {
                    PlayFx("DaenerysTired");
                    characters_manager.daenerys_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public void PrepareLeftAbility()
    {
        left_button.Clicked(); // This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoLeftAbility() //Might be virtual
    {
        // Decrease mana -----------
        DecreaseManaPercentage(mana_cost_percentage_left);
        PlayFx("Dracarys");
        AttackLeftTimer = 0.0f;
        play_audio_roar = true;
    }

    //------------------------------
    //RIGHT ARM --------------------

    public bool OnRightClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough mana to perform its attack
            int tile_x, tile_y;
            movement.GetPlayerPos(out tile_x, out tile_y);
            MovementController.Direction direction = movement.curr_dir;
            switch (direction)
            {
                case MovementController.Direction.NORTH:
                    {
                        tile_y -= 1;
                        break;
                    }
                case MovementController.Direction.SOUTH:
                    {
                        tile_y += 1;
                        break;
                    }
                case MovementController.Direction.EAST:
                    {
                        tile_x += 1;
                        break;
                    }
                case MovementController.Direction.WEST:
                    {
                        tile_x -= 1;
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
            if (movement.CheckIsWalkable(tile_x, tile_y))
            {
                float mana_cost = mana_cost_percentage_right * max_mana / 100.0f;
                if (CanWasteMana(mana_cost))
                {
                    //Check if the ability is not in cooldown
                    if (!cd_right.in_cd)
                    {
                        SetState(State.FIRE_WALL);

                        // First, OnClick of RightWeapon, then, onClick of Cooldown
                        DoRightAbility();

                        // Set Animation
                        Global_Camera.GetComponent<CompAnimation>().PlayAnimationNode("D_Firewall");
                        SetAnimationTransition("ToAttackRight", true);

                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    if (characters_manager.daenerys_tired == false)
                    {
                        PlayFx("DaenerysTired");
                        characters_manager.daenerys_tired = true;
                    }
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        return false;
    }

    public void PrepareRightAbility()
    {
        right_button.Clicked(); //This will execute Cooldown & Weapon OnClick Methods
    }

    public void DoRightAbility()
    {
        // Decrease mana -----------
        DecreaseManaPercentage(mana_cost_percentage_right);

        set_fire_wall = false;

        //GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 30 * distance_left_attack);
        //if (coll_object != null)
        //{
        //    CompCollider obj_collider = coll_object.GetComponent<CompCollider>();
        //    if (obj_collider != null)
        //    {
        //        obj_collider.CallOnContact();
        //    }
        //}
    }

    public bool OnSecondaryClick()
    {
        // Check if player is in Idle State
        if (GetState() == 0)
        {
            // Check if player has enough stamina to perform its attack
            if (CanWasteStamina(sec_ability_cost))
            {
                //Check if the ability is not in cooldown
                if (!sec_ability_cd.in_cd)
                {
                    SecondaryAbility();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (characters_manager.daenerys_tired == false)
                {
                    PlayFx("DaenerysTired");
                    characters_manager.daenerys_tired = true;
                }
                return false;
            }
        }
        return false;
    }

    public override void SecondaryAbility()
    {
        GameObject fball = Instantiate("DaenerysFireball");

        fball.transform.SetPosition(GetSecondaryPosition(curr_position));
        fball.transform.SetRotation(player_obj.transform.GetRotation());

        Fireball fballscript = fball.GetComponent<Fireball>();
        fballscript.vfront = curr_forward;
        fballscript.fireball_particles = daenerys_fireball_particles;
        fballscript.fireball_particles2 = daenerys_fireball_particles2;
        fballscript.fireball_particles3 = daenerys_fireball_particles3;
        fballscript.fireball_particles4 = daenerys_fireball_particles4;

        GameObject coll_object = PhysX.RayCast(curr_position, curr_forward, 254.0f);
        if(coll_object != null)
        {
            coll_object.GetTag();
            if(coll_object.CompareTag("Enemy"))
            {
                fballscript.vfront = GetSecondaryForwardToEnemy(fball.transform.GetPosition(), coll_object.transform.GetPosition());
            }
        }

        // Decrease stamina -----------
        DecreaseStamina(sec_ability_cost);
    }

    public override void EnableAbilities(bool active)
    {
        base.EnableAbilities(active);

        //Flags
        daenerys_left_flag.SetActive(active);
        daenerys_right_flag.SetActive(active);
    }

    public void Heal(float percentage)
    {
        if (state != State.DEAD)
        {
            curr_hp += max_hp * percentage;
            if (curr_hp > max_hp)
            {
                curr_hp = max_hp;
            }
            icon_hp_img.FillAmount(curr_hp / max_hp);
        }
    }
}