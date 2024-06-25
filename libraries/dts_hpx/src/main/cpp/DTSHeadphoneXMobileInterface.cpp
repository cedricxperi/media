/*======================================================================*
 @copyright Copyright (c) 2024 Xperi Corporation / DTS, Inc.

 DTS, Inc.
 5220 Las Virgenes Road
 Calabasas, CA 91302  USA

 CONFIDENTIAL: CONTAINS CONFIDENTIAL PROPRIETARY INFORMATION OWNED BY
 DTS, INC. AND/OR ITS AFFILIATES ("DTS"), INCLUDING BUT NOT LIMITED TO
 TRADE SECRETS, KNOW-HOW, TECHNICAL AND BUSINESS INFORMATION. USE,
 DISCLOSURE OR DISTRIBUTION OF THE SOFTWARE IN ANY FORM IS LIMITED TO
 SPECIFICALLY AUTHORIZED LICENSEES OF DTS.  ANY UNAUTHORIZED
 DISCLOSURE IS A VIOLATION OF STATE, FEDERAL, AND INTERNATIONAL LAWS.
 BOTH CIVIL AND CRIMINAL PENALTIES APPLY.

 DO NOT DUPLICATE. COPYRIGHT 2024, DTS, INC. ALL RIGHTS RESERVED.
 UNAUTHORIZED DUPLICATION IS A VIOLATION OF STATE, FEDERAL AND
 INTERNATIONAL LAWS.

 ALGORITHMS, DATA STRUCTURES AND METHODS CONTAINED IN THIS SOFTWARE
 MAY BE PROTECTED BY ONE OR MORE PATENTS OR PATENT APPLICATIONS.
 UNLESS OTHERWISE PROVIDED UNDER THE TERMS OF A FULLY-EXECUTED WRITTEN
 AGREEMENT BY AND BETWEEN THE RECIPIENT HEREOF AND DTS, THE FOLLOWING
 TERMS SHALL APPLY TO ANY USE OF THE SOFTWARE (THE "PRODUCT") AND, AS
 APPLICABLE, ANY RELATED DOCUMENTATION:  (i) ANY USE OF THE PRODUCT
 AND ANY RELATED DOCUMENTATION IS AT THE RECIPIENT'S SOLE RISK:
 (ii) THE PRODUCT AND ANY RELATED DOCUMENTATION ARE PROVIDED "AS IS"
 AND WITHOUT WARRANTY OF ANY KIND AND DTS EXPRESSLY DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDLESS OF WHETHER DTS KNOWS OR HAS REASON TO KNOW OF THE
 USER'S PARTICULAR NEEDS; (iii) DTS DOES NOT WARRANT THAT THE PRODUCT
 OR ANY RELATED DOCUMENTATION WILL MEET USER'S REQUIREMENTS, OR THAT
 DEFECTS IN THE PRODUCT OR ANY RELATED DOCUMENTATION WILL BE
 CORRECTED; (iv) DTS DOES NOT WARRANT THAT THE OPERATION OF ANY
 HARDWARE OR SOFTWARE ASSOCIATED WITH THIS DOCUMENT WILL BE
 UNINTERRUPTED OR ERROR-FREE; AND (v) UNDER NO CIRCUMSTANCES,
 INCLUDING NEGLIGENCE, SHALL DTS OR THE DIRECTORS, OFFICERS, EMPLOYEES,
 OR AGENTS OF DTS, BE LIABLE TO USER FOR ANY INCIDENTAL, INDIRECT,
 SPECIAL, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, AND LOSS
 OF BUSINESS INFORMATION) ARISING OUT OF THE USE, MISUSE, OR INABILITY
 TO USE THE PRODUCT OR ANY RELATED DOCUMENTATION.
 *======================================================================*/

/**
 * @file dtsHeadphoneXMobileInterface.cpp
 * A set of APIs that can be used to virtualise frames
 * of multichannel audio into a binaural (stereo) output.
 */

#include "include/DTSHeadphoneXMobile.h"
#include <jni.h>
#include <string>

/* Variable Declarations */
DTSHeadphoneXMobileInstance* pDTSHeadphoneXMobileInstance = nullptr;

/* Function Prototypes */
jobject NewInteger(JNIEnv* env, int value);

/*------------------------------- Public Functions -------------------------------- */

