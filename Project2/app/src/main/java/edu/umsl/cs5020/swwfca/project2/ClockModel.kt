package edu.umsl.cs5020.swwfca.project2

import android.app.Activity
import android.util.Log
import java.util.ArrayList

class ClockModel : Activity() {

    private var mMinutes: Int = 0
    private var mSeconds: Int = 0
    private var mMilliSeconds: Long = 0
    var mLapList = ArrayList<String>()

    fun mGetMinutes(): Int {
        Log.e("ClockModel", "Minutes Returned.")
        return mMinutes
    }

    fun mGetSeconds(): Int {
        Log.e("ClockModel", "Seconds Returned.")
        return mSeconds
    }

    fun mGetMilli(): Long {
        Log.e("ClockModel", "MicroSeconds Returned.")
        return mMilliSeconds
    }

    fun mSetMinutes(minutes: Int) {
        Log.e("ClockModel", "Minutes Set.")
        mMinutes = minutes
    }

    fun mSetSeconds(seconds: Int) {
        Log.e("ClockModel", "Seconds Set.")
        mSeconds = seconds
    }

    fun mSetMilli(milli: Long) {
        Log.e("ClockModel", "MicroSeconds Set.")
        mMilliSeconds = milli
    }

    fun mGetLaps(position: Int): String{
        Log.e("ClockModel", "Lap at $position Returned.")
        return mLapList[position]
    }

    fun mGetLapList(): ArrayList<String>{
        Log.e("ClockModel", "LapList Returned.")
        return mLapList
    }

    fun mGetLapListSize(): Int{
        Log.e("ClockModel", "LapList Returned.")
        return mLapList.size
    }

    fun mSetLapList(lapList: ArrayList<String>){
        Log.e("ClockModel", "LapList Set.")
        mLapList.clear()
        mLapList = lapList
    }

    fun mAddLap(lap: String) {
        mLapList.add(lap)
        Log.e("ClockModel", "Lap Added.")
        Log.e("ClockModel", "Lap List: $mLapList.")
    }

    fun mClearLaps() {
        Log.e("ClockModel", "MicroSeconds Returned.")
        mLapList.clear()
    }
}
