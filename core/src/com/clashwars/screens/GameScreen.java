package com.clashwars.screens;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Screen;
import com.clashwars.game.Connection;
import com.clashwars.game.GameRenderer;
import com.clashwars.game.GameWorld;


public class GameScreen implements Screen{
    public Connection c;
    public GameRenderer renderer;

    Game game;

    public GameScreen(Game game, Connection c){
        this.game = game;
        this.c = c;
        renderer = new GameRenderer(game, new GameWorld(),c);
    }

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
