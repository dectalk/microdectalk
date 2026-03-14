import bindings from "bindings";
import fs from "node:fs"

/** @type {ILibDectalk} */
const libdectalk = bindings("dectalk");

const toBytes = (number) => {
  return [0xff & number,
  0xff & (number >> 8),
  0xff & (number >> 16),
  0xff & (number >> 24)]
}

/** @type {(text: string) => Promise<Buffer>} */
const say = (text) => {
  const format = 1;
  const audioBuffer = [];
  let dataLength = 0;

  /** @type {(chunkData: Buffer) => void} */
  const callback = (chunkData) => {
    // copy the buffer immediately before its destroyed
    audioBuffer.push(Buffer.from(chunkData));
    dataLength += chunkData.byteLength;
  }

  libdectalk.setCallback(callback)
  libdectalk.say(text, format);

  const sampleRate = format ? 11025 : 8000
  const bufferLength = 44 + dataLength;
  const audioData = Buffer.concat([...audioBuffer]);

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

  return Buffer.concat([header, audioData])
}

const buffer = say("This is a message from Node J S");
fs.writeFileSync("first.wav", buffer);

const buffer2 = say("This is a second message from Node J S");
fs.writeFileSync("second.wav", buffer2);
console.log("end of programme");
