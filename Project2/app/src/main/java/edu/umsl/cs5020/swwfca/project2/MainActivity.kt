package edu.umsl.cs5020.swwfca.project2

import android.app.Activity
import android.os.Bundle
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : Activity(), ClockFragment.ClockFragmentListener {

    private var clockModel: ClockModel? = null
    private var clockFragment: ClockFragment? = null
//    private var lapFragment: LapFragment? = null
    private var startButtonState: Boolean = true
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager
    private var laps = arrayListOf<String>()
    private var index = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        bindButtons()

        clockModel = ClockModel()

        if (savedInstanceState != null) {
            val savedStartState = savedInstanceState.getBoolean("startState")
            clockModel?.mSetMinutes(savedInstanceState.getInt("minutes"))
            clockModel?.mSetSeconds(savedInstanceState.getInt("seconds"))
            clockModel?.mSetMilli(savedInstanceState.getLong("milliSeconds"))
            clockModel?.mSetLapList(savedInstanceState.getStringArrayList("laps"))


            if (!savedStartState) {
                stop_button.isEnabled = true
                stop_button.alpha = 1.0f
                start_button.isEnabled = false
                start_button.alpha = 0.5f
                startButtonState = false
            }
            else{
                start_button.isEnabled = true
                start_button.alpha = 1.0f
                stop_button.isEnabled = false
                stop_button.alpha = 0.5f
                startButtonState = true
            }
        }
        else{
            start_button.isEnabled = true
            start_button.alpha = 1.0f
            stop_button.isEnabled = false
            stop_button.alpha = 0.5f
            startButtonState = true
            clockModel?.mSetMinutes(0)
            clockModel?.mSetSeconds(0)
            clockModel?.mSetMilli(0)
        }


        viewManager = LinearLayoutManager(this)
        viewAdapter = MainAdapter(laps)

        recyclerView = findViewById<RecyclerView>(R.id.lap_list_recycler_view).apply {
            // use a linear layout manager
            layoutManager = viewManager

            // specify an viewAdapter
            adapter = viewAdapter
        }

        if(clockModel!!.mGetLapListSize() > 0) {
            for (i in 0..(clockModel!!.mGetLapListSize() - 1)) {

                laps.add(clockModel!!.mGetLaps(i))
                index++
            }
        }
        else{
            laps.add("No Laps Yet")
            index = 0
        }

        recyclerView.adapter.notifyDataSetChanged()

        val minStr = clockModel!!.mGetMinutes().toString()
        val secStr = clockModel!!.mGetSeconds().toString()
        val milliStr = clockModel!!.mGetMilli().toString()
        if (minStr.toInt() < 10)
            minute_text.text = "0$minStr"
        else
            minute_text.text = minStr
        if (secStr.toInt() < 10)
            second_text.text = "0$secStr"
        else
            second_text.text = secStr
        when {
            milliStr.toLong() < 10 -> millisecond_text.text = "00$milliStr"
            milliStr.toLong() in 10..99 -> millisecond_text.text = "0$milliStr"
            milliStr.toLong() > 100 -> millisecond_text.text = milliStr
        }

        val manager = fragmentManager
        clockFragment = manager.findFragmentByTag("ClockFragment") as? ClockFragment
        if (clockFragment == null) {
            clockFragment = ClockFragment()
            manager.beginTransaction()
                    .add(clockFragment, "ClockFragment")
                    .commit()
        }
        clockFragment?.listener = this

//        lapFragment = manager.findFragmentByTag("LapFragment") as? LapFragment
//        if (lapFragment == null) {
//            lapFragment = LapFragment()
//            manager.beginTransaction()
//                    .add(lapFragment, "LapFragment")
//                    .commit()
//        }
//
//        lapFragment?.listener = object: LapFragment.LapListViewListener{
//            override val lapList: ArrayList<String>?
//                get() = clockModel?.mLapList
//
//            override fun selectedItemAtPosition(position: Int) {
//                val laps = clockModel?.mLapList?.get(position)
//                Log.e("MainActivity", "Lap Data: ${laps.toString()}")
//            }
//        }

    }

    override fun onBackPressed() {
        finish()
    }

    private fun bindButtons() {
        start_button.setOnClickListener {
            Log.e("MainActivity", "Start pressed!")
            clockFragment?.startTriggered()
        }
        stop_button.setOnClickListener {
            Log.e("MainActivity", "Stop pressed!")
            clockFragment?.stopTriggered()
        }
        reset_button.setOnClickListener {
            Log.e("MainActivity", "Reset pressed!")
            clockFragment?.resetTriggered()
            clockModel?.mClearLaps()
            laps.clear()
            index = 0
            laps.add("No Laps Yet")
            recyclerView.adapter.notifyDataSetChanged()
        }
        lap_button.setOnClickListener {
            clockFragment?.lapTriggered()
            if (index == 0) {
                laps[0] = clockModel!!.mGetLaps(0)
                recyclerView.adapter.notifyItemChanged(index)
            }
            else {
                laps.add(clockModel!!.mGetLaps(index))
                recyclerView.adapter.notifyItemInserted(index)
            }
            Log.e("MainActivity", "Lap pressed! Lap: $laps")
            index++
        }
    }

    // for passing model info to the fragment
    override val minutes: Int
        get() = clockModel?.mGetMinutes() ?: 0

    override val seconds: Int
        get() = clockModel?.mGetSeconds() ?: 0

    override val milliSeconds: Long
        get() = clockModel?.mGetMilli() ?: 0

    override fun setMinutes(minutes: String){
        clockModel?.mSetMinutes(minutes.toInt())
        if (minutes.toInt() < 10)
            minute_text.text = "0$minutes"
        else
            minute_text.text = minutes
    }

    override fun setSeconds(seconds: String){
        clockModel?.mSetSeconds(seconds.toInt())
        if (seconds.toInt() < 10)
            second_text.text = "0$seconds"
        else
            second_text.text = seconds
    }

    override fun setMilliSeconds(milli: String){
        clockModel?.mSetMilli(milli.toLong())
        when {
            milli.toLong() < 10 -> millisecond_text.text = "00$milli"
            milli.toLong() in 10..99 -> millisecond_text.text = "0$milli"
            milli.toLong() > 100 -> millisecond_text.text = milli
        }
    }

    override fun addLap(lap: String){
        clockModel?.mAddLap(lap)
    }

    override fun clearLaps(){
        clockModel?.mClearLaps()
    }

    ////////////////////////////////////////

    override fun disableStart() {
        start_button.alpha = .5f
        start_button.isEnabled = false
        stop_button.alpha = 1.0f
        stop_button.isEnabled = true
        startButtonState = false
    }

    override fun enableStart() {
        start_button.alpha = 1.0f
        start_button.isEnabled = true
        stop_button.alpha = 0.5f
        stop_button.isEnabled = false
        startButtonState = true
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        outState.putBoolean("startState", startButtonState)
        outState.putInt("minutes", clockModel!!.mGetMinutes())
        outState.putInt("seconds", clockModel!!.mGetSeconds())
        outState.putLong("milliSeconds", clockModel!!.mGetMilli())
        outState.putStringArrayList("laps", clockModel!!.mGetLapList())
    }

}



