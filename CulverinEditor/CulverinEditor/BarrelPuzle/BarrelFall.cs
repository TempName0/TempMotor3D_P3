﻿using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
public class BarrelFall : CulverinBehaviour
{
    BarrelMovement barrel_mov;

    public GameObject barrel_mov_go;
    public GameObject puzzle_generator_go;
    private BarrelPuzzleGenerator puzzle_generator;
    CompRigidBody rigid_body;
    Vector3 start_pos;
    bool falling = false;
    
    float fall_x_pos = 0;
    float final_x_pos = 0;
    float start_x_pos = 0;
    bool move_done = false;
    bool calc_final_pos = false;
    void Start()
    {
        barrel_mov_go = GetLinkedObject("barrel_mov_go");
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        rigid_body = gameObject.GetComponent<CompRigidBody>();
        start_pos = gameObject.GetComponent<Transform>().local_position;

        puzzle_generator_go = GetLinkedObject("puzzle_generator_go");
        puzzle_generator = puzzle_generator_go.GetComponent<BarrelPuzzleGenerator>();

        Debug.Log(start_pos.ToString());
        falling = false;
        move_done = false;
    }

    void Update()
    {

        if (!move_done && !falling)
        {
            barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
            if (barrel_mov.restart)
            {
                //CAL FUNCTION
                Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();

                BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
                Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);

                CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
                rigid.MoveKinematic(start_pos * 13 + parent_pos, quat);

                rigid.ResetForce();
                rigid.ApplyImpulse(new Vector3(1, 0, 0));

            }
        }

        if (falling)
        {
            //ONE WAY TO CONTROL WHERE THE BARRELS FALL
            BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);
            Vector3 actual_pos = gameObject.GetComponent<Transform>().local_position*13 + parent_pos;
            CompRigidBody rigid = gameObject.GetComponent<CompRigidBody>();
            //ADD THIS X POSITION
            Vector3 pos;
            Debug.Log("Enter");
            if ((actual_pos.x) - final_x_pos > 0.1f || (actual_pos.x) - final_x_pos < 0.1f)
            {
                pos = new Vector3(actual_pos.x + (final_x_pos - actual_pos.x)/10.0f, actual_pos.y - 2.0f, actual_pos.z);
            }
            else
            {
                pos = new Vector3(actual_pos.x, actual_pos.y - 0.1f, actual_pos.z);
            }

            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();
            if (actual_pos.y > -15.0f)
            {
                rigid.MoveKinematic(pos, quat);
            }
            else
            {
                actual_pos.x = final_x_pos;
                rigid.MoveKinematic(actual_pos, quat);
                rigid.LockTransform();
                falling = false;
                move_done = true;
                Debug.Log("Before Barrel Fall");
                puzzle_generator.OnBarrelFall(gameObject);
                Debug.Log("Locked");
      

            }
        }
        if (calc_final_pos)
        {
            Quaternion quat = rigid_body.GetColliderQuaternion();
            BarrelManage nmana = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(nmana.restart_pos_x, nmana.restart_pos_y, nmana.restart_pos_z);

            Debug.Log("parent pos : " + parent_pos);
            start_x_pos = transform.local_position.x * 13 + parent_pos.x;
            Debug.Log(transform.local_position.ToString());
            Debug.Log("Start x pos : " + start_x_pos);
            final_x_pos = Mathf.Round((Mathf.Round(transform.local_position.x / 2) * 2 + 1) * 13 + parent_pos.x);
            Debug.Log("Final tile pos : " + final_x_pos.ToString());
            calc_final_pos = false;
            falling = true;
        }
    }

    void OnContact()
    {
        if (rigid_body != null)
        {
            rigid_body.RemoveJoint();
            
            calc_final_pos = true;
           /* Quaternion quat = rigid_body.GetColliderQuaternion();
            BarrelManage nmana = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(nmana.restart_pos_x, nmana.restart_pos_y, nmana.restart_pos_z);

            Debug.Log("parent pos : " + parent_pos);
            start_x_pos = transform.local_position.x * 13 + parent_pos.x;
            Debug.Log(transform.local_position.ToString());
            Debug.Log("Start x pos : " + start_x_pos);
            final_x_pos = Mathf.Round((Mathf.Round(transform.local_position.x / 2)*2+1) * 13 + parent_pos.x);
            Debug.Log("Final tile pos : " + final_x_pos.ToString());*/
        }
    }
}
