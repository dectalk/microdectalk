interface ILibDectalk {
  setCallback: (callback: (buffer: Buffer) => void) => void;
  say: (text: string, mode: 0 | 1) => number;
}
