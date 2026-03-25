{
  "targets": [
    {
      "target_name": "dectalk",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "src/dectalk.cc",
        "<!@(node -p \"require('fs').readdirSync('../src').map(f=>'../src/'+f).join(' ')\")"
      ],
      "include_dirs": [
        "../include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [
        'NAPI_DISABLE_CPP_EXCEPTIONS',
        'NO_FILESYSTEM',
        '_REENTRANT',
        'NOMME',
        'LTSSIM',
        'TTSSIM',
        'ANSI',
        'BLD_DECTALK_DLL',
        'ENGLISH',
        'ENGLISH_US',
        'ACCESS32',
        'TYPING_MODE',
        'ACNA',
        'DISABLE_AUDIO',
        'SINGLE_THREADED',
        '__unix__'
       ],
    }
  ]
}
