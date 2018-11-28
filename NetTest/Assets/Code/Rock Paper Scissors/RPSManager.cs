using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RPSManager : MonoBehaviour
{
    public RPSPlayer player;
    public RPSPlayer opponent;
    public Text countdownText;

    bool selectionStage = false;

    int countdown = 0;

    // Start is called before the first frame update
    void Start()
    {
        countdownText.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if(selectionStage && player.turnComplete && opponent.turnComplete)
        {
            endSelectionStage();
        }
        else if(!selectionStage)
        {
            if (Input.GetKeyDown(KeyCode.Space))
                beginTurn();
        }
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
        Debug.Log(player.currentAttack.type + " V.S " + opponent.currentAttack.type);
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
}
