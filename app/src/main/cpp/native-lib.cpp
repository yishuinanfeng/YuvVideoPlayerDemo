#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_WARN,"yuvOpenGlDemo",__VA_ARGS__)

//顶点着色器
#define GET_STR(x) #x
static const char *vertextShader = GET_STR(
        attribute vec4 aPosition;//输入的顶点坐标
        attribute vec2 aTextCoord;//输入的纹理坐标
        varying vec2 vTextCoord;//输出的纹理坐标
        void main() {
            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）
            aTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);
            gl_Position = aPosition;
        }
);

static const char *fragYUV420P = GET_STR(
        precision mediump float;
        varying vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform sample2D yTexture;
        uniform sample2D uTexture;
        uniform sample2D vTexture;
        void main(){
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r =texture2D(yTexture,vTextCoord).r;
            yuv.g =texture2D(uTexture,vTextCoord).r - 0.5;
            yuv.b =texture2D(vTexture,vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0,1.0,1.0
                    0.0,-0.39465,2.03211
                    1.13983,-0.5806,0.0
                    )*yuv;
            gl_FragColor = vec4(rgb,1.0);
        }
);


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_yuvopengldemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_yuvopengldemo_YuvPlayer_loadYuv(JNIEnv *env, jobject thiz, jstring jUrl,
                                                 jobject surface) {
    // TODO: implement loadYuv()
    const char *url = env->GetStringUTFChars(jUrl, 0);
    LOGD("open ulr is %s", url);
    //1.获取原始窗口
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    //获取Display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGD("egl display failed");
        return;
    }
    //2.初始化egl，后两个参数为主次版本号
    if (EGL_TRUE != eglInitialize(display, 0, 0)) {
        LOGD("eglInitialize failed");
        return;
    }

    //3.1 surface配置，可以理解为窗口
    EGLConfig eglConfig;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    if (EGL_TRUE != eglChooseConfig(display, configSpec, &eglConfig, 1, &configNum)) {
        LOGD("eglChooseConfig failed");
        return;
    }

    //3.2创建surface(egl和NativeWindow进行关联。最后一个参数为属性信息，0表示默认版本)
    EGLSurface winSurface = eglCreateWindowSurface(display, eglConfig, nwin, 0);
    if (winSurface == EGL_NO_SURFACE) {
        LOGD("eglCreateWindowSurface failed");
        return;
    }

    //4 创建关联上下文
    //EGL_NO_CONTEXT表示不需要多个设备共享上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    EGLContext context = eglCreateContext(display, eglConfig, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        LOGD("eglCreateContext failed");
        return;
    }
    //将egl和opengl关联
    //第二个surface用来读取的，一般用来离线渲染？
    if (EGL_TRUE != eglMakeCurrent(display, winSurface, winSurface, context)) {
        LOGD("eglMakeCurrent failed");
        return;
    }


}