package com.clashwars.screens;

import com.badlogic.gdx.Screen;
import com.clashwars.game.GameRenderer;
import com.clashwars.game.GameWorld;


public class GameScreen implements Screen{
    GameRenderer renderer = new GameRenderer(new GameWorld());

    @Override
    public void show() {

    }

    @Override
    public void render(float delta) {
        renderer.render(delta);
    }

    @Override
    public void resize(int width, int height) {

    }

    @Override
    public void pause() {

    }

    @Override
    public void resume() {

    }

    @Override
    public void hide() {

    }

    @Override
    public void dispose() {

    }
}
