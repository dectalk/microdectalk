declare module "@dectalk/node" {
    function say(input: string): Buffer<ArrayBuffer>;
    namespace say {}
    export = say;
}
