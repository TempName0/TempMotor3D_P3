﻿using CulverinEditor;
using CulverinEditor.Debug;


public class FireSound : CulverinBehaviour
{
    public float limit_time = 5.0f;
    public float actual_time = 0.0f;
    public bool is_firebreath = false;
    private bool stop = true;

    void Start()
    {
        actual_time = 0.0f;         
       
    }

    void Update()
    {
        actual_time += Time.deltaTime;

        if (is_firebreath && actual_time >= limit_time && stop) 
        {
            GetComponent<CompAudio>().PlayEvent("DaenerysFireballImpact");
            stop = false;
        }
    }

  

}