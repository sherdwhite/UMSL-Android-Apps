package edu.umsl.cs5020.swwfca.project1

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.ArrayAdapter
import kotlinx.android.synthetic.main.activity_main.*
import android.widget.Spinner
import java.io.InputStreamReader
import org.json.JSONException
import org.json.JSONObject



class MainActivity : Activity() {
    var scoreText: String = ""
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val difficulty = findViewById<Spinner>(R.id.difficulty_spinner)
        // Create an ArrayAdapter using the string array and a default spinner layout
        val adapter = ArrayAdapter.createFromResource(this,
                R.array.difficulty_choices, android.R.layout.simple_spinner_item)
        // Specify the layout to use when the list of choices appears
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        // Apply the adapter to the spinner
        difficulty.adapter = adapter

        try {
            scoreText = applicationContext.assets.open("simon.txt").bufferedReader().use {
                it.readText()
            }
            Log.e("MainActivity", "Reading scores from internal storage.")
        }catch (ex:Exception){
            Log.e("MainActivity", "Unable to open file for reading.")
        }
    }

    fun startGame(view: View){
        Log.e("GameStatus", "Game Started!")
//        val difficulty = difficulty_spinner.selectedItemPosition
//        Log.e("GameActivity", "The difficulty is $difficulty")
        // Setting up bogus high score for later retrieval and usage
        val gameIntent = Intent(this, GameActivity::class.java)
        gameIntent.putExtra("difficulty", difficulty_spinner.selectedItemPosition)
        gameIntent.putExtra("highScores", scoreText)
        startActivity(gameIntent)
    }

    fun highScore(view: View){
        Log.e("GameStatus", "High Score Viewed!")
        val highScoreIntent = Intent(this, HighScoreActivity::class.java)
        highScoreIntent.putExtra("highScore", 0)
        startActivity(highScoreIntent)
    }
}
