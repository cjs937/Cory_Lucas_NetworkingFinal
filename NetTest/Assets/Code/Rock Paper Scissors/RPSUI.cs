using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RPSUI : MonoBehaviour
{
    public Text waitingText;
    public Text gameOverUI;
    public Text youWinText;
    public Text youLoseText;

    public Image[] playerAttackImages;
    public Image[] opponentAttackImages;

    Image playerAttackOut;
    Image opponentAttackOut;
    // Start is called before the first frame update
    void Start()
    {
        waitingText.gameObject.SetActive(false);
        gameOverUI.gameObject.SetActive(false);
        youWinText.gameObject.SetActive(false);
        youLoseText.gameObject.SetActive(false);
    }

    public void showAttacks(RPSType _playerAttack, RPSType _opponentAttack)
    {
        playerAttackOut = playerAttackImages[(int)_playerAttack];
        opponentAttackOut = opponentAttackImages[(int)_opponentAttack];

        playerAttackOut.gameObject.SetActive(true);
        opponentAttackOut.gameObject.SetActive(true);


    }

    public void hideAttacks()
    {
        if (playerAttackOut)
        {
            playerAttackOut.gameObject.SetActive(false);

            playerAttackOut = null;
        }

        if (opponentAttackOut)
        {
            opponentAttackOut.gameObject.SetActive(false);

            opponentAttackOut = null;
        }
    }

}
