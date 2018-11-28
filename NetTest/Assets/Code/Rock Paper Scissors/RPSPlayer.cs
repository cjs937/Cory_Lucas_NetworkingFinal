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

    public bool player1; //testing purposes only
    [HideInInspector]
    public RPSAttack currentAttack;

    private void Start()
    {
        selector.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if(!turnComplete)
        {
            if (player1)
            {
                if (Input.GetKeyDown(KeyCode.LeftShift))
                {
                    selectAttack();
                }
            }
            else
            {
                if (Input.GetKeyDown(KeyCode.RightShift))
                {
                    selectAttack();
                }
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
