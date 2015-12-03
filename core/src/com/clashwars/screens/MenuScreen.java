package com.clashwars.screens;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.clashwars.game.GameRenderer;
import com.clashwars.game.GameWorld;
import com.clashwars.side.AssetLoader;
import com.clashwars.side.MenuInputHandler;


public class MenuScreen implements Screen {
    // Screen Display Constants
    public static final int WIDTH = (int) (Gdx.graphics.getWidth());
    public static final int HEIGHT = (int)(Gdx.graphics.getHeight());
    int result;

    Game game;
    private OrthographicCamera camera;


    public MenuScreen(Game game, int result) {
        Gdx.input.setInputProcessor(new MenuInputHandler(game));
        this.game = game;
        camera = new OrthographicCamera(WIDTH, HEIGHT);
        this.result = result;
    }

    @Override
    public void show() {
        Gdx.app.log("GameScreen", "Attached");
    }

    @Override
    public void render(float delta) {

        SpriteBatch batch = new SpriteBatch();
        batch.begin();
        batch.draw(AssetLoader.background,0,0,WIDTH,HEIGHT);
//
//        if(result == 1)    batch.draw(AssetLoader.win,WIDTH/2,(int)(HEIGHT/1.5));
//        if(result == -1)   batch.draw(AssetLoader.loss,WIDTH/2,(int)(HEIGHT/1.5));
//        if(result == 0)    batch.draw(AssetLoader.title,WIDTH/2,(int)(HEIGHT/1.5));
        batch.end();
    }

    @Override
    public void resize(int width, int height) {
        Gdx.app.log("GameScreen", "resizing");
    }

    @Override
    public void pause() {
        Gdx.app.log("GameScreen", "show called");
    }

    @Override
    public void resume() {
        Gdx.app.log("GameScreen", "hide called");
    }

    @Override
    public void hide() {
        Gdx.app.log("GameScreen", "pause called");
    }

    @Override
    public void dispose() {
        Gdx.app.log("GameScreen", "resume called");
    }
}
