/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package androidx.media3.exoplayer.audio;

import android.content.Context;
import android.content.res.Resources;
import android.os.Handler;
import androidx.media3.common.C;
import androidx.media3.common.Format;
import androidx.media3.exoplayer.R;
import java.io.InputStream;
import java.nio.ByteBuffer;
import androidx.media3.nassau.NassauProcess;
import java.nio.ByteOrder;

/**
 * An {@link AudioProcessor} that uses the Nassau library for Source Separation Processing.
 */

/////////////////////////////////////////////////////////////
// IMPORTANT NOTE!
// Nassau Processing is only active when the input is mono.
// Otherwise, it is bypassed.
/////////////////////////////////////////////////////////////

public final class NassauAudioProcessor extends BaseAudioProcessor {

  private static Context context;

  private static final int SAMPLES_PER_FRAME = 1024;
  private static final int MAX_BYTES_PER_SAMPLE = 4; //Bytes per audio sample
  private static final int MAX_NUM_CHANNELS = 2; //Number of channels
  private static final int BYTES_PER_FRAME_MAX =
      SAMPLES_PER_FRAME * MAX_BYTES_PER_SAMPLE * MAX_NUM_CHANNELS; //Bytes per audio sample
  private static final int PRE_BUFFER_MAX = 8 * BYTES_PER_FRAME_MAX; //Maximum input buffer
  private static final int POST_BUFFER_MAX = PRE_BUFFER_MAX; //Maximum output buffer
  private static int bytesPerFrame = 0; //bytes per frame based on number of channels. One frame has SAMPLES_PER_FRAME number of samples.
  private static int bytesPerSample = 0; //bytes per sample. E.g. 16-bit PCM = 2 bytes.

  //Nassau Parameters
  private final int NASSAU_FRAME_LENGTH = 1024;
  private final int NASSAU_NUM_BANDS = 128;
  private final int NASSAU_DEFAULT_NUM_THREADS = 3;
  private Handler mHandler = new Handler();
  private String fileNameInput;
  private boolean nassauEnable, nassauComp;
  private int currentBytesRead;
  private boolean isLastFrame = false;
  private long FloatInstanceAddr;
  private long FxpInstanceAddr;
  private int float_model_file_id = R.raw.nassau_v2c_weights_float;
  private int fxp_model_file_id = R.raw.nassau_v2c_weights_fxp;
  private ByteBuffer bFloatWeight;
  private ByteBuffer bFxpWeight;

  private AudioFormat audioFormat;

  private static byte[] preBuffer; //Contains data before nassau processing
  private static byte[] postBuffer; //Contains data after nassau processing
  private static byte[] oneFrameBuffer; //Buffer to hold one frame for Nassau processing.
  private static short[] tempShortBuffer; //Short version of input buffer.

  protected static ByteBuffer PreBuffer; //wrapper for preBuffer
  protected static ByteBuffer PostBuffer; //wrapper for postBuffer
  protected static ByteBuffer OneFrameBuffer; //wrapper for oneFrameBuffer
  protected static float[] inputBuff;
  protected static float[] outputBuff;


  private boolean inputEnded;

  /**
   * Creates a new Nassau audio processor.
   */
  public NassauAudioProcessor(Context context) {
    this.context = context;
    audioFormat = AudioFormat.NOT_SET;
    preBuffer = new byte[PRE_BUFFER_MAX];
    postBuffer = new byte[POST_BUFFER_MAX];
    oneFrameBuffer = new byte[BYTES_PER_FRAME_MAX];

    //byte arrays are wrapped by ByteBuffers.
    PreBuffer = ByteBuffer.wrap(preBuffer);
    PostBuffer = ByteBuffer.wrap(postBuffer);
    OneFrameBuffer = ByteBuffer.wrap(oneFrameBuffer);

    //Allocate memory for I/O buffers.
    inputBuff = new float[NASSAU_FRAME_LENGTH];
    outputBuff = new float[NASSAU_FRAME_LENGTH];
    tempShortBuffer = new short[NASSAU_FRAME_LENGTH];

    //Init Nassau
    InitNassau();
  }

