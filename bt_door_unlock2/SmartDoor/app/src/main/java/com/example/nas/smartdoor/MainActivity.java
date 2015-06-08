package com.example.nas.smartdoor;

import android.annotation.TargetApi;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.view.animation.AccelerateInterpolator;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/*
Service UUID? 00001800-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a00-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a01-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a02-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a03-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a04-0000-1000-8000-00805f9b34fb
Service UUID? 00001801-0000-1000-8000-00805f9b34fb
    Char UUID? 00002a05-0000-1000-8000-00805f9b34fb

Service UUID? 0000ffe0-0000-1000-8000-00805f9b34fb
    Char UUID? 0000ffe1-0000-1000-8000-00805f9b34fb
 */

/*
0000ffe0-0000-1000-8000-00805f9b34fb
Bluetooth LE uses something called the Generic Attribute Profile (GATT) model which defines Services and Characteristics,
these are identified by a value called UUID. The HM-10 has a Service that allows you to pass-through data,
I found the UUID to be 0000ffe0-0000-1000-8000-00805f9b34fb. The Characteristic that you use to send and receive data
has a UUID of 0000ffe1-0000-1000-8000-00805f9b34fb
 */

@TargetApi(21)
public class MainActivity extends ActionBarActivity
{
    private final String BT_SERVER_ADDR = "54:4A:16:26:8D:C4";
    private final String PIN = "----";

    private static final long SCAN_PERIOD = 10000;
    private int REQUEST_ENABLE_BT = 1;

    private BluetoothAdapter btAdapter;
    private BluetoothLeScanner btScanner;
    private BluetoothGatt btGatt;
    private Handler handler;
    private ScanSettings settings;
    private List<ScanFilter> filters;
    private TextView tvStatus;

    public static UUID HM_10_CONF = UUID.fromString("0000ffe0-0000-1000-8000-00805f9b34fb");
    public static UUID HM_RX_TX =  UUID.fromString("0000ffe1-0000-1000-8000-00805f9b34fb");
    private BluetoothGattCharacteristic characteristicTX;
    private BluetoothGattCharacteristic characteristicRX;
    boolean connected = false;
    private Activity activity = this;



    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        handler = new Handler();
        tvStatus = (TextView)findViewById(R.id.tvStatus);

        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "BLE Not Supported", Toast.LENGTH_SHORT).show();
            finish();
        }
        final BluetoothManager btManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        btAdapter = btManager.getAdapter();
    }


    @Override
    protected void onResume()
    {
        super.onResume();

        if (btAdapter == null || !btAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        else {
            btScanner = btAdapter.getBluetoothLeScanner();
            settings = new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build();
            filters = new ArrayList<ScanFilter>();
            scanLeDevice(true);
        }
    }


    @Override
    protected void onPause()
    {
        super.onPause();
        if (btAdapter != null && btAdapter.isEnabled()) {
            scanLeDevice(false);
        }
    }


    @Override
    protected void onDestroy()
    {
        if (btGatt == null) {
            return;
        }
        btGatt.close();
        btGatt = null;
        super.onDestroy();
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_CANCELED) {
                //Bluetooth not enabled.
                finish();
                return;
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }


    private void scanLeDevice(final boolean enable)
    {
        if (enable)
        {
            handler.postDelayed(new Runnable()
            {
                @Override
                public void run() {
                    btScanner.stopScan(mScanCallback);
                }
            }, SCAN_PERIOD);

            btScanner.startScan(filters, settings, mScanCallback);
        }
        else
        {
            btScanner.stopScan(mScanCallback);
        }
    }


    private ScanCallback mScanCallback = new ScanCallback()
    {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            Log.i("callbackType", String.valueOf(callbackType));
            Log.i("result", result.toString());
            BluetoothDevice btDevice = result.getDevice();

            if(btDevice.getAddress().equals(BT_SERVER_ADDR)) {
                Log.i("NAS>>>Device Name", btDevice.getName());
                Log.i("NAS>>>Device Addr", btDevice.getAddress());
                connectToDevice(btDevice);
            }
        }


        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            for (ScanResult sr : results) {
                Log.i("ScanResult - Results", sr.toString());
            }
        }


        @Override
        public void onScanFailed(int errorCode) {
            Log.e("Scan Failed", "Error Code: " + errorCode);
        }
    };


    public void connectToDevice(BluetoothDevice device) {
        if (btGatt == null) {
            btGatt = device.connectGatt(this, false, gattCallback);
            scanLeDevice(false);// will stop after first device detection
            Toast.makeText(this, "Connected to " + device.getName(), Toast.LENGTH_SHORT).show();
        }
    }


    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Log.i("onConnectionStateChange", "Status: " + status);
            switch (newState) {
                case BluetoothProfile.STATE_CONNECTED:
                    Log.i("gattCallback", "STATE_CONNECTED");
                    connected = true;
                    gatt.discoverServices();
                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    Log.e("gattCallback", "STATE_DISCONNECTED");
                    connected = false;
                    break;
                default:
                    Log.e("gattCallback", "STATE_OTHER");
                    connected = false;
                    break;
            }

        }


        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status)
        {
            characteristicTX = gatt.getService(HM_10_CONF).getCharacteristic(HM_RX_TX);
            characteristicRX = gatt.getService(HM_10_CONF).getCharacteristic(HM_RX_TX);

            try{ Thread.sleep(1000);}
            catch (InterruptedException ex){}
            String str = "s"; //get status
            final byte[] tx = str.getBytes();
            Log.d("NAS>>>", "Sending status request");
            characteristicTX.setValue(tx);
            btGatt.writeCharacteristic(characteristicTX);
            btGatt.setCharacteristicNotification(characteristicRX, true);
        }


        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            byte[] val = characteristic.getValue();
            String s = new String(val);
            Log.i("onCharacteristicRead", s);
            //gatt.disconnect();
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            byte[] val = characteristic.getValue();
            final String recv = new String(val);
            Log.i("onCharacteristicChanged", recv);
            activity.runOnUiThread(new Runnable() {
                public void run() {
                    if(recv.equals("0")) {
                        tvStatus.setText("UNLOCKED");
                    }
                    else if(recv.equals("1")) {
                        tvStatus.setText("LOCKED");
                    }
                    else {
                        Toast.makeText(activity, recv, Toast.LENGTH_LONG).show();
                        if(recv.equals("Locking")) tvStatus.setText("LOCKED");
                        else if(recv.equals("Unlocking")) tvStatus.setText("UNLOCKED");
                    }
                }
            });
            //gatt.disconnect();
        }

    };


    public void btnOpenCloseClick(View view)
    {
        String str = PIN;
        final byte[] tx = str.getBytes();

        if(connected)
        {
            Log.d("NAS>>>", "Sending result=" + str);
            characteristicTX.setValue(tx);
            btGatt.writeCharacteristic(characteristicTX);
            btGatt.setCharacteristicNotification(characteristicRX, true);
        }
        else
        {
            connected = btGatt.connect();
            if(connected) {
                Toast.makeText(this, "Connected to " + btGatt.getDevice().getName(),
                        Toast.LENGTH_SHORT).show();
                Log.d("NAS>>>", "Sending result=" + str);
                characteristicTX.setValue(tx);
                btGatt.writeCharacteristic(characteristicTX);
                btGatt.setCharacteristicNotification(characteristicRX, true);
            }
            else {
                Toast.makeText(this, "Unable to connect to " + btGatt.getDevice().getName(),
                        Toast.LENGTH_SHORT).show();
            }
        }
    }
}
