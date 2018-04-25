package edu.umsl.cs5020.swwfca.project3

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.View
import android.support.v4.app.NotificationCompat
import android.app.PendingIntent
import android.app.NotificationManager
import android.content.Context
import android.os.Handler


class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    override fun onBackPressed() {
        sendNotification()
        finish()
    }

    private fun sendNotification(){
        val handler = Handler()
        handler.postDelayed({
            // Create an explicit intent for an Activity in your appp
            val intent = Intent(this, MainActivity::class.java)
            intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
            val pendingIntent = PendingIntent.getActivity(this, 0, intent, 0)

            // NotificationCompat Builder takes care of backwards compatibility and
            // provides clean API to create rich notifications
            val mBuilder = NotificationCompat.Builder(this, "101")
                    .setSmallIcon(android.R.drawable.ic_dialog_info)
                    .setContentTitle("Hangman")
                    .setContentText("Come play Hangman!")
                    .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                    .setContentIntent(pendingIntent)
                    .setAutoCancel(true)

            // Obtain NotificationManager system service in order to show the notification
            val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.notify(101, mBuilder.build())

        }, 60000)
        Log.e("MainActivity", "Notification Triggered")
    }

    fun startGame(view: View){
        Log.e("GameStatus", "Game Started!")
//        val difficulty = difficulty_spinner.selectedItemPosition
//        Log.e("GameActivity", "The difficulty is $difficulty")
        // Setting up bogus high score for later retrieval and usage
        val gameIntent = Intent(this, GameActivity::class.java)
        startActivity(gameIntent)
    }
}
