project(
  'NumericRepresentations',
  'cpp',
  version: '1.0.0',
  license: 'MIT',
  meson_version: '>=0.63.0',
  default_options: [
    'cpp_std=c++23',
    'buildtype=debugoptimized',
    'warning_level=3',
    'werror=false',
    'b_sanitize=none',
  ]
)

# =============================================================================
# COMPILER DETECTION AND FLAGS
# =============================================================================
cpp = meson.get_compiler('cpp')
is_msvc = cpp.get_id() == 'msvc'
is_gcc = cpp.get_id() == 'gcc'
is_clang = cpp.get_id() == 'clang'

# Common include directories
inc_dirs = include_directories('include')

# =============================================================================
# COMPILER-SPECIFIC FLAGS
# =============================================================================
compile_args = []

if is_msvc
  # MSVC-specific flags for template metaprogramming
  compile_args += [
    '/EHsc',                    # Standard exception handling
    '/permissive-',             # Strict C++ conformance
    '/constexpr:depth2048',     # Deep template recursion
    '/constexpr:steps1048576',  # Complex constexpr evaluation
    '/bigobj',                  # Large object files
    '/Zc:preprocessor',         # Standard-conforming preprocessor
    '/Zc:__cplusplus',          # Correct __cplusplus macro
    '/utf-8',                   # UTF-8 source encoding
  ]
  
  # MSVC optimization flags by build type
  if get_option('buildtype') == 'release'
    compile_args += ['/O2', '/Ob2', '/DNDEBUG']
  elif get_option('buildtype') == 'debug'
    compile_args += ['/Od', '/Zi', '/DEBUG']
  else
    compile_args += ['/O1', '/Zi']
  endif
  
elif is_gcc
  # GCC-specific flags
  compile_args += [
    '-Wall', '-Wextra', '-Wpedantic',
    '-Wno-unused-parameter',
    '-fconstexpr-depth=2048',
    '-fconstexpr-ops-limit=1048576',
  ]
  
  # GCC optimization by build type
  if get_option('buildtype') == 'release'
    compile_args += ['-O3', '-DNDEBUG', '-march=native']
  elif get_option('buildtype') == 'debug'
    compile_args += ['-O0', '-g3', '-fno-omit-frame-pointer']
  else
    compile_args += ['-O2', '-g1']
  endif
  
elif is_clang
  # Clang-specific flags
  compile_args += [
    '-Wall', '-Wextra', '-Wpedantic',
    '-Wno-unused-parameter',
    '-fconstexpr-depth=2048',
    '-fconstexpr-steps=1048576',
  ]
  
  # Clang optimization by build type
  if get_option('buildtype') == 'release'
    compile_args += ['-O3', '-DNDEBUG', '-march=native']
  elif get_option('buildtype') == 'debug'
    compile_args += ['-O0', '-g', '-fno-omit-frame-pointer']
  else
    compile_args += ['-O2', '-g']
  endif
endif

# Add common compile arguments
add_project_arguments(compile_args, language: 'cpp')

# =============================================================================
# DEPENDENCIES
# =============================================================================
# Catch2 dependency (managed by Conan when available, fallback to system)
catch2_dep = dependency('catch2', required: false, method: 'pkg-config')
if not catch2_dep.found()
  # Fallback: try to find Catch2 with main
  catch2_dep = dependency('catch2-with-main', required: false, method: 'pkg-config')
endif

if not catch2_dep.found()
  warning('Catch2 not found via pkg-config. Please install via Conan or system package manager.')
  catch2_dep = declare_dependency()
endif

# =============================================================================
# PROJECT OPTIONS
# =============================================================================

# =============================================================================
# LIBRARY CONFIGURATION (Header-only)
# =============================================================================
numeric_repr_lib = declare_dependency(
  include_directories: inc_dirs,
  compile_args: compile_args
)

# =============================================================================
# SUBDIRECTORIES
# =============================================================================
if get_option('enable_tests')
  subdir('tests')
endif

if get_option('enable_docs')
  subdir('docs')
endif

# =============================================================================
# SUMMARY
# =============================================================================
summary(
  {
    'C++ Standard': get_option('cpp_std'),
    'Build Type': get_option('buildtype'),
    'Compiler': cpp.get_id() + ' ' + cpp.version(),
    'Platform': host_machine.system(),
    'Tests': get_option('enable_tests'),
    'Documentation': get_option('enable_docs'),
    'Benchmarks': get_option('enable_benchmarks'),
  },
  section: 'Configuration',
  bool_yn: true
)