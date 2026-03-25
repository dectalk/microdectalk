import * as esbuild from "esbuild";

const config = {
  entryPoints: ["src/index.ts"],
  platform: "node",
  packages: "external",
  bundle: true,
};

await Promise.all([
  esbuild.build({
    ...config,
    outfile: "dist/index.cjs",
    format: "cjs",
  }),
  esbuild.build({
    ...config,
    outfile: "dist/index.mjs",
    format: "esm",
  }),
]);