  public static void setContext(Context context2) {
    context = context2;
  }

  //Read weights and create Nassau instance
  private void InitNassau() {
    Resources res = context.getResources();
    InputStream fInputWeights = res.openRawResource(float_model_file_id);
    InputStream fxpInputWeights = res.openRawResource(fxp_model_file_id);
    int f_wgt_size = 0, fxp_wgt_size = 0;
    byte[] bFloatFile = null;
    byte[] bFxpFile = null;

    try {
      f_wgt_size = fInputWeights.available();
      bFloatFile = new byte[f_wgt_size];
      fInputWeights.read(bFloatFile);
      fInputWeights.close();

      fxp_wgt_size = fxpInputWeights.available();
      bFxpFile = new byte[fxp_wgt_size];
      fxpInputWeights.read(bFxpFile);
      fxpInputWeights.close();
    } catch (Exception e) {
      e.printStackTrace();
    }

    bFloatWeight = ByteBuffer.allocateDirect(f_wgt_size);
    bFloatWeight.order(ByteOrder.nativeOrder());
    bFloatWeight.put(bFloatFile, 0, f_wgt_size);
    bFloatWeight.flip();
    NassauProcess.NassauSetThreads(NASSAU_DEFAULT_NUM_THREADS);
    FloatInstanceAddr = NassauProcess.NassauAlloccate(bFloatWeight, f_wgt_size, NASSAU_NUM_BANDS);
    if (FloatInstanceAddr == 0) {
      throw new NullPointerException();
    }

    bFxpWeight = ByteBuffer.allocateDirect(fxp_wgt_size);
    bFxpWeight.order(ByteOrder.nativeOrder());
    bFxpWeight.put(bFxpFile, 0, fxp_wgt_size);
    bFxpWeight.flip();
    FxpInstanceAddr = NassauProcess.NassauAlloccate(bFxpWeight, fxp_wgt_size, NASSAU_NUM_BANDS);
    if (FxpInstanceAddr == 0) {
      throw new NullPointerException();
    }
  }

  @Override
  public AudioFormat onConfigure(AudioFormat inputAudioFormat)
      throws UnhandledAudioFormatException {

    if (inputAudioFormat.encoding != C.ENCODING_PCM_16BIT) {
      throw new UnhandledAudioFormatException(inputAudioFormat);
    }

    // Nassau only handles mono. But at this point of configuring, we need to allow other
    // audio formats of up to 10 channels to passthrough. Otherwise playback will stop
    // at this point.
    if ((inputAudioFormat.channelCount < 1) || (inputAudioFormat.channelCount > 10)) {
      throw new UnhandledAudioFormatException(inputAudioFormat);
    }

    //Compute number of bytes per audio sample. E.g. 16-bit PCM = 2 bytes.
    bytesPerSample = inputAudioFormat.bytesPerFrame / inputAudioFormat.channelCount;

    //Compute bytesPerFrame (1024 samples per frame) based on number of channels and number of bytes per sample in the audio.
    bytesPerFrame = inputAudioFormat.bytesPerFrame * SAMPLES_PER_FRAME;

    audioFormat =
        new AudioFormat(inputAudioFormat.sampleRate, inputAudioFormat.channelCount,
            C.ENCODING_PCM_16BIT);

    return audioFormat;
  }


  @Override
  public boolean isActive() {
    return ((audioFormat.sampleRate != Format.NO_VALUE) && (audioFormat.channelCount > 0) && (
        audioFormat.channelCount < 11));
  }

