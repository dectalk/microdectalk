import bindings from "bindings";

interface ILibDectalk {
  setCallback: (callback: (buffer: Buffer) => void) => void;
  say: (text: string, mode: 0 | 1) => number;
}

const libdectalk: ILibDectalk = bindings("dectalk");

const toBytes = (n: number) => {
  return [
    0xff & n,
    0xff & (n >> 8),
    0xff & (n >> 16),
    0xff & (n >> 24)
  ]
}

const say = (text: string): Buffer<ArrayBuffer> => {
  const format = 1;
  const audioBuffer: Buffer<ArrayBuffer>[] = [];
  let dataLength = 0;

  const callback = (chunkData: Buffer) => {
    // copy the buffer immediately before its destroyed
    audioBuffer.push(Buffer.from(chunkData));
    dataLength += chunkData.byteLength;
  }

  libdectalk.setCallback(callback)
  libdectalk.say(text, format);

  const sampleRate = format ? 11025 : 8000
  const bufferLength = 44 + dataLength;

  // Generate a WAV header
  const header = Buffer.from([
    0x52, 0x49, 0x46, 0x46,     // RIFF
    ...toBytes(bufferLength),   // WAV size
    0x57, 0x41, 0x56, 0x45,     // WAVE
    0x66, 0x6d, 0x74, 0x20,     // fmt
    0x10, 0x00, 0x00, 0x00,     // fmt chunk size
    0x01, 0x00, 0x01, 0x00,     // Audio format 1=PCM & Number of channels 1=Mono
    ...toBytes(sampleRate),     // Sampling Frequency in Hz
    ...toBytes(sampleRate * 2), // bytes per second
    0x02, 0x00, 0x10, 0x00,     // 2=16-bit mono & Number of bits per sample
    0x64, 0x61, 0x74, 0x61,     // data
    ...toBytes(dataLength)      // data size
  ])

  return Buffer.concat([header, ...audioBuffer])
}

module.exports = say;
