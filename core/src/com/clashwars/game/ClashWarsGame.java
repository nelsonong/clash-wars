package com.clashwars.game;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.clashwars.screens.GameScreen;
import com.clashwars.screens.MenuScreen;
import com.clashwars.side.AssetLoader;

public class ClashWarsGame extends Game {
	public Connection connection = null;

	public ClashWarsGame(){

	}

	public ClashWarsGame(Game g){
		this.screen = g.getScreen();
	}
	@Override
	public void setScreen(Screen screen) {
		super.setScreen(screen);
		this.screen = screen;
	}

	public Screen screen;

	@Override
	public void create() {

		AssetLoader.load();
		screen = new MenuScreen(this,0,connection);
		setScreen(screen);
	}

	@Override
	public void render() {
		screen.render(1.0f);
	}

	@Override
	public void dispose() {
//		if(connection != null)
//			connection.write("-1");
		super.dispose();
		AssetLoader.dispose();
	}

}
