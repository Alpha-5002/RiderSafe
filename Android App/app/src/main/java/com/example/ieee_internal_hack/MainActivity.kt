package com.example.ieee_internal_hack

import android.Manifest
import android.content.pm.PackageManager
import android.location.Location
import android.os.Bundle
import android.telephony.SmsManager
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices

class MainActivity : AppCompatActivity() {

    private lateinit var locationClient: FusedLocationProviderClient
    private lateinit var locationText: TextView
    private lateinit var phoneEditText: EditText

    private var currentLocation: Location? = null

    companion object {
        const val LOCATION_PERMISSION_CODE = 100
        const val SMS_PERMISSION_CODE = 200
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        phoneEditText = findViewById(R.id.phoneEditText)
        locationText = findViewById(R.id.locationText)

        val getLocationBtn = findViewById<Button>(R.id.getLocationBtn)
        val sendSmsBtn = findViewById<Button>(R.id.sendSmsBtn)
        val simulateCrashBtn = findViewById<Button>(R.id.simulateCrashBtn)

        locationClient = LocationServices.getFusedLocationProviderClient(this)

        getLocationBtn.setOnClickListener {
            requestLocationPermission()
        }

        sendSmsBtn.setOnClickListener {
            requestSmsPermission()
        }

        simulateCrashBtn.setOnClickListener {
            Toast.makeText(this, "Simulated crash alert (mock BLE)", Toast.LENGTH_SHORT).show()
            requestLocationAndSendSms()
        }
    }

    private fun requestLocationPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.ACCESS_FINE_LOCATION), LOCATION_PERMISSION_CODE)
        } else {
            getLocation()
        }
    }

    private fun requestSmsPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.SEND_SMS)
            != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.SEND_SMS), SMS_PERMISSION_CODE)
        } else {
            sendEmergencySms()
        }
    }

    private fun requestLocationAndSendSms() {
        requestLocationPermission()
        requestSmsPermission()
    }

    private fun getLocation() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) return

        locationClient.lastLocation.addOnSuccessListener {
            if (it != null) {
                currentLocation = it
                val locationStr = "Lat: ${it.latitude}, Lon: ${it.longitude}"
                locationText.text = locationStr
            } else {
                Toast.makeText(this, "Location not available", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun sendEmergencySms() {
        if (currentLocation == null) {
            Toast.makeText(this, "Get location first", Toast.LENGTH_SHORT).show()
            return
        }

        val phoneNumber = phoneEditText.text.toString().trim()
        if (phoneNumber.isEmpty()) {
            Toast.makeText(this, "Please enter a valid phone number", Toast.LENGTH_SHORT).show()
            return
        }

        val message = "Emergency! Simulated crash at https://maps.google.com/?q=${currentLocation!!.latitude},${currentLocation!!.longitude}"

        try {
            val smsManager = SmsManager.getDefault()
            smsManager.sendTextMessage(phoneNumber, null, message, null, null)
            Toast.makeText(this, "SMS sent!", Toast.LENGTH_SHORT).show()
        } catch (e: Exception) {
            Toast.makeText(this, "SMS failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            LOCATION_PERMISSION_CODE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    getLocation()
                }
            }
            SMS_PERMISSION_CODE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    sendEmergencySms()
                }
            }
        }
    }
}

