package com.clashwars.game;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.clashwars.screens.GameScreen;
import com.clashwars.screens.MenuScreen;
import com.clashwars.side.AssetLoader;

public class ClashWarsGame extends Game {

	private MenuScreen menu;
	private GameScreen game;

	@Override
	public void create () {

		AssetLoader.load();
		menu = new MenuScreen();
		game = new GameScreen();
		setScreen(menu);
	}

	@Override
	public void render () {
		menu.render(0.1f);
	}

	@Override
	public void dispose() {
		super.dispose();
		AssetLoader.dispose();
	}
}
