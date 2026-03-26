import * as fs from "node:fs";

fs.cpSync("../../src", "src/dectalk", { recursive: true });
fs.cpSync("../../include", "src/include", { recursive: true });