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
    public RPSUI rpsUI;

    public static Guid opponentGuid;

    bool selectionStage = false;

    int countdown = 0;
    static public RPSManager instance;
    public bool gameOver = false;
    

    private void Awake()
    {
        if (RPSManager.instance && RPSManager.instance != this)
            Destroy(gameObject);
        else
            RPSManager.instance = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        countdownText.gameObject.SetActive(false);

        beginTurn();
    }

    // Update is called once per frame
    void Update()
    {
        if(selectionStage && player.turnComplete)
        {
            endSelectionStage();
        }
    }

    public void beginTurn()
    {
        player.beginTurn();
        //opponent.beginTurn();
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
        rpsUI.waitingText.gameObject.SetActive(true);
        fireAttacks();
    }

    void fireAttacks()
    {
        Debug.Log("Firing Attacks");
        NetworkManager.SendCombatInfo(SceneController.localPlayer, player.currentAttack.type, opponentGuid);
    }

    public void recieveRoundWinner(Guid _winnerID, bool isDraw)
    {
        Debug.Log("Round Winner Recieved");
        if (gameOver)
            return;

        rpsUI.waitingText.gameObject.SetActive(false);

        if (!isDraw)
        {
            if (_winnerID == SceneController.localPlayer.identifier)
            {
                opponent.currentLives--;

                if (opponent.currentLives <= 0)
                    endGame(false);

                displayAttacks(isDraw, player.currentAttack.type, true);
            }
            else
            {
                player.currentLives--;

                if (player.currentLives <= 0)
                    endGame(false);

                displayAttacks(isDraw, player.currentAttack.type, false);
            }
        }

        Debug.Log("Player: " + player.currentLives + " Opponent: " + opponent.currentLives);

        //if (!gameOver)
        //    beginTurn();
    }

    public void endGame(bool _playerWins)
    {
        gameOver = true;

        rpsUI.gameOverUI.gameObject.SetActive(true);

        if (_playerWins)
            rpsUI.youWinText.gameObject.SetActive(true);
        else
            rpsUI.youLoseText.gameObject.SetActive(true);
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

    IEnumerator displayAttacks(bool _isDraw, RPSType _playerAttack, bool playerWin)//_winningAttack)
    {
        RPSType opponentAttack;

        if (_isDraw)
        {
            opponentAttack = _playerAttack;
        }
        else
        {
            if (_playerAttack == RPSType.ROCK)
            {
                if (playerWin)
                    opponentAttack = RPSType.SCISSORS;
                else
                    opponentAttack = RPSType.PAPER;
            }
            else if (_playerAttack == RPSType.PAPER)
            {
                if (playerWin)
                    opponentAttack = RPSType.ROCK;
                else
                    opponentAttack = RPSType.SCISSORS;
            }
            else
            {
                if (playerWin)
                    opponentAttack = RPSType.PAPER;
                else
                    opponentAttack = RPSType.ROCK;
            }
        }

        rpsUI.showAttacks(_playerAttack, opponentAttack);

        yield return new WaitForSeconds(2.0f);

        rpsUI.hideAttacks();

        if(!gameOver)
            beginTurn();
    }

    //RPSPlayer getRPSWinner()
    //{
    //    if(player.currentAttack.type == RPSType.ROCK)
    //    {
    //        if(opponent.currentAttack.type == RPSType.ROCK)
    //        {
    //            return null;
    //        }
    //        else if(opponent.currentAttack.type == RPSType.SCISSORS)
    //        {
    //            return player;
    //        }
    //        else
    //        {
    //            return opponent;
    //        }
    //    }
    //    else if(player.currentAttack.type == RPSType.SCISSORS)
    //    {
    //        if (opponent.currentAttack.type == RPSType.ROCK)
    //        {
    //            return opponent;
    //        }
    //        else if (opponent.currentAttack.type == RPSType.SCISSORS)
    //        {
    //            return null;
    //        }
    //        else
    //        {
    //            return player;
    //        }
    //    }
    //    else // paper
    //    {
    //        if (opponent.currentAttack.type == RPSType.ROCK)
    //        {
    //            return player;
    //        }
    //        else if (opponent.currentAttack.type == RPSType.SCISSORS)
    //        {
    //            return opponent;
    //        }
    //        else
    //        {
    //            return null;
    //        }
    //    }
    //}
}
