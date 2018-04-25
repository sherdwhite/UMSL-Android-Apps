package edu.umsl.cs5020.swwfca.project2

import android.os.Bundle
import android.os.Handler
import android.app.Fragment
import android.os.SystemClock
import android.util.Log

class ClockFragment : Fragment() {

    var listener: ClockFragmentListener? = null
    private var handler: Handler? = null
    private var isRunning: Boolean = false
    private var minutesElapsed: Int = 0
    private var secondsElapsed: Int = 0
    private var milliSecondsElapsed: Long = 0
    private var timeMilliSeconds: Long = 0
    private var startTime:Long = 0
    private var bufferTime:Long = 0
    private var runTime:Long = 0
    private var lapIndex:Int = 1

    interface ClockFragmentListener {
        fun enableStart()
        fun disableStart()
        fun setMinutes(minutes: String)
        fun setSeconds(seconds: String)
        fun setMilliSeconds(milli: String)
        fun addLap(lap: String)
        fun clearLaps()
        val minutes: Int
        val seconds: Int
        val milliSeconds: Long
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        retainInstance = true
        minutesElapsed = listener.let { it!!.minutes }
        secondsElapsed = listener.let { it!!.seconds }
        milliSecondsElapsed = listener.let { it!!.milliSeconds }
        listener?.setMinutes(minutesElapsed.toString())
        listener?.setSeconds(secondsElapsed.toString())
        listener?.setMilliSeconds(milliSecondsElapsed.toString())
        Log.e("ClockFragment", "ClockFragment was Created!!!")
    }

    override fun onDestroy() {
        super.onDestroy()
        Log.e("ClockFragment", "ClockFragment was Destroyed!!!")
    }

    fun startTriggered() {
        Log.e("ClockFragment", "Sequence Started!")
        if (!isRunning) {
            listener?.disableStart()
            isRunning = true
            startTime = SystemClock.uptimeMillis()
            if (handler == null) {
                handler = Handler()
                handler!!.post(runnable)
            } else {
                Log.e("ClockFragment", "Handler already created!")
            }
        }
    }

    fun resetTriggered(){
        Log.e("ClockFragment", "Reset Triggered!")
        if (isRunning) {
            handler?.removeCallbacks(runnable)
            handler = null
            isRunning = false

            minutesElapsed = 0
            secondsElapsed = 0
            milliSecondsElapsed = 0
            timeMilliSeconds = 0
            startTime = 0
            bufferTime = 0
            runTime = 0
            lapIndex = 1

            listener?.setMinutes(minutesElapsed.toString())
            listener?.setSeconds(secondsElapsed.toString())
            listener?.setMilliSeconds(milliSecondsElapsed.toString())
            listener?.clearLaps()

            if (!isRunning) {
                listener?.disableStart()
                isRunning = true
                startTime = SystemClock.uptimeMillis()
                if (handler == null) {
                    handler = Handler()
                    handler!!.post(runnable)
                } else {
                    Log.e("ClockFragment", "Handler already created!")
                }
            }
        }
        else {
            minutesElapsed = 0
            secondsElapsed = 0
            milliSecondsElapsed = 0
            timeMilliSeconds = 0
            startTime = 0
            bufferTime = 0
            runTime = 0
            lapIndex = 1

            listener?.setMinutes(minutesElapsed.toString())
            listener?.setSeconds(secondsElapsed.toString())
            listener?.setMilliSeconds(milliSecondsElapsed.toString())
        }
    }

    fun lapTriggered(){
        Log.e("ClockFragment", "Lap Triggered!")
        var minutesString: String
        var secondsString: String
        var millisecondsString = ""
        if (isRunning) {
            handler?.removeCallbacks(runnable)
            handler = null
            isRunning = false

            if (minutesElapsed < 10)
                minutesString = "0$minutesElapsed"
            else
                minutesString = "$minutesElapsed"
            if (secondsElapsed < 10)
                secondsString = "0$secondsElapsed"
            else
                secondsString = "$secondsElapsed"
            when {
                milliSecondsElapsed < 10 -> millisecondsString = "00$milliSecondsElapsed"
                milliSecondsElapsed in 10..99 -> millisecondsString = "0$milliSecondsElapsed"
                milliSecondsElapsed > 100 -> millisecondsString = "$milliSecondsElapsed"
            }

            listener?.addLap("Lap $lapIndex Time: $minutesString:$secondsString:$millisecondsString.")

            minutesElapsed = 0
            secondsElapsed = 0
            milliSecondsElapsed = 0
            timeMilliSeconds = 0
            startTime = 0
            bufferTime = 0
            lapIndex++

            listener?.setMinutes(minutesElapsed.toString())
            listener?.setSeconds(secondsElapsed.toString())
            listener?.setMilliSeconds(milliSecondsElapsed.toString())

            if (!isRunning) {
                listener?.disableStart()
                isRunning = true
                startTime = SystemClock.uptimeMillis()
                if (handler == null) {
                    handler = Handler()
                    handler!!.post(runnable)
                } else {
                    Log.e("ClockFragment", "Handler already created!")
                }
            }
        }
        else {
            if (minutesElapsed < 10)
                minutesString = "0$minutesElapsed"
            else
                minutesString = "$minutesElapsed"
            if (secondsElapsed < 10)
                secondsString = "0$secondsElapsed"
            else
                secondsString = "$secondsElapsed"
            when {
                milliSecondsElapsed < 10 -> millisecondsString = "00$milliSecondsElapsed"
                milliSecondsElapsed in 10..99 -> millisecondsString = "0$milliSecondsElapsed"
                milliSecondsElapsed > 100 -> millisecondsString = "$milliSecondsElapsed"
            }

            listener?.addLap("Lap $lapIndex Time: $minutesString:$secondsString:$millisecondsString.")

            minutesElapsed = 0
            secondsElapsed = 0
            milliSecondsElapsed = 0
            timeMilliSeconds = 0
            startTime = 0
            bufferTime = 0
            runTime = 0
            lapIndex++

            listener?.setMinutes(minutesElapsed.toString())
            listener?.setSeconds(secondsElapsed.toString())
            listener?.setMilliSeconds(milliSecondsElapsed.toString())
        }
    }

    // use when game session is over.
    fun stopTriggered() {
        Log.e("ClockFragment", "Sequence Stopped!")
        bufferTime += timeMilliSeconds
        listener?.enableStart()
        handler?.removeCallbacks(runnable)
        handler = null
        isRunning = false
    }

    // We override the runnable so that we can start a new thread whenever something is triggered.
    private var runnable = object : Runnable {
        override fun run() {
            Log.e("ClockFragment", "Running the runnable.")
            // listener in the override run function will spawn threads every time it sees the function.
            // This ends badly with a loop in the activity.  Trigger posts manually.
            //listener?.systemFlash()
            timeMilliSeconds = SystemClock.uptimeMillis() - startTime
            runTime = bufferTime + timeMilliSeconds
            secondsElapsed = (runTime / 1000).toInt()
            minutesElapsed = secondsElapsed / 60
            secondsElapsed %= 60
            milliSecondsElapsed = (runTime % 1000)
            listener?.setMinutes(minutesElapsed.toString())
            listener?.setSeconds(secondsElapsed.toString())
            listener?.setMilliSeconds(milliSecondsElapsed.toString())
            handler?.postDelayed(this, 0)
        }
    }
}
