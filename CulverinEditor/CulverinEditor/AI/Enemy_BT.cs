﻿using CulverinEditor;
using System.Collections.Generic;
using CulverinEditor.Debug;
using CulverinEditor.Pathfinding;

public class Enemy_BT : BT
{
    public enum ENEMY_STATE
    {
        ENEMY_ALIVE,
        ENEMY_DAMAGED,
        ENEMY_STUNNED,
        ENEMY_DEAD
    }

    public enum ENEMY_GET_DAMAGE_TYPE
    {
        DEFAULT,
        ARROW,
        FIREWALL,
        FIREBALL,
        SWORD,
        FIREBREATH
    }

    public GameObject enemies_manager = null;
    public GameObject player = null;
    public GameObject mesh = null;

    Material enemy_mat;
    public float dmg_alpha = 0.0f;

    public float total_hp = 100;
    public float current_hp = 0;
    public ENEMY_STATE life_state = ENEMY_STATE.ENEMY_ALIVE;

    public float max_anim_speed = 1.5f;
    public float min_anim_speed = 0.5f;

    public float attack_cooldown = 1.0f;
    public float attack_damage = 1.0f;
    public float damaged_limit = 0.6f;
    protected float attack_timer = 0.0f;
    public int range = 1;

    public bool heard_something = false;

    protected bool in_combat = false;

    //Path Go and Back
    public int origin_path_x;
    public int origin_path_y;

    public int end_path_x;
    public int end_path_y;

    //UI Components
    public GameObject enemy_hp_bar;
    public float hp_timer_total = 10.0f;
    public GameObject enemy_hp_border;
    protected float hp_timer = 10.0f;
    protected bool hud_active = false;

    public bool Disable_Movement_Gameplay_Debbuger = false;

    CompAudio audio_comp;
    public bool dying = false;
    public override void Start()
    {
        in_combat = false;
        player = GetLinkedObject("player");
        if (GetLinkedObject("player") == null)
        {
            Debug.Log("PLAYER NULL!!",Department.IA,Color.RED);
        }
        current_hp = total_hp;
        //Enemy starts with the attack loaded
        attack_timer = 0.0f;
        mesh = GetLinkedObject("mesh");
        if (GetLinkedObject("mesh") == null)
        {
            Debug.Log("MESH NULL!!", Department.IA, Color.RED);
        }
        enemies_manager = GetLinkedObject("enemies_manager");
        if (GetLinkedObject("enemies_manager") == null)
        {
            Debug.Log("ENEMIES MANAGER NULL!!", Department.IA, Color.RED);
        }
        GetComponent<CompAnimation>().PlayAnimation("Idle");
        dmg_alpha = 0.0f;
        //ChangeTexturesToAlive();
        enemy_hp_bar = GetLinkedObject("enemy_hp_bar");
        if (GetLinkedObject("enemy_hp_bar") == null)
        {
            Debug.Log("HALTH BAR NULL!!", Department.IA, Color.RED);
        }
        if (enemy_hp_bar.GetComponent<CompImage>() == null)
        {
            Debug.Log("COMP IMAGE NULL!!", Department.IA, Color.RED);
        }
        enemy_hp_border = GetLinkedObject("enemy_hp_border");
        enemy_hp_bar.GetComponent<CompImage>().DeactivateRender();
        enemy_hp_border.GetComponent<CompImage>().DeactivateRender();
        hp_timer_total = 10.0f;
        hp_timer = 0.0f;
        hud_active = false;
        audio_comp = GetComponent<CompAudio>();
        base.Start();
    }

