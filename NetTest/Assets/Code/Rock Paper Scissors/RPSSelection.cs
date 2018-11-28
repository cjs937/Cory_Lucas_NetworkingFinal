using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RPSSelection : MonoBehaviour
{
    public RPSAttack[] imagesInOrder;
    public bool player1;

    [HideInInspector]
    public int currentSelection = 0;

    // Update is called once per frame
    void Update()
    {
        if(player1)
        {
            if (Input.GetKeyDown(KeyCode.A))
            {
                incrementSelection(-1);
            }
            else if (Input.GetKeyDown(KeyCode.D))
                incrementSelection(1);
        }
        else
        {
            if (Input.GetKeyDown(KeyCode.LeftArrow))
                incrementSelection(-1);            
            else if (Input.GetKeyDown(KeyCode.RightArrow))
                incrementSelection(1);
        }

        imagesInOrder[currentSelection].renderer.color = Color.yellow;
    }


    void incrementSelection(int _direction)
    {
        int newSelection = currentSelection + _direction;

        if(newSelection >= imagesInOrder.Length)
        {
            newSelection = 0;
        }
        else if(newSelection < 0)
        {
            newSelection = imagesInOrder.Length - 1;
        }

        imagesInOrder[currentSelection].renderer.color = Color.white;
        currentSelection = newSelection;
    }


    public void resetSelection()
    {
        imagesInOrder[currentSelection].renderer.color = Color.white;
        currentSelection = 0;
    }

    public RPSAttack getSelection()
    {
        return imagesInOrder[currentSelection];
    }
}
