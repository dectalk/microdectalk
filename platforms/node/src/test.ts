import * as fs from "node:fs"
import say from ".";

const buffer = say("This is a message from Node J S");
fs.writeFileSync("first.wav", buffer);

const buffer2 = say("This is a second message from Node J S");
fs.writeFileSync("second.wav", buffer2);
console.log("end of programme");