    public override void Update()
    {
        //Update attack cooldown
        attack_timer += Time.deltaTime;
        Debug.Log(state, Department.PHYSICS);

        if (GetCurrentHP() <= 0 && state != AI_STATE.AI_DEAD)
        {
            Debug.Log("DEAD", Department.IA);
            //GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;


            next_action = GetComponent<Die_Action>();
            current_action.Interupt();
            if (GetComponent<EnemySword_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteSwordEnemy(GetComponent<EnemySword_BT>().gameObject);
            else if (GetComponent<EnemyShield_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteShieldEnemy(GetComponent<EnemyShield_BT>().gameObject);
            else if (GetComponent<EnemySpear_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteLanceEnemy(GetComponent<EnemySpear_BT>().gameObject);
        }

        base.Update();
    }

    public override void MakeDecision()
    {
        if (next_action.action_type == Action.ACTION_TYPE.ATTACK_ACTION || next_action.action_type == Action.ACTION_TYPE.PUSHBACK_ACTION
            || next_action.action_type == Action.ACTION_TYPE.STUN_ACTION || next_action.action_type == Action.ACTION_TYPE.SPEARATTACK_ACTION
            || next_action.action_type == Action.ACTION_TYPE.FACE_PLAYER_ACTION || next_action.action_type == Action.ACTION_TYPE.DIE_ACTION
            || next_action.action_type == Action.ACTION_TYPE.SEPARATE_ACTION || next_action.action_type == Action.ACTION_TYPE.GET_HIT_ACTION 
            || next_action.action_type == Action.ACTION_TYPE.SHIELD_BLOCK_ACTION)
        {
            if (next_action.action_type == Action.ACTION_TYPE.STUN_ACTION)
            {
                life_state = ENEMY_STATE.ENEMY_STUNNED;
            }

            current_action = next_action;
            next_action = null_action;
            current_action.ActionStart();
            return;
        }

        if (in_combat)
        {            
            if (next_action.action_type == Action.ACTION_TYPE.DISENGAGE_ACTION)
            {
                in_combat = false;
                current_action = GetComponent<Disengage_Action>();
                current_action.ActionStart();
                return;
            }
          
            InCombatDecesion();
        }
        else
        {
            if (next_action.action_type == Action.ACTION_TYPE.ENGAGE_ACTION)
            {
                in_combat = true;
                current_action = next_action;
                next_action = null_action;
                current_action.ActionStart();
                return;
            }

            OutOfCombatDecesion();
        }
    }

    protected virtual void InCombatDecesion()
    {
        Debug.Log("[error] In Combat Not Defined");
    }

    protected virtual void OutOfCombatDecesion()
    {
        Debug.Log("[error] Out Of Combat Not Defined");
    }

    public virtual bool ApplyDamage(float damage, ENEMY_GET_DAMAGE_TYPE damage_type)
    {
        
        if (current_action.action_type != Action.ACTION_TYPE.PUSHBACK_ACTION)
        {
            Debug.Log("Yes push hit", Department.PHYSICS, Color.BLUE);

            if (GetComponent<EnemySpear_BT>() != null)
                audio_comp.PlayEvent("Enemy2_Hurt");

            if (GetComponent<EnemySword_BT>() != null)
                audio_comp.PlayEvent("Enemy1_Hurt");

            next_action = GetComponent<GetHit_Action>();
            GetComponent<GetHit_Action>().SetHitType(damage_type);
           // if (GetComponent<EnemySpear_BT>() == null)
              //  InterruptAction();
        }
        else
        {
            Debug.Log("Not push hit", Department.PHYSICS, Color.PINK);
        }

        current_hp -= damage;
        //ChangeTexturesToDamaged();
 
        current_interpolation = current_hp / total_hp;
        dmg_alpha += 0.2f;
        if (current_hp <= 0)
        {
            //GetComponent<CompAnimation>().SetClipsSpeed(anim_speed);
            state = AI_STATE.AI_DEAD;
            life_state = ENEMY_STATE.ENEMY_DEAD;

            if (gameObject.GetComponent<CompCollider>()!=null)
            {
                gameObject.GetComponent<CompCollider>().CollisionActive(false);
            }
            next_action = GetComponent<Die_Action>();
            current_action.Interupt();
            if (GetComponent<EnemySword_BT>() != null)enemies_manager.GetComponent<EnemiesManager>().DeleteSwordEnemy(GetComponent<EnemySword_BT>().gameObject);
            else if (GetComponent<EnemyShield_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteShieldEnemy(GetComponent<EnemyShield_BT>().gameObject);
            else if (GetComponent<EnemySpear_BT>() != null) enemies_manager.GetComponent<EnemiesManager>().DeleteLanceEnemy(GetComponent<EnemySpear_BT>().gameObject);
        }
        else if (life_state != ENEMY_STATE.ENEMY_DAMAGED && current_hp < total_hp * damaged_limit)
        {
            life_state = ENEMY_STATE.ENEMY_DAMAGED;
            //ChangeTexturesToDamaged();
        }

        UpdateHUD();

        return true;
    }

    public void PushEnemy(Vector3 dir)
    {
        current_action.Interupt();

        Debug.Log("Enemy:" + gameObject.GetName(), Department.IA);
        Debug.Log("Enemy posx:" + GetComponent<Movement_Action>().GetCurrentTileX() + "posy:" + GetComponent<Movement_Action>().GetCurrentTileY(), Department.IA);
        Debug.Log("Player direction push:" + dir, Department.IA);

        if (!GetComponent<Movement_Action>().IsWalkable((uint)(GetComponent<Movement_Action>().GetCurrentTileX() + dir.x), (uint)(GetComponent<Movement_Action>().GetCurrentTileY() + dir.z)))
            next_action = GetComponent<Stun_Action>();
        else
        {
            next_action = GetComponent<PushBack_Action>();
            ((PushBack_Action)next_action).SetPushDirection(dir);
        }
    }

    public bool InRange()
    {
        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int x, out int y);
        int distance_x = Mathf.Abs(x - GetComponent<Movement_Action>().GetCurrentTileX());
        int distance_y = Mathf.Abs(y - GetComponent<Movement_Action>().GetCurrentTileY());

        if ((distance_x <= range && distance_y == 0) || (distance_y <= range && distance_x == 0))
            return true;
        return false;
    }

    public int GetDistanceInRange()
    {
        if (GetLinkedObject("player_obj") == null)
            Debug.Log("[error] player object is null");

        GetLinkedObject("player_obj").GetComponent<MovementController>().GetPlayerPos(out int player_tile_x, out int player_tile_y);

        int enemy_x = GetComponent<Movement_Action>().GetCurrentTileX();
        int enemy_y = GetComponent<Movement_Action>().GetCurrentTileY();

        int distance_x = player_tile_x - enemy_x;
        int distance_y = player_tile_y - enemy_y;

        if (distance_x != 0 && distance_y != 0)
            return range + 1;
        else
        {
            List<PathNode> tiles_between = new List<PathNode>();

            //no need to check enemy tile nor player's
            if (distance_x != 0)
            {
                if (distance_x > 0)
                    for (int i = 1; i < distance_x; i++)
                        tiles_between.Add(new PathNode(enemy_x + i, enemy_y));
                else
                    for (int i = -1; i > distance_x; i--)
                        tiles_between.Add(new PathNode(enemy_x + i, enemy_y));
            }

            if (distance_y != 0)
            {
                if (distance_y > 0)
                    for (int i = 1; i < distance_y; i++)
                        tiles_between.Add(new PathNode(enemy_x, enemy_y + i));
                else
                    for (int i = -1; i > distance_y; i--)
                        tiles_between.Add(new PathNode(enemy_x, enemy_y + i));
            }

            foreach (PathNode pn in tiles_between)
                if (GetLinkedObject("map").GetComponent<Pathfinder>().IsWalkableTile(pn) == false)
                    return range + 1;

            if (distance_y != 0)
                return Mathf.Abs(distance_y);
            else
                return Mathf.Abs(distance_x);
        }
    }

    public void SetAction(Action.ACTION_TYPE type)
    {
        switch(type)
        {
            case Action.ACTION_TYPE.GET_HIT_ACTION: next_action = GetComponent<GetHit_Action>(); break;
            case Action.ACTION_TYPE.ENGAGE_ACTION: next_action = GetComponent<Engage_Action>(); break;
            case Action.ACTION_TYPE.DISENGAGE_ACTION: next_action = GetComponent<Disengage_Action>(); break;
            case Action.ACTION_TYPE.INVESTIGATE_ACTION: next_action = GetComponent<Investigate_Action>(); break;

            default: Debug.Log("[error] Unknown action"); break;
        }
    }

    public bool InCombat()
    {
        return in_combat;
    }

    public virtual void ChangeTexturesToDamaged()
    {
        Debug.Log("[error] Damaged change Textures not defined");
    }

    public virtual void ChangeTexturesToAlive()
    {
        Debug.Log("[error] Alive change Textures not defined");
    }

    public virtual void UpdateHUD()
    {
        Debug.Log("current" + current_hp, Department.STAGE, Color.BLUE);
        Debug.Log("total" + total_hp, Department.STAGE, Color.RED);

        float calc_hp = current_hp / total_hp;
        Debug.Log("calc_hp" + calc_hp, Department.STAGE, Color.PINK);
        enemy_hp_bar.GetComponent<CompImage>().FillAmount(calc_hp);
        hp_timer = 0.0f;
    }

    public virtual void ActivateHUD(GameObject text)
    {
        hp_timer = 0.0f;
        enemy_hp_bar.GetComponent<CompImage>().ActivateRender();
        enemy_hp_border.GetComponent<CompImage>().ActivateRender();
        text.SetActive(true);
        hud_active = true;
    }

    public virtual void DeactivateHUD(GameObject text)
    {
        enemy_hp_bar.GetComponent<CompImage>().DeactivateRender();
        enemy_hp_border.GetComponent<CompImage>().DeactivateRender();
        text.SetActive(false);
        hud_active = false;
    }

    public float GetCurrentHP()
    {
        return current_hp;
    }
}