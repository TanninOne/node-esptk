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
                "string_cast.cpp",
                "index.cpp"
            ],
            "include_dirs": [
                "<!(node -p \"require('node-addon-api').include_dir\")"
            ],
            "dependencies": [
              "<!(node -p \"require('node-addon-api').gyp\")"
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
                        'BUILDING_NODE_EXTENSION'
                    ],
                    'libraries': [
                      '-DelayLoad:node.exe',
                    ],
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'ExceptionHandling': 1,
                            'RuntimeLibrary': 0
                        },
                        "VCLibrarianTool": {
                          'AdditionalOptions': [ '/LTCG' ]
                        },
                        'VCLinkerTool': {
                          'LinkTimeCodeGeneration': 1
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
