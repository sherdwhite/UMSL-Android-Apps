package edu.umsl.cs5020.swwfca.project3

import android.app.Activity
import android.graphics.Canvas
import android.os.Bundle
import android.graphics.Bitmap
import android.graphics.Color
import android.graphics.Paint
import android.util.Log
import android.widget.ImageView
import android.widget.Toast
import android.view.inputmethod.EditorInfo
import kotlinx.android.synthetic.main.activity_game.*
import android.app.AlertDialog
import android.content.Intent


class GameActivity : Activity() {

    private var gameModel: GameModel? = null
    private var phrase: String? = null
    private var uncovered: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)

        bindInput()

        gameModel = GameModel()

        phrase = gameModel!!.mGetRandPhrase()
//        Toast.makeText(applicationContext, phrase, Toast.LENGTH_SHORT).show()

        drawFrame()
    }

    private fun bindInput(){
        text_input.setOnEditorActionListener({ _, actionId, _ ->
            var handled = false
            if (actionId == EditorInfo.IME_ACTION_SEND) {
                handled = true
            }
            val letter = text_input.text.toString().toUpperCase()
            if(letter.isEmpty()){
                Toast.makeText(applicationContext, "Please enter a letter!", Toast.LENGTH_SHORT).show()
            }
            else {
                val uppercase = letter.single()
//            Toast.makeText(applicationContext, letter.toString(), Toast.LENGTH_SHORT).show()
                if (uppercase in 'A'..'Z') {
                    if (uppercase.toString() in gameModel!!.mGetUsedList()) {
                        Toast.makeText(applicationContext, "Letter $uppercase already used!", Toast.LENGTH_SHORT).show()
                    } else {
                        gameModel?.mAddUsed(uppercase.toString().toUpperCase())
                        val usedLetters = gameModel!!.mGetUsedList().toString().replace("[", "").replace("]", "").toUpperCase()
                        used_letters.text = usedLetters
                    }
                } else {
                    Toast.makeText(applicationContext, "Please enter a letter!", Toast.LENGTH_SHORT).show()
                }
            }
            text_input.setText("")
            drawFrame()
            handled
        })
//        text_input.addTextChangedListener(watch)

    }

    private fun drawFrame(){
        // Get a reference to our ImageView in the layoutSSS
        val ourFrame = findViewById<ImageView>(R.id.canvas)

        // Create a bitmap object to use as our canvas
        val ourBitmap = Bitmap.createBitmap(350, 350, Bitmap.Config.ARGB_8888)

        val ourCanvas = Canvas(ourBitmap)
        // A paint object that does our drawing, on our canvas
        val paint = Paint()
        // Set the background color
        ourCanvas.drawColor(Color.BLACK)

        if(gameModel!!.mGetIncorrect() >= 1) {
            // Draw Posts
            // Change the color of the virtual paint brush
            paint.color = Color.argb(255, 255, 255, 255)
            ourCanvas.drawRect(25F, 15F, 35F, 290F, paint)
            ourCanvas.drawRect(25F, 15F, 175F, 25F, paint)
            ourCanvas.drawRect(170F, 15F, 180F, 95F, paint)
            ourCanvas.drawRect(10F, 280F, 175F, 290F, paint)
        }

        if(gameModel!!.mGetIncorrect() >= 2) {
            // Draw Person
            paint.color = Color.argb(255, 0, 255, 0)
            ourCanvas.drawCircle(175F, 65F, 25F, paint)
            paint.color = Color.argb(255, 255, 255, 255)
            ourCanvas.drawCircle(165F, 55F, 5F, paint)
            ourCanvas.drawCircle(185F, 55F, 5F, paint)
            ourCanvas.drawText("X", 160F, 75F, paint)
            ourCanvas.drawText("X", 170F, 75F, paint)
            ourCanvas.drawText("X", 180F, 75F, paint)
        }

        if(gameModel!!.mGetIncorrect() >= 3) {
            paint.color = Color.argb(255, 0, 255, 0)
            ourCanvas.drawLine(175F, 40F, 175F, 215F, paint)
        }

        if(gameModel!!.mGetIncorrect() >= 4) {
            ourCanvas.drawLine(175F, 125F, 125F, 115F, paint)
        }
        if(gameModel!!.mGetIncorrect() >= 5) {
            ourCanvas.drawLine(175F, 125F, 225F, 115F, paint)
        }
        if(gameModel!!.mGetIncorrect() >= 6) {
            ourCanvas.drawLine(175F, 215F, 125F, 265F, paint)
        }
        if(gameModel!!.mGetIncorrect() >= 7) {
            ourCanvas.drawLine(175F, 215F, 225F, 265F, paint)
            gameOver()
        }



        // Draw Phrase
        uncovered = 0
        paint.color = Color.argb(255, 255, 255, 255)
        paint.textSize = 15f
        val length = phrase!!.length
        val halfLength = (length / 2) * 20
        var midPosition = 175
        var currentPosition: Float = (midPosition - halfLength - 5).toFloat()
        ourCanvas.drawText("Phrase: ", 10F, 310F, paint)
        val tempString = phrase!!.toMutableList()
        //Draw Phrase
        for(i in 0..(length - 1)){
            var letter = tempString[i].toString()
            Log.e("GameActivity", "Sequence Variable $letter")
            if(letter != " ") {
                if(letter in gameModel!!.mGetUsedList()) {
                    ourCanvas.drawText(letter, currentPosition + 3, 335F, paint)
                    uncovered++
                }
                ourCanvas.drawLine(currentPosition, 340F, currentPosition + 15, 340F, paint)
            }
            else{
                uncovered++
            }
            currentPosition += 20
        }

        //Now put the canvas in the frame
        ourFrame.setImageBitmap(ourBitmap)
        ourFrame.postInvalidate()

        if(uncovered == phrase!!.length){
            val builder = AlertDialog.Builder(this)
            builder.setCancelable(false)
            builder.setMessage("You won! Play Again?")
            builder.setPositiveButton("Yes") { _, _ ->
                //if user pressed "yes", then user plays again
                finish()
                val gameIntent = Intent(this, GameActivity::class.java)
                startActivity(gameIntent)
            }
            builder.setNegativeButton("No") { _, _ ->
                //if user select "No", exits to menu
                finish()
            }
            val alert = builder.create()
            alert.show()
            uncovered = 0
        }
    }

    override fun onBackPressed() {
        val builder = AlertDialog.Builder(this)
        builder.setCancelable(false)
        builder.setMessage("Are you sure you want to Quit?")
        builder.setPositiveButton("Yes") { _, _ ->
            //if user pressed "yes", then he is allowed to exit from application
            finish()
        }
        builder.setNegativeButton("No") { dialog, _ ->
            //if user select "No", just cancel this dialog and continue with app
            dialog.cancel()
        }
        val alert = builder.create()
        alert.show()
    }

    private fun gameOver() {
        val builder = AlertDialog.Builder(this)
        builder.setCancelable(false)
        builder.setMessage("Do you want to play again?")
        builder.setPositiveButton("Yes") { _, _ ->
            //if user pressed "yes", then user plays again
            finish()
            val gameIntent = Intent(this, GameActivity::class.java)
            startActivity(gameIntent)
        }
        builder.setNegativeButton("No") { _, _ ->
            //if user select "No", exits to menu
            finish()
        }
        val alert = builder.create()
        alert.show()
        uncovered = 0
    }

//    private var watch: TextWatcher = object : TextWatcher {
//
//        override fun afterTextChanged(arg0: Editable) {
//            Toast.makeText(applicationContext, "Maximum Limit Reached", Toast.LENGTH_SHORT).show()
//
//        }
//
//        override fun beforeTextChanged(arg0: CharSequence, arg1: Int, arg2: Int,
//                                       arg3: Int) {
//        }
//
//        override fun onTextChanged(s: CharSequence, a: Int, b: Int, c: Int) {
//        }
//    }
}
