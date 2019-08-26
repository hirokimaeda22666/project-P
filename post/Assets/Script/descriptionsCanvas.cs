using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class descriptionsCanvas : MonoBehaviour {

    Text countDonw_;
    Text description_;

    double timer;
    int timerCount;

    string text;
    //頂点に輝くのは一人、
    string[] descriptions = new string[6] {"湯切りの頂", "頂点に輝くのは一人", "参加者に課された条件は2つ","1 : 麺を落とすな", "2 : 15秒の制限時間", "さぁ、全力で振れ"};
    
    // Use this for initialization
    void Start () {
        countDonw_ = GameObject.Find("countDown").GetComponent<Text>();
        description_ = GameObject.Find("description").GetComponent<Text>();

        text = descriptions[0];
        description_.text =text;
        
        timer = 0;
        timerCount = 1;
    }
	
	// Update is called once per frame
	void Update () {
        timer += Time.deltaTime * 1;
        countDonw_.text = ((descriptions.Length) * 2-(int)timer).ToString();

        if (timer > timerCount*2)
        {
            if ((descriptions.Length)*2 <= timer)
            {
                SceneManager.LoadScene("gameScreen");
                return;
            }
            text += "\n"+descriptions[timerCount];
            description_.text = text;
            timerCount++;
        }

    }
}
