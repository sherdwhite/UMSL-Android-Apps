package edu.umsl.cs5020.swwfca.project1

import android.animation.AnimatorSet
import android.animation.ObjectAnimator
import android.animation.ValueAnimator
import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.View
import android.widget.TextView
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_game.*
import org.json.JSONException
import org.json.JSONObject
import java.io.File
import java.io.OutputStreamWriter
import org.json.JSONArray



class GameActivity : Activity(), GameFragment.GameFragmentListener {

    private var gameModel: GameModel? = null
    private var gameFragment: GameFragment? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)
        bindButtons()

        // get the difficulty from MainActivity
        // use generic intent call, not passed name gameIntent
        val difficulty = intent.extras!!.getInt("difficulty")
        val highScores = intent.extras!!.getString("highScores")
        Log.e("GameActivity", "The difficulty is $difficulty")
        Log.e("GameActivity", "The High Score are $highScores")

        var scores: JSONObject? = null
        try {
            scores = JSONObject(highScores)
        } catch (e: JSONException) {
            Log.e("GameActivity", "Unable to create JSON object from score string.")
        }

        val scoreList: ArrayList<Int> = ArrayList()
        if (scores != null) {
            for (i in 0 until scores.length()) {
                scoreList.add(Integer.parseInt(scores.getString(i.toString())))
            }
        }

        gameModel = GameModel(scoreList)

        val manager = fragmentManager
        gameFragment = manager.findFragmentByTag("GameFragment") as? GameFragment
        if (gameFragment == null) {
            gameFragment = GameFragment()
            manager.beginTransaction()
                    .add(gameFragment, "GameFragment")
                    .commit()
        }
        gameFragment?.listener = this

        when(difficulty){
            0 -> {
                gameModel!!.mSetDelay(1000)
                Log.e("GameActivity", "Delay: 1000")
            }
            1 -> {
                gameModel!!.mSetDelay(750)
                Log.e("GameActivity", "Delay: 750")
            }
            2 -> {
                gameModel!!.mSetDelay(500)
                Log.e("GameActivity", "Delay: 500")
            }
        }

        // Starting first round
        // need to add a random color here first.

        startGame()
    }

    override fun onBackPressed() {
        finish()
    }

    private fun bindButtons() {
        green_button.setOnClickListener {
            Log.e("GameActivity", "Green was pressed!")
//            gameFragment?.buttonTriggered()
            flashButton(0)
            checkColor(0)
        }
        red_button.setOnClickListener {
            Log.e("GameActivity", "Red was pressed!")
//            gameFragment?.buttonTriggered()
            flashButton(1)
            checkColor(1)
        }
        yellow_button.setOnClickListener {
            Log.e("GameActivity", "Yellow was pressed!")
//            gameFragment?.buttonTriggered()
            flashButton(2)
            checkColor(2)
        }
        blue_button.setOnClickListener {
            Log.e("GameActivity", "Blue was pressed!")
//            gameFragment?.buttonTriggered()
            flashButton(3)
            checkColor(3)
        }
    }

    // for passing model info to the fragment
    override val sequenceSize: Int
        get() = gameModel?.mGetSize() ?: 0

    override val delay: Long
        get() = gameModel?.mGetDelay() ?: 1000
    ////////////////////////////////////////

    //need to create separate function to start game and disable user interaction
    //found infinite loop when overriding play sequence in fragment and causing buffer overflow
    private fun startGame() {
        Log.e("GameActivity", "Starting Game")
        val gameStatus: TextView = findViewById(R.id.game_status)
        gameStatus.text = "Starting Game"
        gameModel?.mAddRandomColor()  // add first color
        gameModel?.mResetIndex()
        disableUserInteraction()
//        gameFragment?.startSequence()
        Handler().postDelayed({ systemFlash() }, 1000 )  // delay to allow for GameIntent
    }

    private fun checkColor(color: Int) {
        Log.e("GameActivity", "Checking Color")
        val index = gameModel!!.mGetIndex()
        val lastValue = gameModel!!.mGetSize().minus(1)
        if((gameModel!!.mCheckColor(index, color)) && (index <= lastValue)){
            Toast.makeText(this, "Yes!", Toast.LENGTH_SHORT).show()
            gameModel?.mAddToScore()
            if(index == lastValue){
                gameModel?.mAddRandomColor()
                disableUserInteraction()
                systemFlash()
            }
            else{
                gameModel!!.mIncrementIndex()
            }
        }
        else{
//            gameFragment?.stopSequence()
            Toast.makeText(this, "No!!!!", Toast.LENGTH_SHORT).show()
            val resultIntent = Intent(this, ResultActivity::class.java)
            resultIntent.putExtra("score", gameModel?.mGetScore())
            resultIntent.putExtra("highScore", gameModel?.mGetHighScore())
            resultIntent.putExtra("lastPress", color)
            resultIntent.putExtra("correctColor", gameModel?.mColorVal(index))
            startActivity(resultIntent)
            writeToFile()
            finish()
        }
    }

    private fun systemFlash() {
        val lastValue = gameModel!!.mGetSize().minus(1)
        val delay = gameModel?.mGetDelay()
        for (index in 0..lastValue){
            val color = gameModel!!.mColorVal(index)
            Handler().postDelayed({ flashButton(color) }, delay?.times(index)!!)
            Log.e("GameActivity", "System Flashed color:$color at index:$index!")
        }
        gameModel?.mResetIndex()
        Handler().postDelayed({ enableUserInteraction() }, delay!!.times(lastValue))
    }

    fun quitGame(view: View){
        Log.e("GameStatus", "Quit Button Pressed!")
        val mainIntent = Intent(this, GameActivity::class.java)
        startActivity(mainIntent)
        finish()
    }

    // per classmate, found this animation info at https://developer.android.com/guide/topics/graphics/prop-animation.html#choreography
    private fun flashButton(clr: Int) {
        when(clr){
            0 -> {
                Log.e("GameActivity", "Flashing Green.")
                val anim = ObjectAnimator.ofFloat(green_button, "Alpha", 0f, 1f)
                anim.repeatMode = ValueAnimator.REVERSE

                val elevationAnim = ObjectAnimator.ofFloat(green_button, "Elevation", 10f)
                elevationAnim.repeatCount = 1
                elevationAnim.repeatMode = ValueAnimator.REVERSE

                val animSet = AnimatorSet()
                animSet.duration = 800
                animSet.play(elevationAnim).with(anim)
                animSet.start()
            }
            1 -> {
                val anim = ObjectAnimator.ofFloat(red_button, "Alpha", 0f, 1f)
                anim.repeatMode = ValueAnimator.REVERSE

                val elevationAnim = ObjectAnimator.ofFloat(red_button, "Elevation", 10f)
                elevationAnim.repeatCount = 1
                elevationAnim.repeatMode = ValueAnimator.REVERSE

                val animSet = AnimatorSet()
                animSet.duration = 800
                animSet.play(elevationAnim).with(anim)
                animSet.start()
            }
            2 -> {
                val anim = ObjectAnimator.ofFloat(yellow_button, "Alpha", 0f, 1f)
                anim.repeatMode = ValueAnimator.REVERSE

                val elevationAnim = ObjectAnimator.ofFloat(yellow_button, "Elevation", 10f)
                elevationAnim.repeatCount = 1
                elevationAnim.repeatMode = ValueAnimator.REVERSE

                val animSet = AnimatorSet()
                animSet.duration = 800
                animSet.play(elevationAnim).with(anim)
                animSet.start()
            }
            3 -> {
                Log.e("GameActivity", "Flashing Blue.")
                val anim = ObjectAnimator.ofFloat(blue_button, "Alpha", 0f, 1f)
                anim.repeatMode = ValueAnimator.REVERSE

                val elevationAnim = ObjectAnimator.ofFloat(blue_button, "Elevation", 10f)
                elevationAnim.repeatCount = 1
                elevationAnim.repeatMode = ValueAnimator.REVERSE

                val animSet = AnimatorSet()
                animSet.duration = 800
                animSet.play(elevationAnim).with(anim)
                animSet.start()
            }
        }
    }

    private fun disableUserInteraction() {
        val gameStatus: TextView = findViewById(R.id.game_status)
        val highScore: String = gameModel?.mGetHighScore().toString()
        gameStatus.text = "Watch the pattern. \n High Score: $highScore"
        Log.e("GameActivity", "Buttons Disabled.")
        green_button.isFocusableInTouchMode = false
        red_button.isFocusableInTouchMode = false
        yellow_button.isFocusableInTouchMode = false
        blue_button.isFocusableInTouchMode = false
    }

    private fun enableUserInteraction() {
        val gameStatus: TextView = findViewById(R.id.game_status)
        val highScore: String = gameModel?.mGetHighScore().toString()
        gameStatus.text = "Repeat the pattern. \n High Score: $highScore"
        Log.e("GameActivity", "Buttons Enabled.")
        green_button.isFocusableInTouchMode = true
        red_button.isFocusableInTouchMode = true
        yellow_button.isFocusableInTouchMode = true
        blue_button.isFocusableInTouchMode = true
    }

    private fun writeToFile() {
        val highScores: ArrayList<Int> = gameModel!!.mGetHighScoreList()
        val scores = JSONObject()

        scores.put("1", highScores[0].toString())
        scores.put("2", highScores[1].toString())
        scores.put("3", highScores[2].toString())
        scores.put("4", highScores[3].toString())
        scores.put("5", highScores[4].toString())
        scores.put("6", highScores[5].toString())
        scores.put("7", highScores[6].toString())
        scores.put("8", highScores[7].toString())
        scores.put("9", highScores[8].toString())
        scores.put("10", highScores[9].toString())

        scores.put("scores", scores)

        val scoreString: String = scores.toString()

        try {
            val filename = "simon"
            openFileOutput(filename, MODE_PRIVATE).use {
                it.write(scoreString.toByteArray()) }
            Log.e("GameActivity", "Saved scores to internal storage.")
        }catch (ex:Exception){
            Log.e("GameActivity", "Unable to open file for writing.")
        }
    }

}


