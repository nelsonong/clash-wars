package com.clashwars.game;


import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Net;
import com.badlogic.gdx.net.Socket;
import com.badlogic.gdx.net.SocketHints;

import java.io.DataOutputStream;
import java.io.IOException;

public class Connection {
    // Server variables
    public static final String SERVER = "52.26.111.207";
    public static final int PORT = 2201;

    // Socket that handles communication with the server
    Socket socket;

    public Connection(){
        SocketHints hints = new SocketHints();
        // Attempt to connect socket with server
        try {
            socket = Gdx.net.newClientSocket(Net.Protocol.TCP, SERVER, PORT, hints);
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    public void write(String message){
        if(!socket.isConnected())
            Gdx.app.log("read()","Not connected");


        try { // Attempt to write to socket's output stream
            DataOutputStream out = new DataOutputStream(socket.getOutputStream());
            out.write(message.getBytes());
        } catch(IOException e){
            e.printStackTrace();
        }
    }

    public String read(){
        String response = null; // Holds the response
        int MAX_BUFFER_SIZE = 3;

        while(response == null) {
            try { // Attempt to read socket's input stream
                byte[] bytes = new byte[MAX_BUFFER_SIZE];
                 socket.getInputStream().read(bytes);
                response = new String(bytes);
                Gdx.app.log("read()",response);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return response;
    }
}
