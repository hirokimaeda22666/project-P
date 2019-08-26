using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Example : MonoBehaviour
{

    public DD_DataDiagram diagram;
    private GameObject line;
    private float time;

    SerialHandler SerialHandler_;

    void Start()
    {
        SerialHandler_ = GameObject.Find("systemManager").GetComponent<SerialHandler>();

        line = diagram.AddLine("force", Color.yellow);

        time = 0;
    }

    // Update is called once per frame
    void Update()
    {

        float force = SerialHandler_.getForce();

        
        diagram.InputPoint(line, new Vector2(0.1f, force*0.1f));
    }
}
