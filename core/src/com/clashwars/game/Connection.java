package com.clashwars.game;


import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Net;
import com.badlogic.gdx.net.Socket;
import com.badlogic.gdx.net.SocketHints;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Connection {
    // Server variables
    public static final String SERVER = "52.27.37.18";
    public static final int PORT = 2200;

    // Socket that handles communication with the server
    Socket socket;

    Connection(){
        SocketHints hints = new SocketHints();
        // Attempt to connect socket with server
        try {
            socket = Gdx.net.newClientSocket(Net.Protocol.TCP, SERVER, PORT, hints);
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    public void write(String message){

        try { // Attempt to write to socket's output stream
            socket.getOutputStream().write(message.getBytes());
        } catch(IOException e){
            e.printStackTrace();
        }
    }

    public String read(){
        String response = null; // Holds the response

        try{ // Attempt to read socket's input stream
            response = new BufferedReader(new InputStreamReader(socket.getInputStream())).readLine();
        } catch(IOException e){
            e.printStackTrace();
        }

        return null;
    }
}
