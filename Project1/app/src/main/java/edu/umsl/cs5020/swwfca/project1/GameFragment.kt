package edu.umsl.cs5020.swwfca.project1

import android.os.Bundle
import android.os.Handler
import android.app.Fragment
import android.util.Log

class GameFragment : Fragment() {

    private var handler: Handler? = null
    var listener: GameFragmentListener? = null

    interface GameFragmentListener {
        //        fun systemFlash()
        val sequenceSize: Int
        val delay: Long
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        retainInstance = true
        Log.e("GameFragment", "GameFragment was Created!!!")
    }

    override fun onDestroy() {
        super.onDestroy()
        Log.e("GameFragment", "GameFragment was Destroyed!!!")
    }
}

//    fun startSequence() {
//        Log.e("GameFragment", "Sequence Started!")
//        if (handler == null) {
//            handler = Handler()
//            handler!!.postDelayed(runnable, 1000)
//        } else {
//            Log.e("GameFragment", "Handler already created!")
//        }
//    }

//    // Send a thread out whenever a button is pressed by user.
//    fun buttonTriggered() {
//        handler?.post(runnable)
//    }
//
//    fun systemTriggered(){
//        handler?.post(runnable)
//    }

//    // use when game session is over.
//    fun stopSequence() {
//        Log.e("GameFragment", "Sequence Stopped!")
//        handler?.removeCallbacks(runnable)
//        handler = null
//    }
//
//    // We override the runnable so that we can start a new thread whenever something is triggered.
//    private var runnable = object : Runnable {
//        override fun run() {
//            Log.e("GameFragment", "Running the runnable.")
//            // listener in the override run function will spawn threads every time it sees the function.
//            // This ends badly with a loop in the activity.  Trigger posts manually.
//            //listener?.systemFlash()
//            handler?.postDelayed(this, listener.let {it!!.delay})
//        }
//    }
//}
