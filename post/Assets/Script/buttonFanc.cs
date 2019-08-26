using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class buttonFanc : MonoBehaviour {

    const int NONE = 0;
    const int NOTE = 1;
    const int COM = 2;

    systemCanvas systemCanvas_;

    private int fanc;

    private void Start()
    {
        systemCanvas_ = GameObject.Find("Canvas").GetComponent<systemCanvas>();
        fanc = NONE;
    }

    public void rankingReset()
    {
       
        systemCanvas_.setNote("Are you run \"rankingReset\" ?");
        setFanc(NOTE);
    }

    public void NO()
    {
        setFanc(NONE);
        systemCanvas_.setNone();
    }

    public void OK()
    {
        switch (fanc)
        {
            case NONE:
                systemCanvas_.setNone();
                break;
            case NOTE:
                rankingSystem rankingSystem_ = GameObject.Find("systemManager").GetComponent<rankingSystem>();
                rankingSystem_.dateResetForDebug();
                systemCanvas_.setNote("reset");
                fanc = NONE;
                break;
            case COM:
                systemCanvas_.setNote("sorry this fanc no run");
                fanc = NONE;
                break;
        }
    }

    public void setFanc(int f)
    {
        fanc = f;
    }
}
