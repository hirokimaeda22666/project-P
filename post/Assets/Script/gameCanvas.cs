using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class gameCanvas : MonoBehaviour {
    Text countDown_;
    Text force_;
    Text water_;
    Text start_;

    double timer;
    int timerCount;

    float water;
    float face;
    SerialHandler SeriaHandler_;

    rankingSystem rankingSystem_;

    bool yukiriCheck; 

    // Use this for initialization
    void Start () {
        countDown_ = GameObject.Find("countDown").GetComponent<Text>();
        force_ = GameObject.Find("force").GetComponent<Text>();
        water_ = GameObject.Find("water").GetComponent<Text>();
        start_ = GameObject.Find("start").GetComponent<Text>();

        countDown_.text = "";
        force_.text = "";
        water_.text = "";
        start_.text = "GAME";

        timer = 0;
        timerCount = 1;

        water = 1000;

        SeriaHandler_ = GameObject.Find("systemManager").GetComponent<SerialHandler>();
        yukiriCheck = false;

        rankingSystem_ = GetComponent<rankingSystem>();
        
    }
	
	// Update is called once per frame
	void Update () {
        timer += Time.deltaTime * 1;
        if (timer > timerCount && timerCount<=2)
        {
            switch (timerCount) {
                case 1: start_.text = "START";break;
                case 2:
                    start_.text = "";
                    GameObject startPanel = GameObject.Find("startPanel");
                    startPanel.SetActive(false);
                    countDown_.text = "15";
                    force_.text = "force : 0";
                    water_.text = "残り : 1000";
                    timer = 0;
                    break;
            }
            timerCount++;
        }
        if (timerCount == 3) {

            float force= SeriaHandler_.getForce();

            if (force > 20) water -= force;

            countDown_.text = (15-(int)timer).ToString();
            water_.text = "残り : "+(int)water;
            force_.text = "湯切り力 : " + (int)force;

            if (water<=0)
            {                
                water_.text = "残り : 0";
                force_.text = "湯切り力 : 0";
                timerCount++;

                systemManager_.setScore(15-(int)timer);

                rankingSystem_.rankingUpdate(systemManager_.getScore());

                SceneManager.LoadScene("resultScreen");
                return;
            }

            
            if ((15 - (int)timer)<=0)
            {
                timerCount++;

                systemManager_.setScore(0);

                rankingSystem_.rankingUpdate(systemManager_.getScore());

                SceneManager.LoadScene("gameOverScreen");
                return;
            }
            
        }

    }
}
