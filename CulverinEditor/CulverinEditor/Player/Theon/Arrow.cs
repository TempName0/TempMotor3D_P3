﻿using CulverinEditor;
using CulverinEditor.Debug;


public class Arrow : CulverinBehaviour
{
    public float damage = 20;
    public Vector3 speed = Vector3.Zero;
    public bool collision;
    CompRigidBody rb;
    private bool destroyed = false;

    void Start()
    {
        rb = GetComponent<CompRigidBody>();
        destroyed = false;
        Shoot();
        collision = true;
        damage = 10.0f;
    }

    public void Shoot()
    {
        rb.UnLockTransform();
        Vector3 force = new Vector3(0, 5, 0);
        rb.ApplyImpulse(force + speed*100); // Forward impulse
        rb.ApplyTorqueForce(speed*40); // Fall force
    }

    void Update()
    {
        if (GetComponent<Transform>().local_position.y < -5 && destroyed == false)
        {
            Destroy(gameObject);
            destroyed = true;
        }

        //if(rb.LockedTransform())
        //{
        //    Destroy(float delay)
        //}
    }

    void OnContact()
    {
        Debug.Log("[yellow]Contact Start");
        GameObject collided_obj = GetComponent<CompCollider>().GetCollidedObject();

        // DAMAGE ---
        Debug.Log("[error] Collided");
        if (collided_obj != null && destroyed == false)
        {
            //Lock transform to avoid trespassing more than one collider
            rb.LockTransform();

            Debug.Log("[error] OnContact");
            Debug.Log("[error]" + collided_obj.GetName());

            // Check the specific enemy in front of you and apply dmg or call object OnContact
            EnemiesManager enemy_manager = GetLinkedObject("player_enemies_manager").GetComponent<EnemiesManager>();
            if (enemy_manager.IsEnemy(collided_obj))
            {
                Debug.Log("[error] Enemy Found");
                enemy_manager.ApplyDamage(collided_obj, damage);
                Debug.Log("[error] Apply Damage");

            }
            else
            {
                CompCollider obj_col = collided_obj.GetComponent<CompCollider>();
                if (obj_col != null)
                {
                    obj_col.CallOnContact();
                }

            }
        }
        if (destroyed == false)
        {
            Destroy(gameObject);
            destroyed = true;
        }
    }
}