  @Override
  public void queueInput(ByteBuffer inputBuffer) {
    if (!inputBuffer.hasRemaining()) {
      return;
    }
    int inputSize = inputBuffer.remaining();

    //Append incoming input bytes to preBuffer
    inputBuffer.get(preBuffer, PreBuffer.position(), inputSize).flip();
    //Update InputBuffer's pos to reflect that one frame has been consumed.
    inputBuffer.position(inputBuffer.position() + inputSize);

    //Update PreProcessing buffer to reflect that one frame has been written into it.
    PreBuffer.position(PreBuffer.position() + inputSize);

    ProcessNassau();
  }

  @Override
  protected void onQueueEndOfStream() {
    // TODO(internal b/174554082): assert nassau is non-null here and in getOutput.
    inputEnded = true;
  }

  @Override
  public ByteBuffer getOutput() {
    int postBufferSize = PostBuffer.position();

    if (postBufferSize > 0) {

      ByteBuffer outBuffer = ByteBuffer.allocateDirect(postBufferSize);

      //Switch Post processing buffer from write to read mode
      PostBuffer.flip();

      //Copy all contents from post buffer into output buffer
      outBuffer.put(PostBuffer).flip();

      //Reset the post buffer
      PostBuffer.clear();

      return outBuffer;
    } else {
      // Return empty buffer if output samples are not ready.
      // This can happen because audio needs to be buffered until
      // 1 frame worth is reached.
      return EMPTY_BUFFER;
    }
  }

  @Override
  public boolean isEnded() {
    return inputEnded;
  }

  @Override
  public void onFlush() {
    if (isActive()) {
    }
    inputEnded = false;
    PreBuffer.clear();
    PostBuffer.clear();
    OneFrameBuffer.clear();
  }

  @Override
  public void onReset() {
    audioFormat = AudioFormat.NOT_SET;
    inputEnded = false;
    PreBuffer.clear();
    PostBuffer.clear();
    OneFrameBuffer.clear();
  }

  public void ProcessNassau() {
    //Copy 1 frame at a time from input to output buffer
    int preBufferSize = PreBuffer.position();

    //Input buffer could have more than one frame worth of audio samples
    while (preBufferSize >= bytesPerFrame) {
      PreBuffer.flip(); //switch to read mode for pre buffer.
      //Extract one frame from input buffer
      PreBuffer.get(oneFrameBuffer, PreBuffer.position(), bytesPerFrame);
      //Move input buffer back by one frame since one frame is consumed.
      PreBuffer.compact();

      if (true) {
        //Convert byte buffer to short.
        ByteBuffer.wrap(oneFrameBuffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer()
            .get(tempShortBuffer);

        // convert int16 to float
        ShortToFloat(tempShortBuffer, inputBuff);

        //////////////////////////////
        // Nassau Process one frame //
        //////////////////////////////
        NassauProcess.NassauProcess(FxpInstanceAddr, inputBuff, outputBuff, NASSAU_FRAME_LENGTH);

        //Convert from float back to short.
        FloatToShort(outputBuff, tempShortBuffer);

        //Copy short array into oneFrameBuffer
        ByteBuffer.wrap(oneFrameBuffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer()
            .put(tempShortBuffer);
      }

      //Copy oneFrameBuffer to output.
      PostBuffer.put(oneFrameBuffer, 0, bytesPerFrame);

      preBufferSize -= bytesPerFrame;
    }
  }

  private void ShortToFloat(short[] inBuff, float[] outBuff) {
    double tmp_val;
    for (int i = 0; i < inBuff.length; i++) {
      tmp_val = (double) inBuff[i] / 32768;
      outBuff[i] = (float) tmp_val;
    }
  }//ShortToFloat

  private void FloatToShort(float[] inBuff, short[] outBuff) {
    final float mul = 32768.0f;
    int tmp;

    for (int i = 0; i < inBuff.length; i++) {
      tmp = (int) (mul * inBuff[i]);
      tmp = (tmp < -32768) ? -32768 : tmp;// CLIP < 32768
      tmp = (tmp > 32767) ? 32767 : tmp; // CLIP > 32767
      outBuff[i] = (short) tmp;
    }
  }// FloatToShort

}
