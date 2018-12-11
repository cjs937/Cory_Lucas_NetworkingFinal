using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RPSManager : MonoBehaviour
{
    public RPSPlayer player;
    public RPSPlayer opponent;
    public Text countdownText;

    public static Guid opponentGuid;

    bool selectionStage = false;

    int countdown = 0;

    // Start is called before the first frame update
    void Start()
    {
        countdownText.gameObject.SetActive(false);

        beginTurn();
    }

    // Update is called once per frame
    void Update()
    {
        if(selectionStage && player.turnComplete && opponent.turnComplete)
        {
            endSelectionStage();
        }
        //else if(!selectionStage)
        //{
        //    if (Input.GetKeyDown(KeyCode.Space))
        //        beginTurn();
        //}
    }

    public void beginTurn()
    {
        player.beginTurn();
        opponent.beginTurn();
        selectionStage = true;
        countdown = 20;
        countdownText.gameObject.SetActive(true);
        countdownText.text = countdown.ToString();

        StartCoroutine(timerCountdown());
    }

    void endSelectionStage()
    {
        selectionStage = false;

        countdownText.gameObject.SetActive(false);
        fireAttacks();
    }

    void fireAttacks()
    {
        //TODO:
        //send local player attack data to server
    }

    public void recieveRoundWinner(Guid _winnerID)
    {
        //TODO:
        //increment/decrement win counts
        //if any player's lives = 0 end game and send back to main scene
    }

    IEnumerator timerCountdown()
    {
        while (selectionStage && countdown > 0)
        {
            yield return new WaitForSeconds(1f);

            countdown--;

            countdownText.text = countdown.ToString();

            if (countdown == 0)
            {
                player.selectAttack();
                opponent.selectAttack();

                endSelectionStage();

                yield break;
            }
        }
    }

    RPSPlayer getRPSWinner()
    {
        if(player.currentAttack.type == RPSType.ROCK)
        {
            if(opponent.currentAttack.type == RPSType.ROCK)
            {
                return null;
            }
            else if(opponent.currentAttack.type == RPSType.SCISSORS)
            {
                return player;
            }
            else
            {
                return opponent;
            }
        }
        else if(player.currentAttack.type == RPSType.SCISSORS)
        {
            if (opponent.currentAttack.type == RPSType.ROCK)
            {
                return opponent;
            }
            else if (opponent.currentAttack.type == RPSType.SCISSORS)
            {
                return null;
            }
            else
            {
                return player;
            }
        }
        else // paper
        {
            if (opponent.currentAttack.type == RPSType.ROCK)
            {
                return player;
            }
            else if (opponent.currentAttack.type == RPSType.SCISSORS)
            {
                return opponent;
            }
            else
            {
                return null;
            }
        }
    }
}
