using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DelayCombat : MonoBehaviour
{
    float defaultRadius;

    // Start is called before the first frame update
    void Start()
    {
        defaultRadius = SceneController.localPlayer.collisionRadius;
        SceneController.localPlayer.collisionRadius = -1;

        StartCoroutine(combatDelay());
    }

    IEnumerator combatDelay()
    {
        yield return new WaitForSeconds(4f);

        SceneController.localPlayer.collisionRadius = defaultRadius;
    }
}
