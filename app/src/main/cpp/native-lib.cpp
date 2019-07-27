#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_WARN,"yuvOpenGlDemo",__VA_ARGS__)

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