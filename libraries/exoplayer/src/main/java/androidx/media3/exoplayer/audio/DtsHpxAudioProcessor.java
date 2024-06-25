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

import static androidx.media3.dts.hpx.DtsHpxProcess.DTSHeadphoneXMobileInitialisePCM;
import static androidx.media3.dts.hpx.DtsHpxProcess.DTSHeadphoneXMobileInstallLicense;
import static androidx.media3.dts.hpx.DtsHpxProcess.DTSHeadphoneXMobileReleasePCM;
import static androidx.media3.dts.hpx.DtsHpxProcess.ResultOK;


import android.content.Context;
import android.os.Environment;
import android.os.Handler;
import androidx.media3.common.C;
import androidx.media3.common.Format;
import androidx.media3.common.util.Log;
import androidx.media3.dts.hpx.DtsHpxProcess;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/////////////////////////////////////////////////////////////
// IMPORTANT NOTE!
// DtsHpx Processing is only active when the input is mono.
// Otherwise, it is bypassed.
/////////////////////////////////////////////////////////////

public final class DtsHpxAudioProcessor extends BaseAudioProcessor {

  private static Context context;
  private static final String TAG = "DtsHpxAudioProcessor_JAVA";
  private static final String LICENSE_FILE_NAME = "dts_hpx.lic";
  private static final int DTS_HPX_OUTPUT_CHANNEL_COUNT = 2; // Output from DtsHpx is always 2-channels.
  private static final int CHANNEL_COUNT_DEFAULT = 2;
  private static final int SAMPLING_RATE_DEFAULT = 48000;
  // Stereo Mode Direct is the only available mode for non-stereo inputs.
  private static final DtsHpxProcess.StereoMode STEREO_MODE_DEFAULT = DtsHpxProcess.StereoMode.DIRECT;

  private static final int SAMPLES_PER_FRAME = 1024;
  private static final int MAX_BYTES_PER_SAMPLE = 4; //Bytes per audio sample
  private static final int MAX_NUM_CHANNELS = 10; //Number of channels
  private static final int BYTES_PER_FRAME_MAX =
      SAMPLES_PER_FRAME * MAX_BYTES_PER_SAMPLE * MAX_NUM_CHANNELS; //Bytes per audio sample
  private static final int PRE_BUFFER_MAX = 8 * BYTES_PER_FRAME_MAX; //Maximum input buffer
  private static final int POST_BUFFER_MAX = PRE_BUFFER_MAX; //Maximum output buffer
  private static int bytesPerFrame = 0; //bytes per frame based on number of channels. One frame has SAMPLES_PER_FRAME number of samples.
  private static int bytesPerSample = 0; //bytes per sample. E.g. 16-bit PCM = 2 bytes.

  //DtsHpx Parameters
  private final int DTS_HPX_FRAME_LENGTH = 1024;

  private AudioFormat audioFormat;

  private static byte[] preBuffer; //Contains data before DtsHpx processing
  private static byte[] postBuffer; //Contains data after DtsHpx processing
  private static byte[] oneFrameBuffer; //Buffer to hold one frame for DtsHpx processing.
  private static short[] tempShortBuffer; //Short version of input buffer.

  protected static ByteBuffer PreBuffer; //wrapper for preBuffer
  protected static ByteBuffer PostBuffer; //wrapper for postBuffer
  protected static ByteBuffer OneFrameBuffer; //wrapper for oneFrameBuffer
  protected static float[] inputBuff;
  protected static float[] outputBuff;

  // Public Parameters
  public static boolean enabled = true;

  private boolean inputEnded;

  /**
   * Creates a new DtsHpx audio processor.
   */
  public DtsHpxAudioProcessor(Context context) {
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
    inputBuff = new float[DTS_HPX_FRAME_LENGTH * MAX_NUM_CHANNELS];
    outputBuff = new float[DTS_HPX_FRAME_LENGTH * DTS_HPX_OUTPUT_CHANNEL_COUNT];
    tempShortBuffer = new short[DTS_HPX_FRAME_LENGTH * MAX_NUM_CHANNELS];

  }

