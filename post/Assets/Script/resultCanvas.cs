using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class resultCanvas : MonoBehaviour {

    Text countDonw_;
    Text score_;

    double timer;
    int timerCount;

    //rankingSystem rankingSystem_;

    // Use this for initialization
    void Start () {
        countDonw_ = GameObject.Find("countDown").GetComponent<Text>();
        score_ = GameObject.Find("score").GetComponent<Text>();

        score_.text = "score : "+systemManager_.getScore().ToString();

        //rankingSystem_ = GetComponent<rankingSystem>();
    }
	
	// Update is called once per frame
	void Update () {
        timer += Time.deltaTime * 1;

        countDonw_.text = ((5 - (int)timer).ToString());

        if((5 - (int)timer) <= 0)
        {
           // rankingSystem_.rankingUpdate(systemManager_.getScore());
            SceneManager.LoadScene("rankingScreen");
            return;
        }
    }
}
