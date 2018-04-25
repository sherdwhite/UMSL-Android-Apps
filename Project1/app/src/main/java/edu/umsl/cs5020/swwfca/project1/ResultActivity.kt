package edu.umsl.cs5020.swwfca.project1

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.TextView



class ResultActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_result)

        val highScore = intent.extras!!.getInt("highScore")
        val score = intent.extras!!.getInt("score")
        val lastPress = intent.extras!!.getInt("lastPress")
        val correctColor = intent.extras!!.getInt("correctColor")
        Log.e("GameActivity", "The difficulty is $score")
        Log.e("GameActivity", "The High Score is $highScore")
        Log.e("GameActivity", "User pressed $lastPress")
        Log.e("GameActivity", "The correct color was $correctColor")
        var colorPressed: String = "Empty"
        var correctColorStr: String = "Empty"
        when(lastPress){
            0 -> colorPressed = "Green"
            1 -> colorPressed = "Red"
            2 -> colorPressed = "Yellow"
            3 -> colorPressed = "Blue"
        }
        when(correctColor){
            0 -> correctColorStr = "Green"
            1 -> correctColorStr = "Red"
            2 -> correctColorStr = "Yellow"
            3 -> correctColorStr = "Blue"
        }
        val resultText: TextView = findViewById(R.id.game_result)
        resultText.text = "Your score was $score.  The High Score is $highScore.  You pressed " +
                "$colorPressed.  The correct next color was $correctColorStr!"
    }

    fun returnMain(view: View){
        Log.e("GameStatus", "Game Started!")
        val mainIntent = Intent(this, GameActivity::class.java)
        startActivity(mainIntent)
        finish()
    }

    fun highScore(view: View){
        Log.e("GameStatus", "High Score Viewed!")
        val highScoreIntent = Intent(this, HighScoreActivity::class.java)
        val highScore = intent.extras!!.getInt("highScore")
        highScoreIntent.putExtra("highScore", highScore)
        startActivity(highScoreIntent)
    }
}

