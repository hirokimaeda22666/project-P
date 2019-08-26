using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class gameSystem : MonoBehaviour {

    SerialHandler SeriaHandler_;

	// Use this for initialization
	void Start () {
        SeriaHandler_ = GameObject.Find("systemManager").GetComponent<SerialHandler>();
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
