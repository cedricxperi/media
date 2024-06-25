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
 * @file DTSHeadphoneXMobile.h
 *
 * \brief A set of functions for virtualising multichannel audio
 * into a binaural (stereo) output for headphones.
 * 
 * The DTS Headphone:X Mobile SDK provides a set of "C" Application Programming
 * Interface (API) functions to the DTS Headphone:X Mobile library that can be
 * linked in and called by a host application.
 * These functions can be used to process multichannel PCM audio and virtualise
 * it into a binaural stereo output for headphones, delivering an immersive
 * experience to the listener.
 */

#ifndef __DTS_HEADPHONEX_MOBILE_H__
#define __DTS_HEADPHONEX_MOBILE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* Forward Declarations */
    
    /**
     * \brief A DTS Headphone:X Mobile instance.
     */
    typedef struct DTSHeadphoneXMobileInstance DTSHeadphoneXMobileInstance;


    /* Enumerations */

    /**
     * \brief Return codes.
     *
     * Return codes used by DTS Headphone:X Mobile functions.
     */
    typedef enum DTSResult
    {
        
        DTS_SUCCESS                     =  0, /**< Returned on success. */
        DTS_ERROR                       =  1, /**< Returned if a generic error is passed back from
            * the instance. Please contact your Customer Support Engineer. */
        DTS_INVALID_INPUT_ARG           =  2, /**< Returned if one or more input arguments are
            * invalid. */
        DTS_MALLOC_ERROR                =  3, /**< Returned if there is an error allocating
            * memory. */ 
        DTS_ALREADY_INITIALISED         =  4, /**< Returned if you try to initialise an instance
            * that has already been successfully initialised. */
        DTS_NOT_INITIALISED             =  5, /**< Returned if the instance has not yet been
            * initialised. */
        DTS_NOT_ENOUGH_DATA             =  6, /**< Returned if there is not enough data input for
            * processing. */
        DTS_UNSUPPORTED_CHANNEL_LAYOUT  =  7, /**< Returned if the input has an unsupported channel
            * layout. */
        DTS_DSEC_AUTHORISATION_FAILED   =  8, /**< Returned if the license key authorisation
            * failed. */
        DTS_DSEC_LICENSE_NOT_INSTALLED  =  9, /**< Returned if the license has not yet been
            * successfully installed */
    }
    DTSResult;
    
    /**
     * \brief DTS Stereo Mode enumeration.
     *
     * Stereo mode to use when processing audio.  Used in the DTSHeadphoneXMobile_InitialisePCM function.
     */
    typedef enum DTSHeadphoneXMobileStereoMode
    {
        DTS_HEADPHONE_X_MOBILE_STEREO_MODE_DIRECT = 0, /**< This mode removes any widening or narrowing of 
            * the sound stage and is provided as a reference for traditional audio playback. */
        DTS_HEADPHONE_X_MOBILE_STEREO_MODE_WIDE = 1, /**< This mode widens the sound stage for the audio 
            * coming out of the speakers, while maintaining a neutral timbre. For headphones, this essentially 
            * pulls the audio outside of the head which can lead to lower listener fatigue. For internal speakers, 
            * this provides the listener with a more immersive experience as the audio is perceived to be coming from 
            * external speakers further out to the left and right of the listener. */
        DTS_HEADPHONE_X_MOBILE_STEREO_MODE_FRONT = 2, /**< This mode moves the audio closer to the centre of the 
            * device, narrowing the sound field to mimic playing the content back over two high quality stereo 
            * speakers placed in front of the consumer. */
        DTS_HEADPHONE_X_MOBILE_STEREO_MODE_UPMIX = 3, /**< This mode will upmix the audio from 2.0 to 5.1 and render 
            * it as multichannel audio. */
    }
    DTSHeadphoneXMobileStereoMode;


    /* Functions */

    /**
     * \brief Returns the DTS Headphone:X Mobile version.
     * 
     * \return
     * - String containing the DTS Headphone:X Mobile version.
     */
    const char* DTSHeadphoneXMobile_Version();

    /**
     * \brief Installs the license key.
     *
     * Installs the license key for the DTSHeadphoneXMobileInstance. This must be called with valid
     * license key data before PCM inputs can be processed.
     *
     * \param pInstance Pointer to a DTSHeadphoneXMobileInstance that has been initialised using the
     *        DTSHeadphoneXMobile_InitialisePCM function.
     * \param pKeyFileData Pointer to a buffer containing all of the data from the license key file.
     * \param numKeyFileDataBytes The number of bytes in the data buffer pointed to by pKeyFileData.
     *
     * \return
     * - DTS_SUCCESS when the license key has been installed successfully.
     * - DTS_NOT_INITIALISED when the license key has not been installed because the
     *                       DTSHeadphoneXMobileInstance has not yet been initialised.
     * - DTS_DSEC_AUTHORISATION_FAILED if the license key authorisation failed.
     */
    uint32_t DTSHeadphoneXMobile_InstallLicense(
        DTSHeadphoneXMobileInstance* pInstance,
        const unsigned char *pKeyFileData,
        int32_t numKeyFileDataBytes);

    /**
     * \brief Retrieves the current state of the DTS Effect.
     *
     * Retrieves the current state of the DTS Effect. If the DTS Effect is enabled, the audio output
     * from the DTSHeadphoneXMobile_ProcessPCM... functions will be multichannel audio virtualised into
     * a binaural (stereo) output for headphones. If the DTS Effect is disabled, the audio output
     * from the DTSHeadphoneXMobile_ProcessPCM... functions will be multichannel audio downmixed to a
     * stereo output.
     *
     * \param pInstance Pointer to a DTSHeadphoneXMobileInstance that has been initialised using the
     *        DTSHeadphoneXMobile_InitialisePCM function.
     * \param pIsDTSEffectEnabled A pointer to a boolean. After calling this function, the
     *        pointed-to value will show the current state of the DTS Effect. If the pointed-to
     *        value is true, the DTS Effect is enabled. If the pointed-to value is false, the DTS
     *        Effect is disabled.
     *
     * \return
     * - DTS_SUCCESS on success.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     */
    uint32_t DTSHeadphoneXMobile_IsDTSEffectEnabled(
        DTSHeadphoneXMobileInstance* pInstance,
        bool* pIsDTSEffectEnabled);

    /**
     * \brief Sets the state of the DTS Effect.
     *
     * Sets the state of the DTS Effect. If the DTS Effect is enabled, the audio output from the
     * DTSHeadphoneXMobile_ProcessPCM... functions will be multichannel audio virtualised into a
     * binaural (stereo) output for headphones. If the DTS Effect is disabled, the audio output from
     * the DTSHeadphoneXMobile_ProcessPCM... functions will be multichannel audio downmixed
     * to a stereo output.
     *
     * \param pInstance Pointer to a DTSHeadphoneXMobileInstance that has been initialised using the
     *        DTSHeadphoneXMobile_InitialisePCM function.
     * \param isDTSEffectEnabled If value is true, the DTS Effect will be enabled. If value is
     *        false, the DTS Effect will be disabled.
     *
     * \return
     * - DTS_SUCCESS on success.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     */
    uint32_t DTSHeadphoneXMobile_SetDTSEffectEnabled(
        DTSHeadphoneXMobileInstance* pInstance,
        bool isDTSEffectEnabled);

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
    uint32_t DTSHeadphoneXMobile_GetOutputBufferSizeInBytes();
    

    /* PCM Input Functions */

    /**
     * \brief Initialises a DTSHeadphoneXMobileInstance for PCM.
     *
     * Allocates memory for a DTSHeadphoneXMobileInstance 
     * and then initialises it for use with PCM input data.
     * 
     * \param ppInstance Uninitialised pointer to a DTSHeadphoneXMobileInstance pointer. Should be
     *        passed in with the value nullptr so that memory can be allocated within this function.
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
    uint32_t DTSHeadphoneXMobile_InitialisePCM(
        DTSHeadphoneXMobileInstance** ppInstance,
        uint32_t channelCount,
        uint32_t sampleRate,
        DTSHeadphoneXMobileStereoMode stereoMode);

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
     * \param pInstance Pointer to a DTSHeadphoneXMobile instance that has been initialised using
     *        the DTSHeadphoneXMobile_InitialisePCM function.
     * \param ppInputBuffers Pointer to a buffer containing the audio input samples stored as
     *        non-interleaved 32-bit integers in a 2D array. The input buffer must contain 1024
     *        audio samples per channel. The expected channel order for each input layout is:
     *        - 2.0: Left, Right
     *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
     *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
     *          Left Rear Surround, Right Rear Surround
     *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
     *          Right Front Height, Left Rear Height, Right Rear Height
     * \param pOutputBuffer Pointer to a buffer that will contain the processed output audio samples
     *        after this function has been called, stored as interleaved 32-bit integers. Memory for
     *        this buffer must be pre-allocated by the calling application.
     *
     * \return
     * - DTS_SUCCESS if audio has been decoded and processed.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     * - DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
     * - DTS_ERROR if an error occurred.
     */
    uint32_t DTSHeadphoneXMobile_ProcessPCMNonInterleavedInteger(
        DTSHeadphoneXMobileInstance* pInstance,
        int32_t* const* ppInputBuffers,
        int32_t* pOutputBuffer);

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
     * \param pInstance Pointer to a DTSHeadphoneXMobile instance that has been initialised using
     *        the DTSHeadphoneXMobile_InitialisePCM function.
     * \param pInputBuffer Pointer to a buffer containing the audio input samples stored as
     *        interleaved 32-bit integers. The input buffer must contain 1024 audio samples pe
     *        channel. The expected channel order for each input layout is:
     *        - 2.0: Left, Right
     *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
     *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
     *          Left Rear Surround, Right Rear Surround
     *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
     *          Right Front Height, Left Rear Height, Right Rear Height
     * \param pOutputBuffer Pointer to a buffer that will contain the processed output audio samples
     *        after this function has been called, stored as interleaved 32-bit integers. Memory for
     *        this buffer must be pre-allocated by the calling application.
     *
     * \return
     * - DTS_SUCCESS if audio has been decoded and processed.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     * - DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
     * - DTS_ERROR if an error occurred.
     */
    uint32_t DTSHeadphoneXMobile_ProcessPCMInterleavedInteger(
        DTSHeadphoneXMobileInstance* pInstance,
        int32_t* pInputBuffer,
        int32_t* pOutputBuffer);
    
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
     * \param pInstance Pointer to a DTSHeadphoneXMobile instance that has been initialised using
     *        the DTSHeadphoneXMobile_InitialisePCM function.
     * \param ppInputBuffers Pointer to a buffer containing the audio input samples stored as
     *        non-interleaved 32-bit floating-point in a 2D array. The input buffer must contain
     *        1024 audio samples per channel. The expected channel order for each input layout is:
     *        - 2.0: Left, Right
     *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
     *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
     *          Left Rear Surround, Right Rear Surround
     *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
     *          Right Front Height, Left Rear Height, Right Rear Height
     * \param pOutputBuffer Pointer to a buffer that will contain the processed output audio samples
     *        after this function has been called, stored as interleaved 32-bit floating-point.
     *        Memory for this buffer must be pre-allocated by the calling application.
     *
     * \return
     * - DTS_SUCCESS if audio has been decoded and processed.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     * - DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
     * - DTS_ERROR if an error occurred.
     */
    uint32_t DTSHeadphoneXMobile_ProcessPCMNonInterleavedFloat(
        DTSHeadphoneXMobileInstance* pInstance,
        float* const* ppInputBuffers,
        float* pOutputBuffer);

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
     * \param pInstance Pointer to a DTSHeadphoneXMobile instance that has been initialised using
     *        the DTSHeadphoneXMobile_InitialisePCM function.
     * \param pInputBuffer Pointer to a buffer containing the audio input samples stored as
     *        interleaved 32-bit floating-point. The input buffer must contain 1024 audio samples
     *        per channel. The expected channel order for each input layout is:
     *        - 2.0: Left, Right
     *        - 5.1: Left, Right, Centre, LFE, Left Surround, Right Surround
     *        - 7.1: Left, Right, Centre, LFE, Left Side Surround, Right Side Surround,
     *          Left Rear Surround, Right Rear Surround
     *        - 5.1.4: Left, Right, Centre, LFE, Left Surround, Right Surround, Left Front Height,
     *          Right Front Height, Left Rear Height, Right Rear Height
     * \param pOutputBuffer Pointer to a buffer that will contain the processed output audio samples
     *        after this function has been called, stored as interleaved 32-bit floating-point.
     *        Memory for this buffer must be pre-allocated by the calling application.
     *
     * \return
     * - DTS_SUCCESS if audio has been decoded and processed.
     * - DTS_INVALID_INPUT_ARG if any of the input arguments are invalid.
     * - DTS_NOT_INITIALISED if the DTSHeadphoneXMobileInstance has not yet been initialised.
     * - DTS_DSEC_LICENSE_NOT_INSTALLED if a license has not yet been successfully installed.
     * - DTS_ERROR if an error occurred.
     */
    uint32_t DTSHeadphoneXMobile_ProcessPCMInterleavedFloat(
        DTSHeadphoneXMobileInstance* pInstance,
        float* pInputBuffer,
        float* pOutputBuffer);

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
    uint32_t DTSHeadphoneXMobile_ReleasePCM(
        DTSHeadphoneXMobileInstance** ppInstance);

#ifdef __cplusplus
}
#endif

#endif // __DTS_HEADPHONEX_MOBILE_H__
