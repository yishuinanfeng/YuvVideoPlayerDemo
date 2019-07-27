package com.example.yuvopengldemo;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

public class YuvPlayer extends GLSurfaceView implements Runnable, SurfaceHolder.Callback {

    private final static String PATH = "";

    public YuvPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        new Thread(this).start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {

    }

    @Override
    public void run() {
        loadYuv(PATH,getHolder().getSurface());
    }

    public native void loadYuv(String url, Object surface);
}
