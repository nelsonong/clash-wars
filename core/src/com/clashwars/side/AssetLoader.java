package com.clashwars.side;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;

/**
 * Created by Parth on 15-11-28.
 */
public class AssetLoader {
    public static Texture background;

    public static void load(){
        background = new Texture(Gdx.files.internal("background.jpg"));
    }

    public static void dispose(){
        background.dispose();
    }

}