/**
 * \brief Returns the DTS Headphone:X Mobile version.
 * 
 * \return
 * - String containing the DTS Headphone:X Mobile version.
 */
extern "C" JNIEXPORT jstring JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileVersion (JNIEnv *env, jobject) {
    jstring sdkVersion = env->NewStringUTF(DTSHeadphoneXMobile_Version());
    return sdkVersion;
}


/**
 * \brief Installs the license key.
 *
 * Installs the license key for the DTSHeadphoneXMobileInstance. This must be called with valid
 * license key data before PCM inputs can be processed.
 *
 * \param licenseData A buffer containing all of the data from the license key file.
 *
 * \return
 * - DTS_SUCCESS when the license key has been installed successfully.
 * - DTS_NOT_INITIALISED when the license key has not been installed because the
 *                       DTSHeadphoneXMobileInstance has not yet been initialised.
 * - DTS_DSEC_AUTHORISATION_FAILED if the license key authorisation failed.
 */
extern "C" JNIEXPORT jint JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileInstallLicense (
        JNIEnv *env,
        jobject obj,
        jbyteArray licenseData) {
    uint32_t sdkResult = DTS_SUCCESS;

    auto nativeLicenseDataSize = (uint32_t)env->GetArrayLength(licenseData);
    jbyte* elements = env->GetByteArrayElements(licenseData, nullptr);
    auto * nativeLicenseData = (const unsigned char *)elements;

    sdkResult = DTSHeadphoneXMobile_InstallLicense(pDTSHeadphoneXMobileInstance,
                                                   nativeLicenseData,
                                                   nativeLicenseDataSize);
    env->ReleaseByteArrayElements(licenseData,elements,0);

    return sdkResult;
}

/**
 * \brief Retrieves the current state of the DTS Effect.
 *
 * Retrieves the current state of the DTS Effect. If the DTS Effect is enabled, the audio output
 * from the ...DTSHeadphoneXMobileProcessPCM... functions will be multichannel audio virtualised into
 * a binaural (stereo) output for headphones. If the DTS Effect is disabled, the audio output
 * from the ...DTSHeadphoneXMobileProcessPCM... functions will be multichannel audio downmixed to a
 * stereo output.
 *
 * \return
 * - jobjectArray object array with the following parameters.
 * - Element 0 DTS_SUCCESS on success.
 * - Element 0 DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - Element 0 DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 * - Element 1 TRUE if the DTS Effect is enabled.
 * - Element 1 FALSE if the DTS Effect is disabled.
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileIsDTSEffectEnabled (
        JNIEnv *env,
        jobject obj) {
    uint32_t sdkResult = DTS_SUCCESS;
    bool isEffectEnabled = false;
    _jobject *isDTSEffectEnabled;
    const jsize retObjArrLength = 2;
    auto retObjArr = (jobjectArray)env->NewObjectArray(retObjArrLength, env->FindClass("java/lang/Object"), nullptr);

    sdkResult = DTSHeadphoneXMobile_IsDTSEffectEnabled(pDTSHeadphoneXMobileInstance,
                                                       &isEffectEnabled);
    // Retrieve boolean result
    env->SetBooleanField(isDTSEffectEnabled, 0, isEffectEnabled);

    // Add return parameters to object
    env->SetObjectArrayElement(retObjArr, 0, NewInteger(env, sdkResult));
    env->SetObjectArrayElement(retObjArr, 1, isDTSEffectEnabled);
    return retObjArr;
}

/**
 * \brief Sets the state of the DTS Effect.
 *
 * Sets the state of the DTS Effect. If the DTS Effect is enabled, the audio output from the
 * ...DTSHeadphoneXMobileProcessPCM... functions will be multichannel audio virtualised into a
 * binaural (stereo) output for headphones. If the DTS Effect is disabled, the audio output from
 * the ...DTSHeadphoneXMobileProcessPCM... functions will be multichannel audio downmixed
 * to a stereo output.
 *
 * \param dtsEffectEnabled If value is true, the DTS Effect will be enabled. If value is
 *        false, the DTS Effect will be disabled.
 *
 * \return
 * - DTS_SUCCESS on success.
 * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 */
extern "C" JNIEXPORT jint JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileSetDTSEffectEnabled (
        JNIEnv *env,
        jobject obj,
        jboolean dtsEffectEnabled) {
    uint32_t sdkResult = DTS_SUCCESS;

    // Set the dts effect
    sdkResult = DTSHeadphoneXMobile_SetDTSEffectEnabled(pDTSHeadphoneXMobileInstance,
                                                        dtsEffectEnabled);

    return sdkResult;
}

