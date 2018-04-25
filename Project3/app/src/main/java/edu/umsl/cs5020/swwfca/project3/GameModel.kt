package edu.umsl.cs5020.swwfca.project3

import android.app.Activity
import android.util.Log
import java.util.*

class GameModel : Activity() {

    private val mPhraseBank = arrayListOf("SOFTWARE ENGINEER", "PHYSICS", "ASTRONOMY",
            "MATHEMATICS", "BIOLOGY", "CHEMISTRY", "ENGINEERING", "I LIKE TOAST", "COMPUTER SCIENCE", "UNIVERSITY")
    private var mIncorrect: Int = 0
    var mUsedLetters = ArrayList<String>()
    private var mCurrentPhrase = String()

    fun mGetRandPhrase(): String{
        val random = Random()
        val randomPhrase = mPhraseBank[random.nextInt(mGetPhraseListSize())]
        mCurrentPhrase = randomPhrase
        Log.e("GameModel", "Phrase $randomPhrase Returned.")
        return randomPhrase
    }

    fun mGetPhraseList(): ArrayList<String> {
        Log.e("GameModel", "PhraseBank Returned.")
        return mPhraseBank
    }

    private fun mGetPhraseListSize(): Int{
        Log.e("GameModel", "PhraseBank size Returned.")
        return mPhraseBank.size
    }

    fun mGetIncorrect(): Int {
        Log.e("GameModel", "Score Returned.")
        return mIncorrect
    }

    private fun mAddToIncorrect() {
        Log.e("GameModel", "Adding to Score.")
        mIncorrect++
    }

    fun mResetIncorrect() {
        Log.e("GameModel", "Adding to Score.")
        mIncorrect = 0
    }

    fun mGetUsed(position: Int): String{
        Log.e("GameModel", "Used List $position Returned.")
        return mUsedLetters[position]
    }

    fun mGetUsedList(): ArrayList<String>{
        Log.e("GameModel", "Used List Returned.")
        return mUsedLetters
    }

    fun mGetUsedListSize(): Int{
        Log.e("GameModel", "Used List Size Returned.")
        return mUsedLetters.size
    }

    fun mAddUsed(letter: String) {
        mUsedLetters.add(letter)
        if(!(mCurrentPhrase.contains(letter))){
            mAddToIncorrect()
        }
        Log.e("GameModel", "Used Letter Added.")
        Log.e("GameModel", "Used List: $mUsedLetters.")
    }

    fun mClearUsed() {
        Log.e("GameModel", "Cleared Used.")
        mUsedLetters.clear()
    }
}
