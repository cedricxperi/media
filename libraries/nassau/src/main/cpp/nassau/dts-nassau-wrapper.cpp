#include <jni.h>
#include <string>
#include "nassau-v2c-inference-api.h"
#include <time.h>
#include <android/log.h>

#ifdef LOG_NDEBUG
#define ALOGD(...)   ((void)0)
#else
#define LOG_TAG "NASSAU-JNI-LOG"
#define ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

extern "C" JNIEXPORT jlong JNICALL
Java_androidx_media3_nassau_NassauProcess_NassauAlloccate(
        JNIEnv* env,
        jclass clazz,
        jobject Weights,
        jint nWeightSize,
        jint numBands
        )
{
    NassauInferenceV2c obj;
    void *pWght = env->GetDirectBufferAddress( Weights );
    int rc = NassauInference_V2c_Allocate( &obj,
                                           pWght,
                                           nWeightSize,
                                           numBands);
    if(rc == 0)
    {
        return (jlong) (obj);
    }else
    {
        return 0;
    }
}


extern "C" JNIEXPORT jint JNICALL
Java_androidx_media3_nassau_NassauProcess_NassauProcess(
        JNIEnv* env,
        jclass clazz,
        jlong obj_addr,
        jfloatArray input_frame,
        jfloatArray output_frame,
        jint samples
)
{
    jboolean isCopyIn, isCopyKey;
    int32_t rc;
    jfloat pcm_in_addr[1024], pcm_out_addr[1024];
    clock_t start, end;
    unsigned long micros = 0;

    NassauInferenceV2c obj;
    obj = (NassauInferenceV2c)obj_addr;
    env->GetFloatArrayRegion( input_frame, 0, env->GetArrayLength( input_frame ), pcm_in_addr );

    start = clock();

    rc = NassauInference_V2c_ProcessFrame( obj,
                                   pcm_in_addr,
                                   pcm_out_addr,
                                   samples );
    end = clock();
    micros = end - start;
    ALOGD("Elaspsed Time in Micro Seconds: %d\n",(int) micros);
//    double elapsed = double(end - start)/CLOCKS_PER_SEC;
//    ALOGD("Elaspsed Time in Micro Seconds: %f\n", elapsed);

    env->SetFloatArrayRegion( output_frame, 0, env->GetArrayLength( output_frame ), pcm_out_addr );
    return rc;
}

extern "C" JNIEXPORT jint JNICALL
Java_androidx_media3_nassau_NassauProcess_NassauDeallocate(
        JNIEnv* env,
        jclass clazz,
        jlong obj_addr
)
{
    NassauInferenceV2c obj;
    obj = (NassauInferenceV2c)obj_addr;
    return NassauInference_V2c_Deallocate(obj);
}

extern "C"
JNIEXPORT jint JNICALL
Java_androidx_media3_nassau_NassauProcess_NassauSetThreads(JNIEnv *env,
                                                                                jclass clazz,
                                                                                jint n_threads) {
    return NassauInference_SetNumThreads(n_threads);
}