/**
 * \brief Returns the required size of the output buffer in bytes.
 *
 * Returns the required size in bytes of the output buffer pointed to by the pOutputBuffer
 * parameter of the DTSHeadphoneXMobile_Process... functions. Memory for this buffer must be
 * allocated by the calling application before calling a Process function.
 *
 * \return
 * - uint32_t value for the size of the output buffer in bytes.
 */
extern "C" JNIEXPORT jint JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileGetOutputBufferSizeInBytes(
        JNIEnv *env,
        jobject obj){
    return DTSHeadphoneXMobile_GetOutputBufferSizeInBytes();
}

/**
 * \brief Initialises a DTSHeadphoneXMobileInstance for PCM.
 *
 * Allocates memory for a DTSHeadphoneXMobileInstance 
 * and then initialises it for use with PCM input data.
 * 
 * \param channelCount The number of audio channels to be processed. Supported options are:
 *        - 2: 2-channel 2.0 stereo input layout (L, R)
 *        - 6: 6-channel 5.1 input layout (L, R, C, LFE, Ls, Rs)
 *        - 8: 8-channel 7.1 input layout (L, R, C, LFE, Lss, Rss, Lrs, Rrs)
 *        - 10: 10-channel 5.1.4 input layout (L, R, C, LFE, Ls, Rs, Lfh, Rfh, Lrh, Rrh)
 * \param sampleRate The sample rate in Hz of audio to be processed. Supported options are:
 *        - 44100: 44.1kHz sample rate
 *        - 48000: 48kHz sample rate
 *        - 96000: 96kHz sample rate
 * \param stereoMode The stereo mode used when processing audio. Supported options are:
 *        - DTS_HEADPHONE_X_MOBILE_STEREO_MODE_DIRECT
 *        - DTS_HEADPHONE_X_MOBILE_STEREO_MODE_WIDE
 *        - DTS_HEADPHONE_X_MOBILE_STEREO_MODE_FRONT
 *        - DTS_HEADPHONE_X_MOBILE_STEREO_MODE_UPMIX
 *        Stereo modes other than direct are only available for 2-channel (stereo) inputs.
 *
 * \return
 * - DTS_SUCCESS on success.
 * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - DTS_MALLOC_ERROR if there is an error allocating memory.
 * - DTS_ERROR if an error occurred.
 */
extern "C" JNIEXPORT jint JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileInitialisePCM(
        JNIEnv *env,jobject obj, jint channelCount, jint sampleRate, jint stereoMode) {

    uint32_t sdkResult = DTS_SUCCESS;
    sdkResult = DTSHeadphoneXMobile_InitialisePCM(&pDTSHeadphoneXMobileInstance,
                                                  channelCount, sampleRate, static_cast<DTSHeadphoneXMobileStereoMode>(stereoMode));

    return sdkResult;
}


