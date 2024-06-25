/**
 * \file      nassau-common.h
 *
 * \brief     Error code of nassau inference engine.
 *
 * \copyright Copyright(c) 2021 Xperi Corporation.
 *            All rights reserved.
 **/
#ifndef NASSAU_COMMON_H_
#define NASSAU_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DTS_API
#define DTS_API
#endif

/** \file nassau-common.h
*/

/** \addtogroup NASSAU_COMMON_API
*  @{
*  \brief Error code definition and common API's of NASSAU inference engine.
*
*/

/* Error codes */
typedef enum
{

    NASSAU_SUCCESS = 0,               /**< Success, no error */
    NASSAU_OUT_OF_MEMORY = 1,         /**< Out of memory error */
    NASSAU_INVAILD_ARG = 2,           /**< Invalid arguments */
    NASSAU_STFT_SAMPLE_ERROR = 3,     /**< Invalid STFT sample */
    NASSAU_WEIGHT_MEMORY_ERR = 4,     /**< Invalid weight */
    NASSAU_EXECUTION_ERROR = 5,       /**< Execution error */
    NASSAU_ALLOCATION_ERROR = 6,      /**< Allocation error */
    NASSAU_OUTPUT_WRITE_ERROR = 7,    /**< Output data write error */
    NASSAU_PCM_SAMPLE_ERROR = 8,      /**< Invalid PCM sample */

} nassau_status_t;

/** \brief                  API used for tracing execution of NASSAU inference.
 *  \param                  nMode           Enables/Disables execution trace. Range: 0 - Disable, 1 - Enable. Default: Disable
 *  \return                 error code
 *  \par Description:
 *                          API used for tracing execution of NASSAU inference.
 *                          This API should be called after NASSAU allocate API and before NSSAU process API.
 */
DTS_API nassau_status_t NassauInference_SetVerbose
(
    int nMode
);

/** \brief                  Set number of threads for NASSAU inference.
 *  \param                  nThreads      Number of threads.
 *  \return                 error code
 *  \par Description:
 *                          This API set number of threads for NASSAU inference.
 *                          API should be called before both NASSAU allocate and NASSAU process API.
 *                          If number of threads is not set by using this API, then NASSAU code runs at maximum available threads.
 */
DTS_API nassau_status_t NassauInference_SetNumThreads
(
    int nThreads
);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif
/** @} */
#endif /* NASSAU_COMMON_H_ */
