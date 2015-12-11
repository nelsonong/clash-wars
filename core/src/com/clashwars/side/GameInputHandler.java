package com.clashwars.side;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputProcessor;
import com.clashwars.game.ClashWarsGame;
import com.clashwars.game.Connection;
import com.clashwars.game.GameRenderer;
import com.clashwars.screens.GameScreen;
import com.clashwars.screens.MenuScreen;

import java.io.IOException;

import jdk.jfr.events.ThrowablesEvent;

public class GameInputHandler implements InputProcessor {
    public Game game;
    public Connection connection;
    public GameRenderer r;

    public GameInputHandler(final Game game, Connection c, GameRenderer r){
        this.game = game;
        this.connection = c;
        this.r = r;

        new Thread(new Runnable() {
            @Override
            public void run() {
                while(true){
                    String response = connection.read();
                    Gdx.app.log("read()", response);

                    if(response == null) {
                        continue;
                    }
                    else if( "998".equals(response)) {
                        Gdx.app.log("thread","lose code recieved");
                        connection.dispose();
                        connection = null;
                        game.setScreen(new MenuScreen(game,-1,connection));
                        break;
                    }
                    else if("999".equals(response)){
                        Gdx.app.log("thread","win code recieved");
                        connection.dispose();
                        connection = null;
                        game.setScreen(new MenuScreen(game,1,connection));
                        break;
                    } else {
                        ClashWarsGame g = new ClashWarsGame(game);
                            ((GameScreen) g.screen).renderer.setPercent(Integer.parseInt(response));
                    }
                }
            }
        }).start();
    }
    @Override
    public boolean keyDown(int keycode) {
        return false;
    }

    @Override
    public boolean keyUp(int keycode) {
        return false;
    }

    @Override
    public boolean keyTyped(char character) {
        return false;
    }

    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {
       // if(r.percentIsChangeable) {
            connection.write("1");
            return true;
       // }
       // else
       //     return false;
    }

    @Override
    public boolean touchUp(int screenX, int screenY, int pointer, int button) {
        return false;
    }

    @Override
    public boolean touchDragged(int screenX, int screenY, int pointer) {
        return false;
    }

    @Override
    public boolean mouseMoved(int screenX, int screenY) {
        return false;
    }

    @Override
    public boolean scrolled(int amount) {
        return false;
    }
}
