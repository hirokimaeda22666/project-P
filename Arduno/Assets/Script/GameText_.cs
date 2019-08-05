using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;


public class GameText_ : MonoBehaviour
{
    rankingSystem rankingSysytem_;

    Text AllParticipantsText_;
    Text RankingNumberText_;
    Text RankingScoreText_;

    private string nextScene;



    // Use this for initialization
    void Start()
    {        
        rankingSysytem_ = GameObject.Find("GameSystem").GetComponent<rankingSystem>();//いつもの
        AllParticipantsText_ = transform.Find("AllParticipantsText").GetComponent<Text>();//いつもの
        RankingNumberText_ = transform.Find("RankingNumberText").GetComponent<Text>();//いつもの
        RankingScoreText_ = transform.Find("RankingScoreText").GetComponent<Text>();//いつもの

        string nowSceneName = SceneManager.GetActiveScene().name;//現在のシーン名を取得、次の遷移先を指定
        if (nowSceneName== "ranking")
        {
            nextScene = "ranking_2";
        }else if (nowSceneName == "ranking_2")
        {
            nextScene = "ranking";
        }
        AllParticipantsText_.text = ("順位 : " + rankingSysytem_.getRankingNumber());//debug
        RankingNumberText_.text = ("参加者 : " + rankingSysytem_.getRankingAllParticipants());//debug
        RankingScoreText_.text = ("得点 : " + rankingSysytem_.getPlayerScore());//debug

    }

    // Update is called once per frame
    void Update()
    {
        //----------------------------新規プレイヤーのデータ反映のdebug----------------------------
        if (Input.GetKeyDown("space"))
        {
            int rnd = Random.Range(0, 1000);
            rankingSysytem_.rankingUpdate(rnd);//順位、参加者、得点の更新
            AllParticipantsText_.text=("順位 : " + rankingSysytem_.getRankingNumber());//debug
            RankingNumberText_.text=("参加者 : " + rankingSysytem_.getRankingAllParticipants());//debug
            RankingScoreText_.text = ("得点 : "+ rankingSysytem_.getPlayerScore());//debug

            Debug.Log("rankingUpdate");
        }
        //----------------------------新規プレイヤーのデータ反映のdebug----------------------------


        //----------------------------scoreDateの保存のdebug----------------------------
        if (Input.GetKeyDown(KeyCode.S))
        {
            rankingSysytem_.rankingDateSave();//テキストとしてデータの保存する関数

            Debug.Log("rankingDateSava");
        }
        //----------------------------scoreDateの保存のdebug----------------------------


        //----------------------------画面遷移のdebug----------------------------
        if (Input.GetKeyDown("return"))
        {
            SceneManager.LoadScene(nextScene);
        }
        //----------------------------画面遷移のdebug----------------------------


        //----------------------------ゲーム終了の一連処理へ----------------------------
        if (Input.GetKey(KeyCode.Escape))
        {
            rankingSysytem_.rankingDateSave();//テキストとしてデータの保存する関数
            Debug.Log("rankingDateSave & gameFinish");
            rankingSysytem_.Quit();//ゲームを終了させる関数
        }
        //----------------------------ゲーム終了の一連処理へ----------------------------


        //----------------------------ScoreDateReset for Debug----------------------------
        if (Input.GetKey(KeyCode.R))
        {
            rankingSysytem_.dateResetForDebug();
        }
        //----------------------------ScoreDateReset for Debug----------------------------
    }
}
