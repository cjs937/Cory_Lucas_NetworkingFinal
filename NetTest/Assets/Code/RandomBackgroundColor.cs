using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RandomBackgroundColor : MonoBehaviour
{
    public float colorChangeTime;
    Camera camera;
    float lerpStartTime;
    public Color lerpStartColor;
    public Color targetColor;
    // Use this for initialization
    void Start()
    {
        camera = GetComponent<Camera>();
        startNewLerp();
    }

    // Update is called once per frame
    void Update()
    {
        float currentLerpTime = Time.time - lerpStartTime;
        float fracJourney = currentLerpTime / colorChangeTime;

        camera.backgroundColor = Color.Lerp(lerpStartColor, targetColor, fracJourney);

        if (fracJourney >= colorChangeTime)
            startNewLerp();
    }

    void startNewLerp()
    {
        //float randR = Random.Range(0.0f, 256.0f);
        //float randG = Random.Range(0.0f, 256.0f);
        //float randB = Random.Range(0.0f, 256.0f);

        lerpStartColor = camera.backgroundColor;

        targetColor = Random.ColorHSV();//new Color(randR, randG, randB);

        lerpStartTime = Time.time;
    }
}