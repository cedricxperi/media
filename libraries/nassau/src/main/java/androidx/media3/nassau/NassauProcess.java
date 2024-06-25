package androidx.media3.nassau;

import java.nio.ByteBuffer;

public class NassauProcess {

    private static final String LOG_TAG = NassauProcess.class.getSimpleName( );

    static {
        System.loadLibrary( "nassau-lib" );
    }

    public static native int NassauSetThreads( int nThreads );

    public static native long NassauAlloccate( ByteBuffer Weights,
                                              int nWeightSize,
                                              int numBands );

    public static native int NassauProcess( long opaque_obj,
                                            float[] input_frame,
                                            float[] output_frame,
                                            int samples);

    public static native int NassauDeallocate( long opaque_obj );

}
