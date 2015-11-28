package com.clashwars.side;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputProcessor;
import com.clashwars.game.Connection;
import com.clashwars.screens.GameScreen;

public class MenuInputHandler implements InputProcessor {
    Connection connection;

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
        //connection = new Connection();
        // After connection, display 'waiting for matchmaking message'.
        //String confirmMsg = new String();
        //while (!confirmMsg.equals("GAME FOUND"))
        //    confirmMsg = connection.read();

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