  public static void setContext(Context context2) {
    context = context2;
  }

  // Inits and Instantiate the HPX Instance.
  private void InitDtsHpx(AudioFormat inputAudioFormat) {

    // Create & Initialise DTS Headphone:X Mobile instance.

    int result = DTSHeadphoneXMobileInitialisePCM(inputAudioFormat.channelCount, inputAudioFormat.sampleRate, STEREO_MODE_DEFAULT.ordinal());

    if (!ResultOK(result)) {
      String msg = "Error with DTSHeadphoneXMobileInitialisePCM() ErrorCode=" + result;
      Log.e(TAG, msg);
      return;
    }
    // Install Licenses file.
    byte[] license_file_buf = readLicenseDataFromFile(LICENSE_FILE_NAME);

    if (license_file_buf == null) {
      return;
    }

    result = DTSHeadphoneXMobileInstallLicense(license_file_buf);
    if (!ResultOK(result)) {
      String msg =
          "License file: " + LICENSE_FILE_NAME + " cannot be installed. ErrorCode=" + result;
      Log.e(TAG, msg);
      return;
    }
  }

  @Override
  public AudioFormat onConfigure(AudioFormat inputAudioFormat)
      throws UnhandledAudioFormatException {

    if (inputAudioFormat.encoding != C.ENCODING_PCM_16BIT) {
      throw new UnhandledAudioFormatException(inputAudioFormat);
    }

    if ((inputAudioFormat.channelCount < 1) || (inputAudioFormat.channelCount > 10)) {
      throw new UnhandledAudioFormatException(inputAudioFormat);
    }

    //Init DtsHpx
    InitDtsHpx(inputAudioFormat);

    //Compute number of bytes per audio sample. E.g. 16-bit PCM = 2 bytes.
    bytesPerSample = inputAudioFormat.bytesPerFrame / inputAudioFormat.channelCount;

    //Compute bytesPerFrame (1024 samples per frame) based on number of channels and number of bytes per sample in the audio.
    bytesPerFrame = inputAudioFormat.bytesPerFrame * SAMPLES_PER_FRAME;

    audioFormat =
        new AudioFormat(inputAudioFormat.sampleRate, inputAudioFormat.channelCount,
            C.ENCODING_PCM_16BIT);

    // If input has more than 2 channels, will need to set the output to 2 channels.
    if ((enabled) && (inputAudioFormat.channelCount > 2))
    {
      AudioFormat outputFormat =  new AudioFormat(inputAudioFormat.sampleRate, DTS_HPX_OUTPUT_CHANNEL_COUNT,
          C.ENCODING_PCM_16BIT);
      return outputFormat;
    }
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

    //HPX Process
    Process();
  }

  @Override
  protected void onQueueEndOfStream() {
    // TODO(internal b/174554082): assert dtshpx is non-null here and in getOutput.
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

    // Release HPX instance
    int result = DTSHeadphoneXMobileReleasePCM();
    if (!DtsHpxProcess.ResultOK(result)) {
      String msg = "onReset: DTSHeadphoneXMobileReleasePCM() errorCode=" + result;
      android.util.Log.e(TAG, msg);
    }
  }

