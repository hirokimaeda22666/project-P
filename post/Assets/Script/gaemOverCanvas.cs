using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class gaemOverCanvas : MonoBehaviour {
    Text countDonw_;

    double timer;
    int timerCount;

    // Use this for initialization
    void Start()
    {
        countDonw_ = GameObject.Find("countDown").GetComponent<Text>();
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime * 1;

        countDonw_.text = ((5 - (int)timer).ToString());

        if ((5 - (int)timer) <= 0)
        {
            SceneManager.LoadScene("resultScreen");
            return;
        }
    }
}
