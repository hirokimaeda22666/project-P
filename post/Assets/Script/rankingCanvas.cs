using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class rankingCanvas : MonoBehaviour {

    Text countDonw_;
    Text ranking_;
    Text rankingAllParticipants_;
    Text score_;


    double timer;
    int timerCount;

    rankingSystem rankingSystem_;

    // Use this for initialization
    void Start()
    {
        countDonw_ = GameObject.Find("countDown").GetComponent<Text>();
        ranking_ = GameObject.Find("ranking").GetComponent<Text>();
        rankingAllParticipants_= GameObject.Find("rankingAllParticipants").GetComponent<Text>();
        score_ = GameObject.Find("score").GetComponent<Text>();

        rankingSystem_ = GetComponent<rankingSystem>();

        int number = rankingSystem_.getRankingNumber();
        int Anumber = rankingSystem_.getRankingAllParticipants();

        rankingAllParticipants_.text = "Anumber : "+Anumber;
        score_.text = "score : " + systemManager_.getScore();
        ranking_.text = "number : "+number;
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime * 1;

        countDonw_.text = ((5 - (int)timer).ToString());
       
        if ((5 - (int)timer) <= 0)
        {
            rankingSystem_.rankingDateSave();
            SceneManager.LoadScene("startScreen");
            return;
        }
    }
}
