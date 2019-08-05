using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Threading;

public class SerialHandler : MonoBehaviour
{
    public string portName ;
    public int baudRate = 9600;

    private SerialPort serialPort_;
    private Thread thread_;
    private bool isRunning_ = false;

    private string message_;
    private bool isNewMessageReceived_ = false;

    int count;

    private float force;
    private float prece;

    void Start()
    {

        Debug.LogWarning("Start");
        count = 0;

        force = 0;
        prece = 0;

        Open();
    }

    // Update is called once per frame
    void Update()
    {

        // Debug.LogWarning("Serial-Update");
        if (isNewMessageReceived_)
        {
            OnDateReceived(message_);
        }

        if (Input.GetMouseButtonDown(0))
        {
            Close();
        }

    }



    void OnDestoroy()
    {
        Debug.LogWarning("OnDestoroy");
        Close();
    }

    private void Open()
    {

        serialPort_ = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One);
        serialPort_.Open();

        isRunning_ = true;

        thread_ = new Thread(Read);
        thread_.Start();
    }

    private void Read()
    {
        Debug.LogWarning("Read1");
        while (isRunning_ && serialPort_ != null && serialPort_.IsOpen)
        {
            //  Debug.LogWarning("Read2");
            try
            {
                message_ = serialPort_.ReadLine();
                count++;
                // Debug.LogWarning(message_+" : "+count);
                setSenserSetting(message_);//オリジナル
                isNewMessageReceived_ = true;
            }
            catch (System.Exception e)
            {
                Debug.LogWarning(e.Message);
            }
        }
    }

    public void Write(string message)
    {
        try
        {
            serialPort_.Write(message);
        }
        catch (System.Exception e)
        {
            Debug.LogWarning(e.Message);
        }
    }

    private void Close()
    {
        isRunning_ = false;
        if (thread_ != null && thread_.IsAlive)
        {
            thread_.Join();
            Debug.LogWarning("Clsoe-Join");
        }
        if (serialPort_ != null && serialPort_.IsOpen)
        {
            serialPort_.Close();
            serialPort_.Dispose();
            Debug.LogWarning("Close");
        }
    }

    void OnDateReceived(string message)
    {
        // Debug.LogWarning("OndateReceivd1");
        var date = message.Split(new string[] { "\t" }, System.StringSplitOptions.None);
        if (date.Length < 2) return;

        try { }
        catch (System.Exception e)
        {
            Debug.LogWarning(e.Message);
        }
    }

    void setSenserSetting(string rawCount)
    {
        var splitText = rawCount.Split(new string[] { "," }, System.StringSplitOptions.None);
        
        Debug.Log(splitText.Length);
        //if (splitText.Length < 4) { return; }
        prece = float.Parse(splitText[0]);
       
        force = float.Parse(splitText[1]);
        
    }

    public float getForce()
    {
        return force;
    }
    public float getPrece()
    {
        return prece;
    }
}