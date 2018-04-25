package edu.umsl.cs5020.swwfca.project1

import android.util.Log
import java.util.*  // for Random
import kotlin.collections.ArrayList

class GameModel(_highScoreList: ArrayList<Int>){
    private val mColorList: ArrayList<Int> = ArrayList()
    private val mHighScoreList: ArrayList<Int> = _highScoreList
    private var mHighScore: Int = mHighScoreList[0]
    private var mScore: Int = 0
    private var mIndex: Int = 0
    private var mDelay: Long = 0

    fun mAddRandomColor() {
        val random = Random()
        val randomColor = random.nextInt(3)
        Log.e("GameModel", "Adding $randomColor to color list.")
        mColorList.add(randomColor)
    }

    fun mGetHighScoreList(): ArrayList<Int> {
        Log.e("GameModel", "High Score List Array Returned.")
        return mHighScoreList
    }

    fun mSetHighScoreList(highScores: ArrayList<Int>) {
        Log.e("GameModel", "High Score List Array Set.")

    }

    fun mCheckColor(index: Int, color: Int): Boolean {
        val colorValue = mColorList.elementAt(index)
        Log.e("GameModel", "Color $color checked against index $index. with color value of $colorValue")
        return (mColorList.elementAt(index) == color)
    }

    fun mGetSize(): Int {
        Log.e("GameModel", "Color List Size Returned.")
        return mColorList.size
    }

    fun mColorVal(index: Int): Int {
        Log.e("GameModel", "Element at index: $index returned with colorListSize = ${mColorList.size}.")
        return mColorList.elementAt(index)
    }

    fun mGetScore(): Int {
        Log.e("GameModel", "Score Returned.")
        return mScore
    }

    fun mGetHighScore(): Int{
        Log.e("GameModel", "High Score Returned.")
        return mHighScore
    }

    fun mSetHighScore(highScore: Int){
        Log.e("GameModel", "High Score Set to $highScore.")
        mHighScore = highScore
    }

    fun mAddToScore() {
        Log.e("GameModel", "Adding to Score.")
        mScore++
        if (mScore > mHighScore) {
            mHighScore = mScore
        }
    }

    fun mSetDelay(delay: Long){
        Log.e("GameModel", "Delay of $delay Set.")
        mDelay = delay
    }

    fun mGetDelay(): Long {
        Log.e("GameModel", "Delay Returned.")
        return mDelay
    }

    fun mGetIndex(): Int {
        Log.e("GameModel", "Index Returned.")
        return mIndex
    }

    fun mIncrementIndex(){
        Log.e("GameModel", "Index Incremented.")
        mIndex.inc()
    }

    fun mResetIndex(){
        Log.e("GameModel", "Index Reset.")
        mIndex = 0
    }
}