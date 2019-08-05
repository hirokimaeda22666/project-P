using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class senserText : MonoBehaviour
{
    SerialHandler SerialHandler_;

    Text ForceText_;
    Text PreceText_;

    // Use this for initialization
    void Start()
    {
        SerialHandler_ = GameObject.Find("SerialHandler").GetComponent<SerialHandler>();

        ForceText_ = transform.Find("ForceText").GetComponent<Text>();
        PreceText_ = transform.Find("PreceText").GetComponent<Text>();
    }

    // Update is called once per frame
    void Update()
    {
        float force_ = SerialHandler_.getForce();
        float prece_ = SerialHandler_.getPrece();

        ForceText_.text = ("force = " + force_.ToString());
        if (force_ >= 30) Debug.Log("forceOver30 : " + force_);
        PreceText_.text = ("prece = " + prece_.ToString());


    }
}
