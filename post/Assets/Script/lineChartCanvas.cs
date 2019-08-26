using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.IO;

public class lineChartCanvas : MonoBehaviour {

    bool saveCheck;

    public GameObject save;
    Text save_;

    private static List<float> forceChartLine_ = new List<float>();

    public GameObject maneger_;
    SerialHandler SerialHandler_;

    // Use this for initialization
    void Start () {
        save_ = save.GetComponent<Text>();

        save_.text = "stop_save";

        saveCheck = false;

        SerialHandler_ = maneger_.GetComponent<SerialHandler>();
	}
	
	// Update is called once per frame
	void Update () {
        if (saveCheck)
        {
            float force = SerialHandler_.getForce();
            save_.text = "run_save"+"\n"+"list : "+ forceChartLine_.Count;
            forceChartLine_.Add(force);
        }
        else
        {
                save_.text = "stop_save";            
        }
	}
    public void forceChartLineSave()
    {

        string unitText = "";
        for (int i = 0; i < forceChartLine_.Count; i++)
        {
            unitText += forceChartLine_[i];
            if (i < forceChartLine_.Count - 1) unitText += "\n";
        }

        var path = Application.streamingAssetsPath + "/forceChartLine_.txt";

        StreamWriter output;
        output = new StreamWriter(path, false);
        //output = new StreamWriter(Application.dataPath + "/Resources/rankingContent/rankingScore.txt", false);
        output.Write(unitText);
        output.Flush();
        output.Close();
        Debug.Log("forceChartLine_Save");
    }

    public void setSave()
    {
        saveCheck=!saveCheck;
        if (!saveCheck)
        {
            forceChartLineSave();
            Debug.Log("count : "+forceChartLine_.Count);
            forceChartLine_.Clear();
            Debug.Log("count : "+forceChartLine_.Count);
        }
        
    }
   
}
