/*
 * Copyright 2024 The Android Open Source Project
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
package androidx.media3.decoder.iamf;

import android.os.Handler;
import androidx.annotation.Nullable;
import androidx.media3.common.C;
import androidx.media3.common.Format;
import androidx.media3.common.MimeTypes;
import androidx.media3.common.audio.AudioProcessor;
import androidx.media3.common.util.TraceUtil;
import androidx.media3.common.util.Util;
import androidx.media3.decoder.CryptoConfig;
import androidx.media3.decoder.DecoderException;
import androidx.media3.exoplayer.audio.AudioRendererEventListener;
import androidx.media3.exoplayer.audio.AudioSink;
import androidx.media3.exoplayer.audio.DecoderAudioRenderer;
import java.util.Objects;

/** Decodes and renders audio using the native IAMF decoder. */
public class LibiamfAudioRenderer extends DecoderAudioRenderer<IamfDecoder> {

  /**
   * Creates a new instance.
   *
   * @param eventHandler A handler to use when delivering events to {@code eventListener}. May be
   *     null if delivery of events is not required.
   * @param eventListener A listener of events. May be null if delivery of events is not required.
   * @param audioProcessors Optional {@link AudioProcessor}s that will process audio before output.
   */
  public LibiamfAudioRenderer(
      @Nullable Handler eventHandler,
      @Nullable AudioRendererEventListener eventListener,
      AudioProcessor... audioProcessors) {
    super(eventHandler, eventListener, audioProcessors);
  }

  /**
   * Creates a new instance.
   *
   * @param eventHandler A handler to use when delivering events to {@code eventListener}. May be
   *     null if delivery of events is not required.
   * @param eventListener A listener of events. May be null if delivery of events is not required.
   * @param audioSink The sink to which audio will be output.
   */
  public LibiamfAudioRenderer(
      @Nullable Handler eventHandler,
      @Nullable AudioRendererEventListener eventListener,
      AudioSink audioSink) {
    super(eventHandler, eventListener, audioSink);
  }

  @Override
  protected int supportsFormatInternal(Format format) {
    return !IamfLibrary.isAvailable()
            || !Objects.equals(format.sampleMimeType, MimeTypes.AUDIO_IAMF)
        ? C.FORMAT_UNSUPPORTED_TYPE
        : C.FORMAT_HANDLED;
  }

  @Override
  protected IamfDecoder createDecoder(Format format, @Nullable CryptoConfig cryptoConfig)
      throws DecoderException {
    TraceUtil.beginSection("createIamfDecoder");
    IamfDecoder decoder = new IamfDecoder(format.initializationData);
    TraceUtil.endSection();
    return decoder;
  }

  @Override
  protected Format getOutputFormat(IamfDecoder decoder) {
    return Util.getPcmFormat(
        IamfDecoder.DEFAULT_PCM_ENCODING,
        IamfDecoder.DEFAULT_CHANNEL_COUNT,
        IamfDecoder.DEFAULT_OUTPUT_SAMPLE_RATE);
  }

  @Override
  public String getName() {
    return "LibiamfAudioRenderer";
  }
}