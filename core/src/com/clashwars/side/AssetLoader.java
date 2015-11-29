package com.clashwars.side;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

import java.awt.TextComponent;
import java.util.Arrays;


public class AssetLoader {
    public static Texture background, gokuSheet, buuSheet, gokuBeam, buuBeam, gokuClashSheet, buuClashSheet;
    public static Animation gokuAnimation, buuAnimation, gokuClashAnimation, buuClashAnimation;
    public static TextureRegion[] gokuFrames, buuFrames, gokuClash, buuClash;

    public static void load(){
        background = new Texture(Gdx.files.internal("background.jpg"));
        gokuSheet = new Texture(Gdx.files.internal("goku.png"));
        buuSheet = new Texture(Gdx.files.internal("kidbuu.png"));
        buuBeam = new Texture(Gdx.files.internal("kidbuu_light.png"));
        gokuBeam = new Texture(Gdx.files.internal("light.png"));
        gokuClashSheet = new Texture(Gdx.files.internal("goku_fire.png"));
        buuClashSheet = new Texture(Gdx.files.internal("kidbuu_fire.png"));
        gokuFrames = new TextureRegion[21];
        gokuClash = new TextureRegion[2];
        buuFrames = new TextureRegion[21];
        buuClash = new TextureRegion[2];

        gokuClash[0] = new TextureRegion(gokuClashSheet,26,0,139,496);
        gokuClash[1] = new TextureRegion(gokuClashSheet,226,0,139,496);
        buuClash[0] = new TextureRegion(buuClashSheet,38,0,139,496);
        buuClash[1] = new TextureRegion(buuClashSheet,237,0,139,496);

        final int PARSE_INT = 320;
        for(int i = 0; i < gokuFrames.length; i++){
            gokuFrames[i] = new TextureRegion(gokuSheet,PARSE_INT*i,0,PARSE_INT,gokuSheet.getHeight());
            buuFrames[buuFrames.length-i-1] = new TextureRegion(buuSheet,PARSE_INT*i,0,PARSE_INT,buuSheet.getHeight());
        }

        final float FRAME_SPEED = 0.75f;
        gokuAnimation = new Animation(FRAME_SPEED,gokuFrames);
        buuAnimation = new Animation(FRAME_SPEED,buuFrames);

        final float SLOW_RATE = 4.0f;
        gokuClashAnimation = new Animation(SLOW_RATE,gokuClash);
        buuClashAnimation = new Animation(SLOW_RATE,buuClash);
    }

    public static void dispose(){
        background.dispose();
        gokuSheet.dispose();
        buuSheet.dispose();
    }

}
