using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum RPSType
{
    ROCK,
    PAPER,
    SCISSORS
}
public class RPSAttack : MonoBehaviour
{
    public RPSType type;
    public AudioClip hitSound;
    [HideInInspector]
    public SpriteRenderer renderer;

    // Start is called before the first frame update
    void Start()
    {
        renderer = GetComponent<SpriteRenderer>();   
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
