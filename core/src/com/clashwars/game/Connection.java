package com.clashwars.game;


import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Net;
import com.badlogic.gdx.net.Socket;
import com.badlogic.gdx.net.SocketHints;

public class Connection {
    Socket socket;

    Connection(){
        socket = Gdx.net.newClientSocket(Net.Protocol.TCP,"52.27.37.18",2200,new SocketHints());
    }
}
