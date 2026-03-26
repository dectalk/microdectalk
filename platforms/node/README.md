# DECtalk for NodeJS

A native NodeJS package -- it should run anywhere NodeJS and node-gyp runs.

## Building `/platforms/node`

```sh
# Copy the source files into the folder
node copyFiles.js

# Install
npm ci
```

## Example

```ts
// esm or cjs
const say = require("@dectalk/say");
import say from "@dectalk/say";

// synthesize audio
const buffer = say("Hello world!");
fs.writeFileSync("output.wav", buffer);
```
