package edu.umsl.cs5020.swwfca.project2

import android.support.v7.widget.RecyclerView
import android.util.Log
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.TextView

class MainAdapter(private val laps: ArrayList<String>): RecyclerView.Adapter<MainAdapter.LapViewHolder>() {

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder.
    // Each data item is just a string in this case that is shown in a TextView.
    class LapViewHolder(val view: TextView) : RecyclerView.ViewHolder(view)

    // Create new views (invoked by the layout manager)
    override fun onCreateViewHolder(parent: ViewGroup?, viewType: Int): MainAdapter.LapViewHolder {
        // create a new view
        val inflater = LayoutInflater.from(parent?.context)
                .inflate(R.layout.lap_row_layout, parent, false) as TextView
        Log.d("LapFragment", "LapHolder Created.")
        return LapViewHolder(inflater)
    }
    override fun onBindViewHolder(holder: LapViewHolder, position: Int) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        holder.view.text = laps[position]
    }

    // Return the size of your dataset (invoked by the layout manager)
    override fun getItemCount(): Int {
        Log.d("MainAdapter", "getItemCount returned size.")
        return laps.size
    }
}