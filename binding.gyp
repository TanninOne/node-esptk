{
    "targets": [
        {
            "target_name": "esptk",
            "includes": [
                "auto.gypi"
            ],
            "sources": [
                "esptk/src/espfile.cpp",
                "esptk/src/record.cpp",
                "esptk/src/subrecord.cpp",
                "index.cpp"
            ],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'msbuild_settings': {
              'ClCompile': {
                'AdditionalOptions': ['-std:c++17']
              }
            },
            'conditions': [
                ['OS=="win"', {
                    'defines!': [
                        '_HAS_EXCEPTIONS=0'
                    ],
                    'defines': [
                        'BUILDING_NODE_EXTENSION',
                    ],
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'ExceptionHandling': 1,
                        }
                    }
                }],
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    }
                }]
            ]
        }
    ],
    "includes": [
        "auto-top.gypi"
    ]
}
