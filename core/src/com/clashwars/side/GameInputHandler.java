package com.clashwars.side;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputProcessor;
import com.clashwars.game.ClashWarsGame;
import com.clashwars.game.Connection;
import com.clashwars.screens.GameScreen;

public class GameInputHandler implements InputProcessor {
    public Game game;
    public Connection connection;

    public GameInputHandler(final Game game, Connection c){
        this.game = game;
        this.connection = c;

        new Thread(new Runnable() {
            @Override
            public void run() {
                while(true){
                    String response = connection.read();

                    if(response == null) {
                        continue;
                    }
                    else if(response == "998") {
                        Gdx.app.log("thread","lose code recieved");
                    }
                    else if(response == "999"){
                        Gdx.app.log("thread","win code recieved");
                    } else {
                        ClashWarsGame g = new ClashWarsGame(game);
                        ((GameScreen)g.screen).renderer.setPercent(Integer.parseInt(response));
                    }
                }
            }
        });
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
        connection.write("1");
        return true;
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
