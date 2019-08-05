using System.Collections.Generic;
using UnityEngine;
using System.IO;

/*  rankingSystem説明
 *  なんか適当にランキングの実装を行う
 *  ゲーム起動時一回目のみtextから今までのscoreデータの読み込みを行い 
 *  <list> static float rankingOverall 変数に保存
 *  順次新規プレイのscoreをrankingOverallに保存していく
 *  この時のscoreの追加はrankingOverallにソートしていく(降順
 *  現プレイヤーのrankingはrankingOverallを確認していく事で決定する
 *  ranking画面から別の画面への遷移を挟む場合にはtextにrankingOverallを保存する
 */

/*---------------仕様書------------------------
 * とりあえず、ここを読んでおいて欲しいが
 * テキストデータとして保存と取り出しをするには、普通にリソースファイルからの呼び出しだと
 * ビルドした時に、メタデータ化かよくわからんが、とりあえずリソースファイルの中身が全部一つのファイルに固められちまうみたい
 * こうするとテキストデータの読み込みは正常にできても書き込みが難しい
 * よって別の手段でデータの読み込み囲みを行う
 * start()関数に
 * var path = Application.streamingAssetsPath + "/rankingScore.txt";
 * があって、ここでアセットの中にある [ StreamingAssets ]　のランキングデータにアクセスパスを指定している
 * 問題なのは、この [ StreamingAssets ]　ファイルと [ rankingScore.txt" ]データは、
 * このプログラムを流用するときはそっちで作らなければならないという事や、
 * まぁ、あーだこーだやれば作れるからがんば
 */
public class rankingSystem : MonoBehaviour {
    private static int startCount=0; 
    private string loadText;
    private string[] splitText;
    private static List<int> rankingScore_=new List<int>();
    private static int rankingNumber;
    private static int playerScore;

    public int testScore;

    

    public void rankingUpdate(int score)//ランキングの更新
    {
        int number = 0;
        if (rankingScore_[rankingScore_.Count - 1] > score)
        {
            rankingScore_.Add(score);
            number = rankingScore_.Count - 1;
        }
        else
        {
            for (int i = 0; i < rankingScore_.Count; i++)
            {
                if (rankingScore_[i] < score)
                {
                    rankingScore_.Insert(i, score);
                    number = i;
                    break;
                }
            }
        }
        setRankingNumber(number);
        setPlayerScore(score);
    }

    public void setRankingNumber(int number)//今プレイした順位を保存
    {
        rankingNumber = number;
    }
    public int getRankingNumber()//今プレイした順位を受け渡し
    {
        return rankingNumber+1;
    }

    public int getRankingAllParticipants()//ランキング参加者数を受け渡し
    {        
        return rankingScore_.Count;
    }

    public void rankingDateSave()
    {
        string unitText = "";
        for (int i = 0; i <rankingScore_.Count;i++)
        {
            unitText += rankingScore_[i];
            if (i < rankingScore_.Count - 1) unitText += "\n";
        }

        var path = Application.streamingAssetsPath + "/rankingScore.txt";

        StreamWriter output;
        output = new StreamWriter(path, false);
        //output = new StreamWriter(Application.dataPath + "/Resources/rankingContent/rankingScore.txt", false);
        output.Write(unitText);
        output.Flush();
        output.Close();

    }

    void setPlayerScore(int score)
    {
        playerScore = score;
    }
    public int getPlayerScore()
    {
        return playerScore;
    }

   public void Quit()
    {
#if UNITY_STANDALONE
        Application.Quit();
#endif

#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#endif
    }

    public void dateResetForDebug()
    {
        var path = Application.streamingAssetsPath + "/rankingScore.txt";

        StreamWriter output;
        output = new StreamWriter(path, false);
        output.Write("0");
        output.Flush();
        output.Close();

        Quit();

        Debug.Log("ScoreDateをリセットしました。\n強制終了します");

    }

    void Start () {

        if (startCount == 0)
        {
            /* 通常のtextファイルから読み込むか、untiy上で形式変換したtxtで読み込むかの違い、
             * txtで今までやっていたがinspector上で記憶するので連続動作は不向きだったが、
             * わんちゃんtextから読み込めばinspector上での保存はないかもしれない            
             * var a= Resources.Load("rankingContent/rankingScore")as TextAsset;
             * Debug.Log(a.text);
             * output = new StreamWriter(Application.dataPath + "/Resources/rankingContent/rankingScore.text", false);
             * output.Write(unitText);
            */
            var path = Application.streamingAssetsPath + "/rankingScore.txt";
            loadText = File.ReadAllText(path);
            splitText = loadText.Split(char.Parse("\n"));
            for (int i = 0; i < splitText.Length; i++)
            {
                rankingScore_.Add(int.Parse(splitText[i]));
            }
            
            Debug.Log("textLoadをしました。");
            startCount++;

            rankingNumber = 0;
        }else
        {
            Debug.Log("textLoadの処理を飛ばしました。");
        }
	}

	
}
