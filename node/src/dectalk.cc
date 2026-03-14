#include <napi.h>

extern "C" {
  #include "epsonapi.h"
}

Napi::FunctionReference callback;
bool callbackWasSet = false;

short* audio_callback(short *data, long length, int phoneme) {
  Napi::Env env = callback.Env();

  Napi::Buffer<short> array = Napi::Buffer<short>::New(env, data, length);

  callback.Call({ array });
}

Napi::Value setCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  callback = Napi::Persistent(info[0].As<Napi::Function>());
  callbackWasSet = true;

  return Napi::String::New(env, "Done");
}

Napi::Value say(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::String text = info[0].As<Napi::String>();
  Napi::Number format = info[1].As<Napi::Number>();

  const std::string cppstr = text.ToString().Utf8Value();
  const char* input = cppstr.c_str();
  const int fmt = format.Int32Value();

  int result = TextToSpeechStart((char*) input, NULL, fmt);

  if (result == 0) TextToSpeechSync();

  return Napi::Number::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "setCallback"),
              Napi::Function::New(env, setCallback));

  exports.Set(Napi::String::New(env, "say"),
              Napi::Function::New(env, say));

  TextToSpeechInit(audio_callback, NULL);

  return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
