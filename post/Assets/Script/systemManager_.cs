using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class systemManager_ : MonoBehaviour {

    const int SYSTME=1;
    const int START= 2;
    const int DESCRIPTION = 3;
    const int GAME = 4;
    const int GAMEOVER = 5;
    const int RANKING = 6;
    const int RESULT = 7;
    const int ELSE = 0;

    private int nowScreen;

    private static int score;

    int startCount;

    SerialHandler SerialHandler_;

    // Use this for initialization
    void Start () {
        startCount = 0;
        DontDestroyOnLoad(this);
        SerialHandler_ = GetComponent<SerialHandler>();

	}
	
	// Update is called once per frame
	void Update () {

        /*if (startCount == 0)
        {
            setScreen(SceneManager.GetActiveScene().name);
            startCount++;
        }
        */
        if (Input.GetKeyDown(KeyCode.Return))
        {
            loadScene();
        }

        if (Input.GetMouseButtonDown(1))
        {
            Debug.Log("serial close");
            SerialHandler_.Close();
        }

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            Quit();
        }
	}
    public void loadScene()
    {
        setScreen(SceneManager.GetActiveScene().name);
        switch (nowScreen)
        {
            case SYSTME: SceneManager.LoadScene("startScreen"); break;
            case START: SceneManager.LoadScene("descriptionScreen"); break;
            case DESCRIPTION: SceneManager.LoadScene("gameScreen"); break;
            case GAME: SceneManager.LoadScene("resultScreen"); break;
            case GAMEOVER: SceneManager.LoadScene("resultScreen"); break;
            case RESULT: SceneManager.LoadScene("rankingScreen"); break;
            case RANKING: SceneManager.LoadScene("startScreen"); break;
            case 0: SceneManager.LoadScene("startScreen"); break;
        }
        startCount = 0;
    }

   void setScreen(string name)
    {
        if (name == "systemSetting") nowScreen = SYSTME;
        else if (name == "startScreen") nowScreen = START;
        else if (name == "descriptionScreen") nowScreen = DESCRIPTION;
        else if (name == "gameScreen") nowScreen = GAME;
        else if (name == "gameOverScreen") nowScreen = GAMEOVER;
        else if (name == "resultScreen") nowScreen = RESULT;
        else if (name == "rankinfScreen") nowScreen = RANKING;
        else nowScreen=ELSE;
    }

    public static void setScore(int s)
    {
        score = s;
    }

    public static int getScore()
    {
        return score;
    }
    public static void Quit()
    {
#if UNITY_STANDALONE
        Application.Quit();
#endif

#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#endif
    }
}
