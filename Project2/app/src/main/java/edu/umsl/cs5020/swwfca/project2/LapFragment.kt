package edu.umsl.cs5020.swwfca.project2

import android.os.Bundle
import android.app.Fragment
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_main.*
import java.util.ArrayList

internal class LapFragment : Fragment() {

    var listener: LapListViewListener? = null

    internal interface LapListViewListener {
        val lapList: ArrayList<String>?
        fun selectedItemAtPosition(position: Int)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                          savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.activity_main, container, false)
        Log.d("LapFragment", "LapFragment Created.")
        }

    override fun onViewCreated(view: View?, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        lap_list_recycler_view?.layoutManager = LinearLayoutManager(activity)
        lap_list_recycler_view?.adapter = LapAdapter()
        Log.d("LapFragment", "LapFragment Adapter set.")
    }

    inner class LapAdapter: RecyclerView.Adapter<LapHolder>() {
        override fun onCreateViewHolder(parent: ViewGroup?, viewType: Int): LapHolder {
            val inflater = LayoutInflater.from(activity)
            val view = inflater.inflate(R.layout.lap_row_layout, parent, false)
            Log.d("LapFragment", "LapHolder Created.")
            return LapHolder(view)
        }

        override fun getItemCount(): Int {
            Log.d("LapFragment", "getItemCount returned size.")
            return listener?.lapList!!.size
        }

        override fun onBindViewHolder(holder: LapHolder?, position: Int) {
            Log.d("LapFragment", "OnBindViewHolder got position.")
            holder?.bindLap(listener?.lapList?.get(position)!!)
        }
    }

    inner class LapHolder(view: View?): RecyclerView.ViewHolder(view), View.OnClickListener {
        private val lapTextView: TextView? = view?.findViewById(R.id.lap_row_text)

        init {
            view?.setOnClickListener(this)
        }

        fun bindLap(lap: String) {
            lapTextView?.text = "$lap"
            Log.d("LapFragment", "bindLap text set.")
        }

        override fun onClick(v: View?) {
            listener?.selectedItemAtPosition(adapterPosition)
            Log.d("LapFragment", "What is the position: $adapterPosition")
        }
    }
}