  public void Process() {
    //Copy 1 frame at a time from input to output buffer
    int preBufferSize = PreBuffer.position();

    //Input buffer could have more than one frame worth of audio samples
    while (preBufferSize >= bytesPerFrame) {
      PreBuffer.flip(); //switch to read mode for pre buffer.
      //Extract one frame from input buffer
      PreBuffer.get(oneFrameBuffer, PreBuffer.position(), bytesPerFrame);
      //Move input buffer back by one frame since one frame is consumed.
      PreBuffer.compact();

      int outputBufferFactor = 1;

      if (enabled) {
        //Convert byte buffer to short.
        ByteBuffer.wrap(oneFrameBuffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer()
            .get(tempShortBuffer);

        // convert int16 to float
        if (inputAudioFormat.channelCount == 10) {
          // Special handling for DTS:X P2 which has 10 channels and needs to be remapped.
          ShortToFloatWith514Remap(tempShortBuffer, inputBuff, inputBuff.length);
        } else {
          ShortToFloat(tempShortBuffer, inputBuff, inputBuff.length);
        }

        ///////////////////////////
        // HPX Process one frame //
        ///////////////////////////
        Object[] retVal = DtsHpxProcess.DTSHeadphoneXMobileProcessPCMInterleavedFloat(inputBuff, outputBuff);
        int result = (int) retVal[0];
        if (!DtsHpxProcess.ResultOK(result)) {
          String msg = "DtsHpxProcess.DTSHeadphoneXMobileProcessPCMInterleavedFloat(): errorCode=" + result;
          android.util.Log.e(TAG, msg);
        }
        //Convert from float back to short.
        FloatToShort(outputBuff, tempShortBuffer, outputBuff.length);

        //Copy short array into oneFrameBuffer
        ByteBuffer.wrap(oneFrameBuffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer()
            .put(tempShortBuffer,0, outputBuff.length);

        outputBufferFactor = audioFormat.channelCount/2; //output is always 2 channels for HPX enabled.
      }

      //Copy oneFrameBuffer to output.
      PostBuffer.put(oneFrameBuffer, 0, bytesPerFrame/outputBufferFactor);

      preBufferSize -= bytesPerFrame;
    }
  }

  private void ShortToFloat(short[] inBuff, float[] outBuff, int length) {
    double tmp_val;
    for (int i = 0; i < length; i++) {
      tmp_val = (double) inBuff[i] / 32768;
      outBuff[i] = (float) tmp_val;
    }
  }//ShortToFloat

  private void ShortToFloatWith514Remap(short[] inBuff, float[] outBuff, int length) {
//    Output from C2 DTSX Decoder  : C L R Ls  Rs LFE Lfh Rfh Lrh Rrh
//    Input to DTS:X Mobile HPX SDK: L R C LFE Ls Rs  Lfh Rfh Lrh Rrh
    int[] remapIdx = {
        1, 2, 0, 5, 3, 4, 6, 7, 8, 9
    };
    double tmp_val;
    for (int i = 0; i < length; i += remapIdx.length) {
      for (int j = 0; j < 10; j++) {
        tmp_val = (double) inBuff[i + remapIdx[j]] / 32768;
        outBuff[i + j] = (float) tmp_val;
      }
    }
  }//ShortToFloatWithChannelRemap

  private void FloatToShort(float[] inBuff, short[] outBuff, int length) {
    final float mul = 32768.0f;
    int tmp;

    for (int i = 0; i < length; i++) {
      tmp = (int) (mul * inBuff[i]);
      tmp = (tmp < -32768) ? -32768 : tmp;// CLIP < 32768
      tmp = (tmp > 32767) ? 32767 : tmp; // CLIP > 32767
      outBuff[i] = (short) tmp;
    }
  }// FloatToShort

  byte[] readLicenseDataFromFile(String filename) {
    // Determine if license file is available.
    String extStorageState = Environment.getExternalStorageState();
    if (Environment.MEDIA_MOUNTED.equals(extStorageState)) {
      try {
        InputStream stream = context.getAssets().open(filename);
        int size = stream.available();
        byte[] buffer = new byte[size];
        stream.read(buffer);
        stream.close();
        return buffer;
      } catch (Exception e) {
        // Handle exceptions here
        String msg = "License file, %s cannot be read." + filename;
        Log.e(TAG, msg);
      }
    }

    String msg = "External storage not mounted. License file, %s cannot be read." + filename;
    Log.e(TAG, msg);
    return null;
  }
}