/**
 * \brief Takes a buffer of non-interleaved PCM audio samples stored as 32-bit floating-point as
 * input and processes it into a stereo output.
 *
 * Takes a buffer of non-interleaved PCM audio samples as input and processes it into a binaural
 * (stereo) output for headphones. The input buffer must contain 1024 audio samples per channel.
 * Output audio samples are stored as 32-bit floating-point. The output channel layout will
 * always be 2 channel (Left and Right) stereo. The output audio samples will be interleaved
 * (Left sample, Right sample, Left sample, Right sample etc).
 * The number of audio samples in the output buffer for each channel will match the number of
 * audio samples per channel that was input, which is always 1024 audio samples per channel.
 * Since the output buffer is interleaved and the output is always stereo (2 channels), the
 * total number of audio samples in the output buffer will be 2048. Each sample is 4 bytes,
 * which means the buffer size in bytes would be 8192 (this corresponds to the value returned by
 * DTSHeadphoneXMobile_GetOutputBufferSizeInBytes).
 *
 * \param inputData A buffer containing the audio input samples stored as
 *        non-interleaved 32-bit floating-point in a 2D array. The input buffer must contain
 *        1024 audio samples per channel. The expected channel order for each input layout is:
 *        - 2.0: Left, Right
 *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
 *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
 *          Left Rear Surround, Right Rear Surround
 *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
 *          Right Front Height, Left Rear Height, Right Rear Height
 * \param outputBuffer A buffer that will contain the processed output audio samples
 *        after this function has been called, stored as interleaved 32-bit floating-point.
 *        Memory for this buffer must be pre-allocated by the calling application.
 *
 * \return
 * - jobjectArray object array with the following parameters.
 * - Element 0 DTS_SUCCESS if audio has been decoded and processed.
 * - Element 0 DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - Element 0 DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 * - Element 0 DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
 * - Element 0 DTS_ERROR if an error occurred.
 * - Element 1 Processed data in a float array
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileProcessPCMNonInterleavedFloat(
        JNIEnv *env,
        jobject,
        jfloatArray inputData,
        jfloatArray outputBuffer) {

    auto *pInputData = (u_int8_t *)env->GetFloatArrayElements(inputData, nullptr);
    float *pOutput = env->GetFloatArrayElements(outputBuffer, nullptr);
    uint32_t result = 0;
    const jsize retObjArrLength = 2;
    auto retObjArr = (jobjectArray)env->NewObjectArray(retObjArrLength, env->FindClass("java/lang/Object"), nullptr);

    result = DTSHeadphoneXMobile_ProcessPCMNonInterleavedFloat(pDTSHeadphoneXMobileInstance,
                                                               reinterpret_cast<float *const *>(pInputData), pOutput);

    // Release input buffer
    env->ReleaseFloatArrayElements(inputData, (jfloat *)pInputData, JNI_ABORT);

    // Place output buffers in an array
    uint32_t  numSamplesInArray = DTSHeadphoneXMobile_GetOutputBufferSizeInBytes() / sizeof(float);
    env->SetFloatArrayRegion(outputBuffer, 0,
                             numSamplesInArray,
                             pOutput);

    // Add return parameters and virtualised audio to object
    env->SetObjectArrayElement(retObjArr, 0, NewInteger(env, result));
    env->SetObjectArrayElement(retObjArr, 1, outputBuffer);

    return retObjArr;
}

/**
 * \brief Takes a buffer of interleaved PCM audio samples stored as 32-bit floating-point as
 * input and processes it into a stereo output.
 *
 * Takes a buffer of interleaved PCM audio samples as input and processes it into a binaural
 * (stereo) output for headphones. The input buffer must contain 1024 audio samples per channel.
 * Output audio samples are stored as 32-bit floating-point. The output channel layout will
 * always be 2 channel (Left and Right) stereo. The output audio samples will be interleaved
 * (Left sample, Right sample, Left sample, Right sample etc).
 * The number of audio samples in the output buffer for each channel will match the number of
 * audio samples per channel that was input, which is always 1024 audio samples per channel.
 * Since the output buffer is interleaved and the output is always stereo (2 channels), the
 * total number of audio samples in the output buffer will be 2048. Each sample is 4 bytes,
 * which means the buffer size in bytes would be 8192 (this corresponds to the value returned by
 * DTSHeadphoneXMobile_GetOutputBufferSizeInBytes).
 *
 * \param inputData A buffer containing the audio input samples stored as
 *        interleaved 32-bit floating-point. The input buffer must contain 1024 audio samples
 *        per channel. The expected channel order for each input layout is:
 *        - 2.0: Left, Right
 *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
 *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
 *          Left Rear Surround, Right Rear Surround
 *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
 *          Right Front Height, Left Rear Height, Right Rear Height
 * \param outputBuffer A buffer that will contain the processed output audio samples
 *        after this function has been called, stored as interleaved 32-bit floating-point.
 *        Memory for this buffer must be pre-allocated by the calling application.
 *
 * \return
 * - jobjectArray object array with the following parameters.
 * - Element 0 DTS_SUCCESS if audio has been decoded and processed.
 * - Element 0 DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - Element 0 DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 * - Element 0 DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
 * - Element 0 DTS_ERROR if an error occurred.
 * - Element 1 Processed data in a float array
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileProcessPCMInterleavedFloat(
        JNIEnv *env,
        jobject,
        jfloatArray inputData,
        jfloatArray outputBuffer) {

    float *pInputData = env->GetFloatArrayElements(inputData, nullptr);
    float *pOutput = env->GetFloatArrayElements(outputBuffer, nullptr);
    uint32_t result = 0;
    const jsize retObjArrLength = 2;
    auto retObjArr = (jobjectArray)env->NewObjectArray(retObjArrLength, env->FindClass("java/lang/Object"), nullptr);

    result = DTSHeadphoneXMobile_ProcessPCMInterleavedFloat(pDTSHeadphoneXMobileInstance,
                                                            pInputData,
                                                            pOutput);

    // Release input buffer
    env->ReleaseFloatArrayElements(inputData, (jfloat *)pInputData, JNI_ABORT);

    // Place output buffers in an array
    uint32_t  numSamplesInArray = DTSHeadphoneXMobile_GetOutputBufferSizeInBytes() / sizeof(float);
    env->SetFloatArrayRegion(outputBuffer, 0,
                             numSamplesInArray,
                             pOutput);

    // Add return result and virtualised audio to object
    env->SetObjectArrayElement(retObjArr, 0, NewInteger(env, result));
    env->SetObjectArrayElement(retObjArr, 1, outputBuffer);

    return retObjArr;
}

/**
 * \brief Takes a buffer of non-interleaved PCM audio samples stored as 32-bit integers as input
 * and processes it into a stereo output.
 *
 * Takes a buffer of non-interleaved PCM audio samples as input and processes it into a binaural
 * (stereo) output for headphones. The input buffer must contain 1024 audio samples per channel.
 * Output audio samples are stored as 32-bit integers. The output channel layout will always be
 * 2 channel (Left and Right) stereo. The output audio samples will be interleaved (Left sample,
 * Right sample, Left sample, Right sample etc).
 * The number of audio samples in the output buffer for each channel will match the number of
 * audio samples per channel that was input, which is always 1024 audio samples per channel.
 * Since the output buffer is interleaved and the output is always stereo (2 channels), the
 * total number of audio samples in the output buffer will be 2048. Each sample is 4 bytes,
 * which means the buffer size in bytes would be 8192 (this corresponds to the value returned by
 * DTSHeadphoneXMobile_GetOutputBufferSizeInBytes).
 *
 * \param inputData A buffer containing the audio input samples stored as
 *        non-interleaved 32-bit integers in a 2D array. The input buffer must contain 1024
 *        audio samples per channel. The expected channel order for each input layout is:
 *        - 2.0: Left, Right
 *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
 *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
 *          Left Rear Surround, Right Rear Surround
 *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
 *          Right Front Height, Left Rear Height, Right Rear Height
 * \param outputBuffer A buffer that will contain the processed output audio samples
 *        after this function has been called, stored as interleaved 32-bit integers. Memory for
 *        this buffer must be pre-allocated by the calling application.
 *
 * \return
 * - jobjectArray object array with the following parameters.
 * - Element 0 DTS_SUCCESS if audio has been decoded and processed.
 * - Element 0 DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - Element 0 DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 * - Element 0 DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
 * - Element 0 DTS_ERROR if an error occurred.
 * - Element 1 Processed data in an int array
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileProcessPCMNonInterleavedInteger(
        JNIEnv *env,
        jobject,
        jintArray inputData,
        jintArray outputBuffer) {

    int32_t *const * pInputData = reinterpret_cast<int32_t *const *>(env->GetIntArrayElements(
            inputData, nullptr));
    jint *pOutput = env->GetIntArrayElements(outputBuffer, nullptr);
    uint32_t result = 0;
    const jsize retObjArrLength = 2;
    auto retObjArr = (jobjectArray)env->NewObjectArray(retObjArrLength, env->FindClass("java/lang/Object"), nullptr);

    result = DTSHeadphoneXMobile_ProcessPCMNonInterleavedInteger(pDTSHeadphoneXMobileInstance,
                                                                 pInputData,
                                                                 reinterpret_cast<int32_t *>(pOutput));

    // Release input buffer
    env->ReleaseIntArrayElements(inputData, (jint *)pInputData, JNI_ABORT);

    // Place output buffers in an array
    uint32_t  numSamplesInArray = DTSHeadphoneXMobile_GetOutputBufferSizeInBytes() / sizeof(int32_t);
    env->SetIntArrayRegion(outputBuffer, 0,
                             numSamplesInArray,
                             pOutput);

    // Add return parameters and virtualised audio to object
    env->SetObjectArrayElement(retObjArr, 0, NewInteger(env, result));
    env->SetObjectArrayElement(retObjArr, 1, outputBuffer);

    return retObjArr;
}

/**
 * \brief Takes a buffer of interleaved PCM audio samples stored as 32-bit integers as input and
 * processes it into a stereo output.
 *
 * Takes a buffer of interleaved PCM audio samples as input and processes it into a binaural
 * (stereo) output for headphones. The input buffer must contain 1024 audio samples per channel.
 * Output audio samples are stored as 32-bit integers. The output channel layout will always be
 * 2 channel (Left and Right) stereo. The output audio samples will be interleaved (Left sample,
 * Right sample, Left sample, Right sample etc).
 * The number of audio samples in the output buffer for each channel will match the number of
 * audio samples per channel that was input, which is always 1024 audio samples per channel.
 * Since the output buffer is interleaved and the output is always stereo (2 channels), the
 * total number of audio samples in the output buffer will be 2048. Each sample is 4 bytes,
 * which means the buffer size in bytes would be 8192 (this corresponds to the value returned by
 * DTSHeadphoneXMobile_GetOutputBufferSizeInBytes).
 *
 * \param inputData A buffer containing the audio input samples stored as
 *        interleaved 32-bit integers. The input buffer must contain 1024 audio samples pe
 *        channel. The expected channel order for each input layout is:
 *        - 2.0: Left, Right
 *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
 *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
 *          Left Rear Surround, Right Rear Surround
 *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
 *          Right Front Height, Left Rear Height, Right Rear Height
 * \param outputBuffer A buffer that will contain the processed output audio samples
 *        after this function has been called, stored as interleaved 32-bit integers. Memory for
 *        this buffer must be pre-allocated by the calling application.
 *
 * \return
 * - jobjectArray object array with the following parameters.
 * - Element 0 DTS_SUCCESS if audio has been decoded and processed.
 * - Element 0 DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - Element 0 DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
 * - Element 0 DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
 * - Element 0 DTS_ERROR if an error occurred.
 * - Element 1 Processed data in an int array
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileProcessPCMInterleavedInteger(
        JNIEnv *env,
        jobject,
        jintArray inputData,
        jintArray outputBuffer) {

    jint *pInputData = env->GetIntArrayElements(inputData, nullptr);
    jint *pOutput = env->GetIntArrayElements(outputBuffer, nullptr);
    uint32_t result = 0;
    const jsize retObjArrLength = 2;
    auto retObjArr = (jobjectArray)env->NewObjectArray(retObjArrLength, env->FindClass("java/lang/Object"), nullptr);

    result = DTSHeadphoneXMobile_ProcessPCMInterleavedInteger(pDTSHeadphoneXMobileInstance,
                                                              pInputData,
                                                              reinterpret_cast<int32_t *>(pOutput));

    // Release input buffer
    env->ReleaseIntArrayElements(inputData, (jint *)pInputData, JNI_ABORT);

    // Place output buffers in an array
    uint32_t  numSamplesInArray = DTSHeadphoneXMobile_GetOutputBufferSizeInBytes() / sizeof(int32_t);
    env->SetIntArrayRegion(outputBuffer, 0,
                             numSamplesInArray,
                             pOutput);

    // Add return result and virtualised audio to object
    env->SetObjectArrayElement(retObjArr, 0, NewInteger(env, result));
    env->SetObjectArrayElement(retObjArr, 1, outputBuffer);

    return retObjArr;
}

/**
 * \brief Releases all memory and resources for a DTSHeadphoneXMobileInstance for PCM.
 *
 * Deinitialises a DTSHeadphoneXMobileInstance that was initialised using
 * DTSHeadphoneXMobile_InitialisePCM and deallocates all memory for the instance.
 * 
 * \return
 * - DTS_SUCCESS on success.
 * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
 * - DTS_ERROR if an error occurred.
 */
extern "C" JNIEXPORT jint JNICALL Java_androidx_media3_dts_hpx_DtsHpxProcess_DTSHeadphoneXMobileReleasePCM (JNIEnv *env, jobject) {
    return DTSHeadphoneXMobile_ReleasePCM(&pDTSHeadphoneXMobileInstance);
}

/* Support Functions */

/**
 * \brief Creates and returns new object
 */
jobject NewInteger(JNIEnv* env, int value){
    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
    return env->NewObject(integerClass, integerConstructor, static_cast<jint>(value));
}