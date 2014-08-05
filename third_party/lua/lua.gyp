# lua.gyp - for Lua 5.3.0 (alpha)
{
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {},
      'Release': {},
    },
  },
  'targets': [
    {
      'target_name': 'lua_static',
      'product_name': 'lua',
      'type': 'static_library',
      'defines': [
        'LUA_INT_INT',
      ],
      'include_dirs': [
        '.',
      ],
      'sources': [
        'src/lapi.c',
        'src/lcode.c',
        'src/lctype.c',
        'src/ldebug.c',
        'src/ldo.c',
        'src/ldump.c',
        'src/lfunc.c',
        'src/lgc.c',
        'src/llex.c',
        'src/lmem.c',
        'src/lobject.c',
        'src/lopcodes.c',
        'src/lparser.c',
        'src/lstate.c',
        'src/lstring.c',
        'src/ltable.c',
        'src/ltm.c',
        'src/lundump.c',
        'src/lutf8lib.c',
        'src/lvm.c',
        'src/lzio.c',
        'src/lauxlib.c',
        'src/lbaselib.c',
        'src/lbitlib.c',
        'src/lcorolib.c',
        'src/ldblib.c',
        'src/liolib.c',
        'src/lmathlib.c',
        'src/loslib.c',
        'src/lstrlib.c',
        'src/ltablib.c',
        'src/loadlib.c',
        'src/linit.c',
        'src/lua.c',
      ],
      'msvs_configuration_attributes': {
        #'CharacterSet': '2', # Multibyte Character Set
        #'OutputDirectory': '$(SolutionDir)$(ConfigurationName)',
      },
      'msvs_settings':{
        'VCCLCompilerTool': {
          'SuppressStartupBanner': 'true', # /nologo
          'WarningLevel': '3',             # /W3
          'Optimization': '2',             # /O2
          'InlineFunctionExpansion': '1',  # /Ob1
          'PreprocessorDefinitions': [
            'WIN32',
            'NDEBUG',
          ],
          'StringPooling': 'true',              # /GF
          'RuntimeLibrary': '0',                # /MT
          'EnableFunctionLevelLinking': 'true', # /Gy
        },
      },
      'xcode_settings': {
	    'SKIP_INSTALL': 'YES',
      },
    },
  ],# "targets"
}
