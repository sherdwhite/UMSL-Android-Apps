package edu.umsl.cs5020.swwfca.project1

import android.app.Activity
import android.os.Bundle

class HighScoreActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_high_score)
    }

    override fun onBackPressed() {
        finish()
    }
}
