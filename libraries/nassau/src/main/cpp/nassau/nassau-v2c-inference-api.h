/**
 * \file      nassau-v2c-inference-pcm-api.h
 *
 * \brief     API header file for NASSAU V2C inference engine.
 *
 * \copyright Copyright(c) 2021 Xperi Corporation.
 *            All rights reserved.
 **/

#ifndef NASSAU_V2C_INFERENCE_API_H_
#define NASSAU_V2C_INFERENCE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nassau-common.h"

/** \file nassau-v2c-inference-api.h
*/

/** \addtogroup NASSAU_V2c_API
*  @{
*  \brief Interface of NASSAU engine.
*
*   This header file describes NASSAU V2c API. NASSAU V2c API will process 1024 floating point PCM samples in a frame.
*   NASSAU inference can operates in floating point or 8 bit integer with number of bands as 64 or 128.
*/

/** \brief NASSAU instance type */
typedef void *NassauInferenceV2c;

/** \brief                  Allocate and initialize network instance
 *  \param                  pInstance           Network instance
 *  \param                  pWeights            Network weights
 *  \param                  nWeightSize         Size of weight file in bytes
 *  \param                  nNumBands           Number of bands for inference processing. Supports : 128, 64
 *  \return                 error code
 *  \par Description:
 *
 *  *                       Trained weights available at "dsp-ml/inference/nassau/misc/data/weights/nassau_v2c_weights_08.bin"
 *                          operates in floating point NASSAU inference.
 *
 *  *                       Trained weights available at "dsp-ml/inference/nassau/misc/data/weights/nassau_v2c_weights_09.bin"
 *                          operates in 8 bit fixed point NASSAU inference.
 */
DTS_API nassau_status_t NassauInference_V2c_Allocate
(
    NassauInferenceV2c *pInstance,
    void *pWeights,
    int nWeightSize,
    int nNumBands
);

/** \brief                  Get weight file version details used by NASSAU inference
 *  \param                  pInstance           Network instance
 *  \param                  pVersion            Get weight version [pVersion[0] : Major version,
 *                                                                  pVersion[1] : Minor version,
 *                                                                  pVersion[2] : Network version,
 *                                                                  pVersion[3] : Process version]
 *  \return                 error code
 *  \par Description:
 *                          This function gets weight file version used by NASSAU inference.
 *                          This API should be called after NASSAU allocate API \ref NassauInference_V2c_Allocate().
 */
DTS_API nassau_status_t NassauInference_V2c_GetWeightVersion
(
    NassauInferenceV2c pInstance,
    unsigned char *pVersion
);

/** \brief                  Set inference denoise limit, this limit will reduce artifacts introduced during aggressive denoising.
 *  \param                  pInstance       Network instance
 *  \param                  Value           Negative float value to limit the amount of noise reduction by that many dB. Recommended limit range: Max: -10dB ~ Min: -50dB
 *                                          Default: -200dB (No denoising limit applied, maximum denoising effect at NASSAU ouput)
 *  \return                 error code
 *  \par Description:
 *                          Set inference denoise limit, this limit will reduce artifacts introduced during aggressive denoising.
 *                          This API should be called after NASSAU allocate API \ref NassauInference_V2c_Allocate().
 *
 *                          Pseudo-code for user-facing UI
 *                          \code{.py}
 *                              Recommended values for the mapping equation below:
 *                                 - nassau_min = -50
 *                                 - nassau_max = -10
 *
 *
 *                             slider = Slider(min=0, max=100, default=100)
 *
 *                             def on_change(slider):
 *                                 # map slider value to nassau parameter values
 *                                 assert slider.min <= slider.value <= slider.max
 *                                 if slider.value == 100:
 *                                     """
 *                                     NASSAU outputs at maximum denoising effect from inference, no denoising limit applied.
 *                                     """
 *                                     value = -200
 *                                 elif slider.value == 0:
 *                                     """
 *                                     No denoising effect from NASSAU. User can disable NASSAU externally
 *                                     to avoid performance overhead. But user needs to consider external
 *                                     logic to avoid glitches while disabling and enabling NASSAU due to
 *                                     NASSAU output latency.
 *                                     """
 *                                     value = 0
 *                                 else:
 *                                     ratio = (nassau_max - nassau_min) / (slider.max - slider.min)
 *                                     value = nassau_max - (slider.value - slider.min) * ratio
 *
 *                                 nassau.set_reduction_limit_db(value)
 *                         \endcode
 */
DTS_API nassau_status_t NassauInference_V2c_SetDenoiseLimit
(
    NassauInferenceV2c pInstance,
    float Value
);

/** \brief                  Process single data frame
 *  \param                  pInstance                  Network instance
 *  \param                  pfInputFrame               Float value input PCM samples
 *  \param                  pfOutputFrame              Float value output PCM samples
 *  \param                  nSamples                   Input sample count (Supports 1024 samples, single channel PCM and sampling frequency 48KHz only)
 *  \return                 error code
 *  \par Description:
 *                          This function will execute NASSAU inference and which has to be called after creating NASSAU network instance
 *                          \ref NassauInference_V2c_Allocate(). NASSAU inference inputs 1024 PCM samples and outputs 1024 PCM samples.
 *
 *
 */
DTS_API nassau_status_t NassauInference_V2c_ProcessFrame
(
    NassauInferenceV2c pInstance,
    float *pfInputFrame,
    float *pfOutputFrame,
    int nSamples
);

/** \brief                  De-initialize network instance and free allocated memory.
 *  \param                  pInstance             Network instance
 *  \return                 error code
 *  \par Description:
 *                          This function releases all resources allocated in \ref NassauInference_V2c_Allocate().
 */
DTS_API nassau_status_t NassauInference_V2c_Deallocate
(
    NassauInferenceV2c pInstance
);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* NASSAU_V2C_INFERENCE_API_H_ */
