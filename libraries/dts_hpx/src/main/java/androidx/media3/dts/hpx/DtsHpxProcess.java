package androidx.media3.dts.hpx;

public class DtsHpxProcess {
    /** Enum class used to interpret results from the JNI layer. */
    public static boolean ResultOK(int errorCode) {
        if (errorCode == DTSResult.DTS_SUCCESS.ordinal()) {
            return true;
        } else {
            return false;
        }
    }
    public static enum DTSResult {
        DTS_SUCCESS,
        DTS_ERROR,
        DTS_INVALID_INPUT_ARG,
        DTS_MALLOC_ERROR,
        DTS_ALREADY_INITIALISED,
        DTS_NOT_INITIALISED,
        DTS_NOT_ENOUGH_DATA,
        DTS_UNSUPPORTED_CHANNEL_LAYOUT,
        DTS_DSEC_AUTHORISATION_FAILED,
        DTS_DSEC_LICENSE_NOT_INSTALLED,
    }
    public static enum StereoMode {
        DIRECT,
        WIDE,
        FRONT,
        UPMIX,
    }
    private static final String LOG_TAG = DtsHpxProcess.class.getSimpleName( );

    static {
        try {
            System.loadLibrary("native-lib");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native library failed to load: " + e);
            e.printStackTrace();
            // Handle the error or rethrow it
        }
    }

    public static native String DTSHeadphoneXMobileVersion();
    public static native int DTSHeadphoneXMobileInstallLicense(byte[] licenseData);
    public static native Object[] DTSHeadphoneXMobileIsDTSEffectEnabled();
    public static native int DTSHeadphoneXMobileSetDTSEffectEnabled(boolean dtsEffectEnabled);
    public static native int DTSHeadphoneXMobileGetOutputBufferSizeInBytes();
    public static native int DTSHeadphoneXMobileInitialisePCM(int channelCount, int sampleRate, int stereoMode);
    public static native Object[] DTSHeadphoneXMobileProcessPCMNonInterleavedFloat(float[] inputData, float[] outputBuffer);
    public static native Object[] DTSHeadphoneXMobileProcessPCMInterleavedFloat(float[] inputData, float[] outputBuffer);
    public static native Object[] DTSHeadphoneXMobileProcessPCMNonInterleavedInteger(float[] inputData, float[] outputBuffer);
    public static native Object[] DTSHeadphoneXMobileProcessPCMInterleavedInteger(float[] inputData, float[] outputBuffer);
    public static native int DTSHeadphoneXMobileReleasePCM();
}
