using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RPSPlayer : MonoBehaviour
{
    public int currentLives = 3;
    public RPSSelection selector;
    public bool turnComplete = true;

    public RPSAttack rockAttackPrefab;
    public RPSAttack paperAttackPrefab;
    public RPSAttack scissorsAttackPrefab;

    [HideInInspector]
    public RPSAttack currentAttack;

    private void Start()
    {
        if(selector)
            selector.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if(!turnComplete)
        {
            if (Input.GetKeyDown(KeyCode.Return))
            {
                selectAttack();
            }
        }
    }

    public void beginTurn()
    {
        turnComplete = false;
        selector.gameObject.SetActive(true);
        selector.resetSelection();
    }

    public void selectAttack()
    {
        currentAttack = selector.getSelection();
        endTurn();
    }

    public void endTurn()
    {
        turnComplete = true;
        selector.gameObject.SetActive(false);
    }
}
