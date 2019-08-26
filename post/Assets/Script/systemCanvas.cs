using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class systemCanvas : MonoBehaviour
{

    const int NONE = 0;
    const int NOTE = 1;
    const int COM = 2;

    Text note_;
    InputField comNumber;

    buttonFanc buttonFanc_;

    // Use this for initialization
    void Start()
    {
        note_ = GameObject.Find("note").GetComponent<Text>();
        comNumber = GameObject.Find("inM5com").GetComponent<InputField>();
        buttonFanc_ = GameObject.Find("button").GetComponent<buttonFanc>(); ;
    }
    public void setComNumber()
    {
        string comFiled = comNumber.text;
        if (comFiled == "")
        {
            setNote("com Blank :(");
        }
        else
        {
            setNote("COM change\n" + comFiled + "  ?");
            buttonFanc_.setFanc(COM);
        }
    }

    public void setNote(string t)
    {
        note_.text = t;
    }

    public void setNone()
    {
        note_.text = "a note of caution \nnone lol";
    }
}
