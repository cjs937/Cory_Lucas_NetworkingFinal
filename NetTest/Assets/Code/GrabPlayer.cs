using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GrabPlayer : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        if(SceneController.localPlayer)
            SceneController.localPlayer.transform.SetParent(transform);   
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
