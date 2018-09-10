// *************************************************************
// Titre : Application Android de télémétrie pour projet ELE3000
// Auteur : Simon Bellemare, Polytechnique Montréal
// Date : 15 avril 2014
// *************************************************************

package com.polyeracing.telemetrie;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;
import java.nio.charset.Charset;
import java.text.DecimalFormat;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Color;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener {
	  public static final String SERVERIP = "192.168.16.255";
      public static final int SERVERPORT = 50000;
      
      public CharSequence cs_connecte = "CONNECTÉ";
      public CharSequence cs_deconnecte = "DÉCONNECTÉ";
  	  
  	  
      public TextView tension_totale;
      public TextView tv_wifi_status;
      public TextView tv_interlock_state;
      public TextView tv_vitesse;
      public TextView tv_temp_batt;
      public int heartbeat = 0;
      public int int_tension_totale;
      public int int_vitesse = 0;
      public int int_temp_batt = 0;
      public double dbl_tension_totale;
      public double dbl_temp_batt;
      public String str_tension_totale;
      public String str_interlock_state = "N/D";
      public String str_vitesse = "N/D";
      public String str_temp_batt = "N/D";
      public int intCanId;
      public int intCanData;
      public int i = 0;
      public boolean start;
      public Handler Handler;
      private ProgressBar tension_total_bar;
      public WifiInfo w;
      
    /** Called when the activity is first created. */
    @SuppressLint("HandlerLeak")
	@Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.fragment_main);
      
         tension_totale=(TextView)findViewById(R.id.txt_total_voltage);
         tv_interlock_state=(TextView)findViewById(R.id.txt_interlock_state);
         tv_vitesse=(TextView)findViewById(R.id.txt_speed);
         tv_temp_batt=(TextView)findViewById(R.id.txt_temp_batt);
         tension_total_bar = (ProgressBar) findViewById(R.id.progressBar);
         tv_wifi_status=(TextView)findViewById(R.id.txt_wifi_status);
         dbl_tension_totale = 0.0;
         
         WifiManager mWiFiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
         w = mWiFiManager.getConnectionInfo();
         Toast.makeText(this, "Connecté à "+w.getSSID(), Toast.LENGTH_SHORT).show();
         
         new Thread(new Server()).start();
        
	     Handler = new Handler() {  
		     @Override
		     public void handleMessage(Message msg) {
		    	 intCanId = msg.arg1;
		    	 intCanData = msg.arg2;
		    	 heartbeat++;
		    	 tv_wifi_status.setTextColor(Color.GREEN); 
             	 tv_wifi_status.setText(cs_connecte);; 
		    	 
		    	 switch(msg.arg1) {
		    	 	// Interlock state
		    	 	case 0: 
		    	 		if(msg.arg2 == 0) {
		    	 			str_interlock_state = "Ouvert";
		    	 			tv_interlock_state.setText(str_interlock_state); 
		    	 			tv_interlock_state.setTextColor(Color.RED); 
		    	 		}
		    	 		else {
		    	 			str_interlock_state = "Fermé";
		    	 			tv_interlock_state.setText(str_interlock_state); 
		    	 			tv_interlock_state.setTextColor(Color.BLACK); 
		    	 		}
		    	 		break;
		    	 	// Vitesse
		    	 	case 2:
		    	 		int_vitesse = intCanData;
		    	 		int_vitesse = int_vitesse*200/3800;
		    	 		tv_vitesse.setText(String.valueOf(int_vitesse) + " km/h"); 
		    	 		break;
		    	 	// Température batterie
		    	 	case 1:
		    	 		int_temp_batt = intCanData;
		    	 		dbl_temp_batt = (double)int_temp_batt*65/3800;
		    	 		DecimalFormat df1 = new DecimalFormat("#.0");
		    	 		str_temp_batt = df1.format(dbl_temp_batt);
				    	tv_temp_batt.setText(str_temp_batt + "°C"); 
		    	 		break;
		    	 	// Tension totale
		    	 	case 3:
		    	 		int_tension_totale = intCanData;
				    	dbl_tension_totale = (double)int_tension_totale*300/3800;
				    	DecimalFormat df = new DecimalFormat("#");
				    	str_tension_totale = df.format(dbl_tension_totale);
				    	tension_totale.setText(str_tension_totale + " V"); 
				    	//str_tension_totale = "";
				    	if(int_tension_totale*100/3800 < 100) {
				    		tension_total_bar.setProgress(int_tension_totale*100/3800);
				    	}
				    	else {
				    		tension_total_bar.setProgress(100);
				    	}
				    	
		    	 		break;
		    	 	default: 
		    	 		break;
		    	 }
		     }
         };
    }
 
    public class Server implements Runnable {
    	private InetAddress serverAddr;
        private DatagramSocket socket;
        private byte[] buf = new byte[1024];
        private DatagramPacket packet;
        
        Server() {
        	try {
	        		serverAddr = InetAddress.getByName(SERVERIP);
		            socket = new DatagramSocket(SERVERPORT, serverAddr);
		            socket.setSoTimeout(5000);
		            packet = new DatagramPacket(buf, buf.length);
        	 
        	 } catch (Exception e) {
        	        // Exception
        	 }
        }
        
        @Override
        public void run() {
            while(true)
            {
                try {
                	packet.setData(new byte[1024]);
                      socket.receive(packet);
                      updateUI(new  String( packet.getData(), Charset.forName("US-ASCII")));

                } catch (SocketTimeoutException ste) {

                } catch (Exception e) {
                	tv_wifi_status.setTextColor(Color.RED); 
            	} 
            
            }
            
        }
    } // fin de la classe Server
    
    
      @Override
      public void onClick(View v) {
            start=true;
      }
      
      public void updateUI(String value){
          Message msg = new Message();
          
          String[] result = value.split(":");
          result[1] = result[1].trim();
    		
          msg.arg1 = Integer.parseInt(result[0]);
          msg.obj = result[1].toString();
          msg.arg2 = Integer.parseInt(result[1]);
          
          Handler.sendMessage(msg);
          
          
          
      }
      
      @Override
      public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setContentView(R.layout.fragment_main);
      }
     
}